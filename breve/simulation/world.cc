/*****************************************************************************
 *                                                                           *
 * The breve Simulation Environment                                          *
 * Copyright (C) 2000, 2001, 2002, 2003 Jonathan Klein                       *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *****************************************************************************/

int gPhysicsError;
char *gPhysicsErrorMessage;
 
#include "simulation.h"

void slODEErrorHandler(int errnum, const char *msg, va_list ap) {
	static char error[2048];

	vsnprintf(error, 2047, msg, ap);
	gPhysicsErrorMessage = (char*)error;	
}

/*
	+ slWorldNew 
	= creates a new empty world.	
*/

slWorld *slWorldNew() {
	slWorld *w;
	slVector g;

	gPhysicsError = 0;
	gPhysicsErrorMessage = NULL;

	w = slMalloc(sizeof(slWorld));
	bzero(w, sizeof(slWorld));

	slAllocIntegrationVectors(w);

	w->odeWorldID = dWorldCreate();
	// dWorldSetQuickStepNumIterations(w->odeWorldID, 100);

	dWorldSetCFM (w->odeWorldID,1e-6);
	dWorldSetERP(w->odeWorldID,0.1);
	dSetErrorHandler(slODEErrorHandler);
	dSetMessageHandler(slODEErrorHandler);

	w->odeCollisionGroupID = dJointGroupCreate(0);
	w->odeJointGroupID = dJointGroupCreate(0);

	w->initialized = 0;

	w->cameras = slStackNew();

	w->maxObjects = 8;
	w->objects = slMalloc(sizeof(slWorldObject*) * w->maxObjects);
	w->objectCount = 0;

	w->maxPatchGrids = 4;
	w->patchGrids = slMalloc(sizeof(slPatchGrid*) * w->maxPatchGrids);
	w->patchGridCount = 0;

	w->age = 0.0;

	w->detectCollisions = 1;

	w->collisionCallback = NULL;
	w->collisionCheckCallback = NULL;

	w->integrator = slEuler;

	w->boundingBoxOnly = 0;

	w->backgroundTexture = -1;
	slVectorSet(&w->backgroundTextureColor, 1, 1, 1);

	w->clipData = NULL;

	w->clipData = slVclipDataNew();

	slVectorSet(&g, 0.0, -9.81, 0.0);
	slWorldSetGravity(w, &g);

	return w;
}

/*
	slWorldFree

	frees an slWorld object, including all of its objects and its clipData.
*/

void slWorldFree(slWorld *w) {
	int n;

	for(n=0;n<w->objectCount;n++) slWorldFreeObject(w->objects[n]);
	for(n=0;n<w->patchGridCount;n++) slFreePatchGrid(w->patchGrids[n]);

	if(w->clipData) slFreeClipData(w->clipData);
	if(w->proximityData) slFreeClipData(w->proximityData);
	if(w->objects) slFree(w->objects);
	if(w->patchGrids) slFree(w->patchGrids);
	if(w->cameras) slStackFree(w->cameras);

	dWorldDestroy(w->odeWorldID);
	dJointGroupDestroy(w->odeCollisionGroupID);
	dJointGroupDestroy(w->odeJointGroupID);

	slFreeIntegrationVectors(w);
	slFree(w);
}

void slWorldAddCamera(slWorld *w, slCamera *camera) {
	slStackPush(w->cameras, camera);
}

void slWorldRemoveCamera(slWorld *w, slCamera *camera) {
	slStackRemove(w->cameras, camera);
}

void slRenderWorldCameras(slWorld *w) {
	int n;

	for(n=0;n<w->cameras->count;n++) {
		slCamera *c = w->cameras->data[n];
		slDrawWorld(w, c, 0, GL_RENDER, 0, 1);
	}
}

/*
	slWorldFreeObject
   
	what do you think it does?

	okay, i should tell you that it frees the object, and decreases the 
	reference count on its slShape through freeShape
*/

void slWorldFreeObject(slWorldObject *o) {
	slWorldObject *otherObject;

	if(!o) return;

	switch(o->type) {
		case WO_STATIONARY:
			if(o->data) {
				slFreeShape(((slStationary*)o->data)->shape);
				slFree(o->data);
			}
			break;
		case WO_LINK:
			if(o->data) slLinkFree(o->data);
			break;
		case WO_TERRAIN:
			if(o->data) slFreeTerrain(o->data);
			break;
		default:
			slMessage(DEBUG_ALL, "slWorldFreeObject called with unknown object type [%d]: %p\n", o->type, o);
			break;
	}

	slStackFree(o->neighbors);

	// so why not iterate the list as normal ( while(l) l = l->next )?
	// because slRemoveObjectLine is destructive and will change the 
	// list with each iteration.  so we'll have to go back to the start
	// each time.  not pretty.  and yet, not ugly either.

	while(o->inLines) {
		otherObject = o->inLines->data;
		slRemoveObjectLine(otherObject, o);
	}

	while(o->outLines) {
		otherObject = ((slObjectLine*)o->outLines->data)->destination;
		slRemoveObjectLine(o, otherObject);
	}

	slFree(o);
}

