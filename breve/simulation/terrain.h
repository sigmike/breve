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

/*!
	\brief Serialized \ref slTerrain data.
*/

struct slSerializedTerrain {
	int side;
	int repeating;
	double xscale;
	double yscale;

	float *values;
};

/*!
	\brief Terrain data.
*/

#ifdef __cplusplus
class slTerrain: public slWorldObject {
	public:
		slTerrain() : slWorldObject() {}
		~slTerrain();

		void draw(slCamera *camera);

		float **matrix;
		slVector **fnormals[2];
		slVector **vnormals;

		int initialized;

		int side;
		int repeating;

		int drawList;
		int drawMode;

		double xscale;
		double yscale;

		float h;
	
		double heightDelta;
		double heightMin;

		// color at the valleys and peaks

		slVector bottomColor;
		slVector topColor;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
slTerrain *slTerrainNew(int res, double xscale, void *userData);
void slGenerateFractalTerrain(slTerrain *l, double h, double height);

void slTerrainSetLocation(slTerrain *l, slVector *location);
void slTerrainSetScale(slTerrain *l, double scale);

void slFractalTerrainMatrix(slTerrain *l, double h, double height);
void slTerrainBoundingBox(slTerrain *l);
float slAverageDiamondValues(slTerrain *l, int x, int y, int jump);

void slTerrainInitialize(slTerrain *l);

void slTerrainFree(slTerrain *l);
void slTerrainMakeNormals(slTerrain *l);

int slTerrainTestPair(slVclipData *vc, int x, int y, slCollisionEntry *ce);

void slTerrainFacesUnderRange(slTerrain *l, 
	double minX, double maxX, double minZ, double maxZ,
	int *startx, int *endx, int *startz, int *endz,
	int *earlyStart, int *lateEnd);

int slTerrainPlaneUnderPoint(slTerrain *l, slVector *point, slPlane *plane);
int slTerrainSphereClip(slVclipData *vc, slTerrain *l, int x, int y, slCollisionEntry *ce, int flip);
int slTerrainShapeClip(slVclipData *vc, slTerrain *l, int obX, int obY, slCollisionEntry *ce, int flip);
double slPointTerrainClip(slTerrain *t, slPosition *pp, slPoint *p, slCollisionEntry *ce);

int slTerrainEdgePlaneClip(slVector *start, slVector *end, slFace *face, slPosition *position, slPlane *facePlane, slCollisionEntry *ce);

void slDrawTerrain(slTerrain *l, int texture, double textureScale, int drawMode, int flags);
void slDrawTerrainSide(slTerrain *l, int texture, double textureScale, int drawMode, int flags, int bottom);

int slPointIn2DTriangle(slVector *vertex, slVector *a, slVector *b, slVector *c);

slSerializedTerrain *slSerializeTerrain(slTerrain *t, int *size);

void slTerrainSetHeight(slTerrain *t, int x, int y, double height);
double slTerrainGetHeight(slTerrain *t, int x, int y);

void slTerrainSetTopColor(slTerrain *t, slVector *color);
void slTerrainSetBottomColor(slTerrain *t, slVector *color);
#ifdef __cplusplus
}
#endif