slWorldObject *slAddObject(slWorld *w, void *p, int type, slVector *color) {
	slWorldObject *no;

	w->initialized = 0;

	if(w->objectCount == w->maxObjects) {
		w->maxObjects *= 2;
		w->objects = slRealloc(w->objects, w->maxObjects*sizeof(slWorldObject*));
	}

	no = w->objects[w->objectCount] = slWorldNewObject(p, type, color);

	w->objectCount++;

	return no;
}

slPatchGrid *slAddPatchGrid(slWorld *w, slVector *center, slVector *patchSize, int x, int y, int z) {
	slPatchGrid *g = slNewPatchGrid(center, patchSize, x, y, z);

	if(w->patchGridCount == w->maxPatchGrids) {
		w->patchGridCount *= 2;
		w->patchGrids = slRealloc(w->patchGrids, w->patchGridCount * sizeof(slPatchGrid*));
	}

	w->patchGrids[w->patchGridCount++] = g;

	return g;
}

/*!
	\brief Removes an object from the world.

	Removes the object p, shift all the other objects down.
*/

void slRemoveObject(slWorld *w, slWorldObject *p) {
	int n, found = 0;

	for(n=0;n<w->objectCount;n++) {
		if(found) {
			w->objects[n - 1] = w->objects[n];
		} else if(w->objects[n] == p) {
			w->objects[n] = NULL;
			found = 1;
		}
	}

	w->objectCount--;

	if(found) w->initialized = 0;
}

/*!
	\brief Creates a new slWorldObject struct.

	Takes a void pointer, which must correspond to a slWorldObjectType
	(currently a stationary or link) and a type and creates a new slWorldObject 
	to be placed in the slWorld.
*/

slWorldObject *slWorldNewObject(void *d, int type, slVector *color) {
	slWorldObject *w;

	w = slMalloc(sizeof(slWorldObject));
	bzero(w, sizeof(slWorldObject));

	if(!w) return NULL;

	w->type = type;
	w->data = d;

	w->drawMode = 0;

	w->userData = NULL;

	w->texture = -1;
	w->textureMode = 0;
	w->textureScale = 16;

	w->proximityRadius = 0.00001;
	w->neighbors = slStackNew();

	w->billboardRotation = 0;
	w->alpha = 1.0;

	w->inLines = NULL;
	w->outLines = NULL;

	w->e = 0.4; 
	w->eT = 0.2;
	w->mu = 0.15;

	if(color) slVectorCopy(color, &w->color);
	else w->color.x = w->color.y = w->color.z = 1.0;

	/* type specific initializations */

	switch(type) {
		case WO_LINK:
		case WO_STATIONARY:
		case WO_TERRAIN:
			break;
		default:
			slMessage(DEBUG_ALL, "slWorldNewObject called with unknown object type: %d\n", type);
			return NULL;
			break;
	}

	return w;
}

/*!
	\brief Creates a new stationary object.
*/

slStationary *slNewStationary(slShape *s, slVector *loc, double rot[3][3]) {
	slStationary *so;

	s->referenceCount++;
 
	so = slMalloc(sizeof(slStationary));

	so->shape = s;
	slVectorCopy(loc, &so->position.location);
	slMatrixCopy(rot, so->position.rotation);

	return so;
}

/*!
	\brief A callback to qsort to sort world objects with multibodies first.
*/

int slObjectSortFunc(const void *a, const void *b) {
	slWorldObject *sa, *sb;

	sa = *(slWorldObject**)a;
	sb = *(slWorldObject**)b;

	if(!sa) return 1;
	if(!sb) return -1;

	if(sa->type == WO_LINK && sb->type != WO_LINK) return -1;
	if(sa->type != WO_LINK && sb->type == WO_LINK) return 1;
	else return 0;
}

/*!
	\brief Runs the world for deltaT seconds, in steps of the given size.

	Makes repeated calls to \ref slWorldStep to step the world forward
	deltaT seconds.
*/

double slRunWorld(slWorld *w, double deltaT, double step, int *error) {
	double total = 0.0;

	*error = 0;

	if(!w->initialized) slVclipDataInit(w);

	while(total < deltaT && !*error) 
		total += slWorldStep(w, step, error);

	w->age += total;

	return total;
}

/*!
	\brief Takes a single simulation step.
*/

double slWorldStep(slWorld *w, double stepSize, int *error) {
	int n, result, simulated = 0;

	n = 0;

	/* only step forward to the end of the multibodies */

	while(n < w->objectCount && w->objects[n] && w->objects[n]->type == WO_LINK) {
		double dt = stepSize;
		slLink *link;
		link = (slLink*)w->objects[n]->data;

		if(link->simulate) {
			slLinkUpdatePositions(link);
			slLinkApplyJointControls(link);
			simulated++;
		} else {
			w->integrator(w, link, &dt, 0);
			slLinkSwapConfig(link);
			slLinkUpdatePosition(link);
		}

		if(w->detectCollisions) slLinkUpdateBoundingBox(link);
		n++;
	}

	if(w->detectCollisions) {
		result = slVclip(w->clipData, 0.0, 0, w->boundingBoxOnly);

		if(result == -1) {
			slMessage(DEBUG_ALL, "warning: error in vclip\n");
			(*error)++;
			return 0;
		}

		for(n=0;n<w->clipData->collisionCount;n++) {
			slCollisionEntry *c;
			slWorldObject *w1;
			slWorldObject *w2;
			slPairEntry *pe;
			int x;

			c = w->clipData->collisions[n];

			w1 = w->objects[c->n1];
			w2 = w->objects[c->n2];

			if(c->n1 > c->n2) pe = &w->clipData->pairList[c->n1][c->n2];
			else pe = &w->clipData->pairList[c->n2][c->n1];

			if(w1 && w2 && ((w1->type != WO_LINK || ((slLink*)w1->data)->simulate) && (w2->type != WO_LINK || ((slLink*)w2->data)->simulate))) {
				dBodyID bodyX = NULL, bodyY = NULL;
				dJointID id;
				double maxDepth = 0.0;
				double mu = 0;
				double e = 0;

				if(w1->type == WO_LINK) {
					slLink *l = w1->data;
					bodyX = l->odeBodyID;
				}
				
				mu = w1->mu + w2->mu;
				e = w1->e + w2->e;

				if(w2->type == WO_LINK) {
					slLink *l = w2->data;
					bodyY = l->odeBodyID;
				}

				mu /= 2;
				e /= 2;

				mu += 1.0;

				for(x=0;x<c->pointCount;x++) {
					dContact *contact, con;

					contact = &con;

					bzero(contact, sizeof(dContact));
					contact->surface.mode = dContactSoftERP|dContactApprox1|dContactBounce;
					// contact->surface.mode = dContactSoftERP|dContactBounce;

					//contact->surface.soft_erp = 0.05;
					contact->surface.soft_cfm = 0.001;
					contact->surface.soft_erp = 0.05;
					contact->surface.mu = mu;
					// contact->surface.soft_cfm = 0.01;
					contact->surface.bounce = e;
					contact->surface.bounce_vel = .05;

					if(c->pointDepths[x] < -0.10) {
						/* this is a scenerio we might want to analyze */

						c->pointDepths[x] = -0.05;

						slMessage(DEBUG_WARN, "warning: point depth = %f for pair (%d, %d) -- cheating\n", c->pointDepths[x], c->n1, c->n2);
					}

					if(c->pointDepths[x] < maxDepth) maxDepth = c->pointDepths[x];
	
					contact->geom.depth = -c->pointDepths[x];
					// contact->geom.depth = -.001;
					contact->geom.g1 = NULL;
					contact->geom.g2 = NULL;

					// printf("%d, %d\n", pe->odeBodyIDX, pe->odeBodyIDY);
					// slVectorPrint(&c->normal);
					// printf("%d\n", x);
					// slVectorPrint(&c->worldPoints[x]);
	
					contact->geom.normal[0] = -c->normal.x;
					contact->geom.normal[1] = -c->normal.y;
					contact->geom.normal[2] = -c->normal.z;

					contact->geom.pos[0] = c->worldPoints[x].x;
					contact->geom.pos[1] = c->worldPoints[x].y;
					contact->geom.pos[2] = c->worldPoints[x].z;

					// printf("contacting %p, %p\n", bodyX, bodyY);
	
					id = dJointCreateContact(w->odeWorldID, w->odeCollisionGroupID, contact);
					dJointAttach(id, bodyY, bodyX);
				}

				// printf("t = %f, max: %f ", w->age, maxDepth);
				// slVectorPrint(&c->normal);
			}

			if((pe->flags & BT_CALLBACK) && w->collisionCallback && w1 && w2) {
				w->collisionCallback(w1->userData, w2->userData, CC_NORMAL);
			}
		}
	}

	if(simulated != 0) {
		dWorldStep(w->odeWorldID, stepSize);

		// dWorldQuickStep(w->odeWorldID, stepSize);
		// FILE *f = fopen ("myfile.DIF","wb");
		// dWorldExportDIF (w->odeWorldID, f, "");
		// fclose (f);

		dJointGroupEmpty(w->odeCollisionGroupID);

		if(gPhysicsError) (*error)++;
	}

	return stepSize;
}

void slNeighborCheck(slWorld *w) {
	int n;
	double dist;
	slVector *location, diff, *l1 = NULL, *l2 = NULL;
	slStationary *st;
	slPairEntry *pe;
	slWorldObject *o1, *o2;

	if(!w->initialized) slVclipDataInit(w);

	if(!w->proximityData) {
		w->proximityData = slVclipDataNew();
		slVclipDataRealloc(w->proximityData, w->objectCount);
		slInitProximityData(w);
		slInitBoundSort(w->proximityData);
		slVclip(w->proximityData, 0.0, 1, 0);
	}

	/* update all the bounding boxes first */

	for(n=0;n<w->objectCount;n++) {
		if(w->objects[n]->type == WO_LINK) {
			slLink *link = w->objects[n]->data;
		   
			location = &link->position.location;
		} else if(w->objects[n]->type == WO_STATIONARY) {
			st = w->objects[n]->data;

			location = &st->position.location;
		} else {
			location = NULL;
		}

		if(location) {
			w->objects[n]->min.x = location->x - w->objects[n]->proximityRadius;
			w->objects[n]->min.y = location->y - w->objects[n]->proximityRadius;
			w->objects[n]->min.z = location->z - w->objects[n]->proximityRadius;
			w->objects[n]->max.x = location->x + w->objects[n]->proximityRadius;
			w->objects[n]->max.y = location->y + w->objects[n]->proximityRadius;
			w->objects[n]->max.z = location->z + w->objects[n]->proximityRadius;

			slStackClear(w->objects[n]->neighbors);
		}
	}

	/* vclip, but stop after the pruning stage */

	slVclip(w->proximityData, 0.0, 1, 0);

	for(n=0;n<w->proximityData->candidateCount;n++) {
		pe = w->proximityData->collisionCandidates[n];

		if(pe) {
			o1 = w->objects[pe->x];
			o2 = w->objects[pe->y];


			switch(o1->type) {
				case WO_STATIONARY:
					l1 = &((slStationary*)o1->data)->position.location;
					break;
				case WO_LINK: 
					l1 = &((slLink*)o1->data)->position.location;
					break;
				case WO_TERRAIN:
					l1 = &((slTerrain*)o1->data)->position;
					break;
			}
		
			switch(o2->type) {
				case WO_STATIONARY:
					l2 = &((slStationary*)o2->data)->position.location;
					break;
				case WO_LINK: 
					l2 = &((slLink*)o2->data)->position.location;
					break;
				case WO_TERRAIN:
					l2 = &((slTerrain*)o2->data)->position;
					break;
			}

			slVectorSub(l1, l2, &diff);
			dist = slVectorLength(&diff);

			if(dist < o1->proximityRadius) slStackPush(o1->neighbors, o2);
			if(dist < o2->proximityRadius) slStackPush(o2->neighbors, o1);
		}
	}
}

void slWorldSetGravity(slWorld *w, slVector *gravity) {
	dWorldSetGravity(w->odeWorldID, gravity->x, gravity->y, gravity->z);
}

slObjectLine *slAddObjectLine(slWorldObject *src, slWorldObject *dst, int stipple, slVector *color) {
	slObjectLine *line;

	if(src == dst) return NULL;

	line = slFindObjectLine(src, dst);

	// stipple = 0xfc;

	if(line) {
		slVectorCopy(color, &line->color);
		line->stipple = stipple;
		return line;
	}

	line = slMalloc(sizeof(slObjectLine));

	line->stipple = stipple;
	line->destination = dst;
	slVectorCopy(color, &line->color);

	src->outLines = slListPrepend(src->outLines, line);
	dst->inLines = slListPrepend(dst->inLines, src);

	return line;
}

int slRemoveObjectLine(slWorldObject *src, slWorldObject *dst) {
	slList *lines = src->outLines;
	slObjectLine *line;
	int d = 0;

	while(lines) {
		line = lines->data;

		if(line->destination == dst) {
			src->outLines = slListRemoveData(src->outLines, line);
			dst->inLines = slListRemoveData(dst->inLines, src);

			return 0;
		}

		lines = lines->next;
		d++;
	}

	return -1;
}

int slRemoveAllObjectLines(slWorldObject *src) {
	slObjectLine *line;
	slWorldObject *dst;

	while(src->outLines) {
		line = src->outLines->data;
		dst = line->destination;

		src->outLines = slListRemoveData(src->outLines, line);
		dst->inLines = slListRemoveData(dst->inLines, src);
	}

	return 0;
}

slObjectLine *slFindObjectLine(slWorldObject *src, slWorldObject *dst) {
	slList *lines = src->outLines;
	slObjectLine *line;

	while(lines) {
		line = lines->data;

		if(line->destination == dst) return line;

		lines = lines->next;
	}

	return NULL;
}
