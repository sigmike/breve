#include "kernel.h"
#include "camera.h"

/** \defgroup breveObjectAPI The breve object API: constructing a new language frontend for breve

	The functions and structures in this group are used for creating 
	custom language frontends for breve.  If you wish to construct
	breve simulations from other languages, you'll need to use the 
	functions shown here to interface with the breve object API.
*/

/*@{*/

/*!
	\brief Registers a new object type with the engine.
*/

void brEngineRegisterObjectType(brEngine *e, brObjectType *t) {
	e->objectTypes.push_back(t);
}

/*!
	\brief Finds a method in an object.

	Given an object type, a name, and an argument count, this function
	finds a method and returns it as a brMethod structure.  This allows
	a pointer to the method to be cached, to avoid frequent lookups.
*/

brMethod *brMethodFind(brObject *o, char *name, unsigned char *types, int argCount) {
	brMethod *m;
	unsigned char *t = NULL;
	void *mp;
	int n;

	if(!types && argCount) {
		t = new unsigned char[ argCount ];
		types = t;
		for(n=0;n<argCount;n++) types[n] = AT_UNDEFINED;
	}

	mp = o->type->findMethod(o->userData, name, types, argCount);

	if(!mp) return NULL;

	m = new brMethod;
	m->userData = mp;
	m->argumentCount = argCount;
	m->name = slStrdup( name );

	if( t ) delete[] t;

	return m;
}

/*!
	\brief Finds a method with a range of argument counts.

	Looks for a method named "name" that accepts between min and max 
	arguments.  Will return the method with the highest number of 
	arguments possible.

	This is used for callbacks which can optionally accept arguments.
	If the user has specified that the method accepts arguments, they
	are provided.  
*/

brMethod *brMethodFindWithArgRange(brObject *o, char *name, unsigned char *types, int min, int max) {
	int n;

	for(n=max;n>=min;n--) {
		brMethod *m = brMethodFind(o, name, types, n);

		if(m) return m;
	}

	return NULL;
}

/*!
    \brief Finds an object in the given namespace

	Looks up an object in the engine's table of known objects.  If 
	the object cannot be found, \ref brUnknownObjectFind is called 
	to ask each language frontend to locate the object.
*/

brObject *brObjectFind(brEngine *e, char *name) {
	brObject *object;
	std::string names = name;

	if((object = e->objects[ names])) return object;
	if((object = e->objectAliases[ names])) return object;

	return brUnknownObjectFind(e, name);
}

/*!
	\brief Looks up an unknown object and adds it to the engine.

	Uses the language frontend callbacks to locate and register
	an object that does not currently exist in the engine.
*/

brObject *brUnknownObjectFind(brEngine *e, char *name) {
	std::vector<brObjectType*>::iterator oi;

	for(oi = e->objectTypes.begin(); oi != e->objectTypes.end(); oi++ ) {
		brObjectType *type = *oi;
		void *pointer = NULL;

		if(type->findObject) {
			pointer = type->findObject(type->userData, name);

			if(pointer) return brEngineAddObject(e, type, name, pointer);
		}
	}

	return NULL;
}

/*!
	\brief Returns the userData field of a brInstance.
*/

void *brInstanceGetUserData(brInstance *i) {
	return i->userData;
}

/*!
	\brief Returns the userData field of a brObject.
*/

void *brObjectGetUserData(brObject *o) {
	return o->userData;
}

/*!
	\brief Calls a method for an instance.

	Executes the callMethod callback for to trigger a method call.

	WARNING: the brEval values stored in args may be changed by the 
	method call, depending on the implementation of the language frontend.
*/

int brMethodCall(brInstance *i, brMethod *m, brEval **args, brEval *result) {
	if(i->status != AS_ACTIVE) {
		slMessage(DEBUG_ALL, "warning: method \"%s\" called for released instance %p\n", m->name, i);
		return EC_OK;
	}

	int r = i->object->type->callMethod(i->userData, m->userData, args, result);

	return r;
}

/*!
	\brief Find and call a method by name.

	Finds and calls a method by name.  This is ineffecient because it looks
	up the method every time, instead of caching it, so this function should
	not be used if the method is going to be called frequently.
*/

int brMethodCallByName(brInstance *i, char *name, brEval *result) {
	brMethod *m = brMethodFind(i->object, name, NULL, 0);
	int r;

	if(!m) {
		slMessage(DEBUG_ALL, "warning: unknown method \"%s\" called for instance %p of class \"%s\"\n", name, i->userData, i->object->name);
		return EC_ERROR;
	}

	r = brMethodCall(i, m, NULL, result);

	brMethodFree(m);

	return r;
}

/*!
	\brief Find and call a method by name, with arguments.

	As with \ref brMethodCallByName, this method is inefficient because
	it has to look up the method being called.  This function should 
	only be used for sporatic method calls.

	WARNING: the brEval values stored in args may be changed by the 
	method call, depending on the implementation of the language frontend.
*/

int brMethodCallByNameWithArgs(brInstance *i, char *name, brEval **args, int count, brEval *result) {
	brMethod *m = brMethodFind(i->object, name, NULL, count);
	int r;

	if(!m) {
		slMessage(DEBUG_ALL, "warning: unknown method \"%s\" called for instance %p of class \"%s\"\n", name, i->userData, i->object->name);
		return EC_ERROR;
	}

	r = brMethodCall(i, m, args, result);

	brMethodFree(m);

	return r;
}

/*!
	\brief Registers an instance as an observer.
		
	Registers "observer" as an observer of "i", waiting for the specified 
	notification.  When the notificication is announced, the method mname 
	is executed for the observer.
*/

int brInstanceAddObserver(brInstance *i, brInstance *observer, char *notification, char *mname) {
    brObserver *o;                                                           
    brMethod *method;
	unsigned char types[] = { AT_INSTANCE, AT_STRING };
 
	method = brMethodFindWithArgRange(observer->object, mname, types, 0, 2);
 
    if(!method) {                                                            
        slMessage(DEBUG_ALL, "error adding observer: could not locate method \"%s\" for class %s\n", mname, observer->object->name);
        return -1; 
    }
 
    o = new brObserver( observer, method, notification );
 
    i->observers = slListPrepend(i->observers, o);
    observer->observees = slListPrepend(observer->observees, i);

    return 0;
}   

/*!
	\brief Removes an instance from an object's observer list.

	Stops observerInstance from waiting for the specified notification from 
	instance i.
*/
    
void brEngineRemoveInstanceObserver(brInstance *i, brInstance *observerInstance, char *notification) {
    slList *observerList, *match, *last;
    brObserver *observer;

    observerList = i->observers;
    
    last = NULL;

    if(i->status == AS_FREED || observerInstance->status == AS_FREED) return;

    while(observerList) {
        observer = (brObserver*)observerList->data;

        if(observer->instance == observerInstance && (!notification || !strcmp(notification, observer->notification))) {
            match = observerList;

            if(last) last->next = observerList->next;
            else i->observers = observerList->next;

            observerList = observerList->next;

	    delete observer;
            slFree(match);
        } else {
            last = observerList;
            observerList = observerList->next;
        }
    }

    observerInstance->observees = slListRemoveData(observerInstance->observees, i);
}

/*!
	\brief Adds an object to the engine.
*/

brObject *brEngineAddObject(brEngine *e, brObjectType *t, char *name, void *pointer) {
	brObject *o;
	std::string names = name;

	if(!name || !t || !e) return NULL;

	o = new brObject;

	o->name = slStrdup(name);
	o->type = t;
	o->userData = pointer;
	o->collisionHandlers = slStackNew();

	e->objects[names] = o;

	return o;
}

/*!
	\brief Adds an alias for an object.

	An object alias is another name for an existing object.
*/

void brEngineAddObjectAlias(brEngine *e, char *name, brObject *o) {
	std::string names = name;

	e->objectAliases[name] = o;
}

/*!
	\brief Adds an instance to the breve engine.

	The instance's iterate method will be called at each iteration.
*/

brInstance *brEngineAddInstance(brEngine *e, brObject *object, void *pointer) {
	brMethod *imethod, *pmethod;
	brInstance *i;

	i = new brInstance;

	i->engine = e;
	i->object = object;
	i->status = AS_ACTIVE;
    
	i->menus = slStackNew();

	i->observers = NULL;
	i->observees = NULL;

    	i->userData = pointer;

	// it's a bit of a hack, but we need the camera to be informed of
	// new objects in the world.  the code which adds objects to the
	// world doesn't have a pointer to the camera, so we'll do it here

	if(e->camera) e->camera->setRecompile();

	// find the iterate method which we will call at each iteration

	imethod = brMethodFind(i->object, "iterate", NULL, 0);
	pmethod = brMethodFind(i->object, "post-iterate", NULL, 0);

	i->iterate = imethod;
	i->postIterate = pmethod;

	e->instancesToAdd.push_back(i);

	return i;
}

brInstance *brObjectInstantiate(brEngine *e, brObject *o, brEval **args, int argCount) {
	return brEngineAddInstance(e, o, o->type->instantiate(o->userData, args, argCount));
}

/*!
	\brief Marks a \ref brInstance as released, so that it can be removed
	from the engine.

	The instance will be removed from the engine and freed at the end of 
	the next simulation iteration.  This function should be used instead of
	\ref brInstanceFree except during simulation deallocation and cleanup.
*/

void brInstanceRelease(brInstance *i) {
	if(!i || i->status != AS_ACTIVE) return;

	// printf("adding %p for removal\n", i);

	i->engine->instancesToRemove.push_back(i);
	brInstanceFree(i);

	i->status = AS_RELEASED;
}

/*!
	\brief Removes an object from the engine.

	The object may still exist in the simulation (technically), but it
	will no longer be iterated by the engine.
*/

void brEngineRemoveInstance(brEngine *e, brInstance *i) {
	// inform the camera of the change

	std::vector<brInstance*>::iterator bi;

	if(e->camera) e->camera->setRecompile();

	bi = std::find(e->iterationInstances.begin(), e->iterationInstances.end(), i);
	if(bi != e->iterationInstances.end()) e->iterationInstances.erase(bi);

	bi = std::find(e->postIterationInstances.begin(), e->postIterationInstances.end(), i);
	if(bi != e->postIterationInstances.end()) e->postIterationInstances.erase(bi);

	bi = std::find(e->instances.begin(), e->instances.end(), i);
	if(bi != e->instances.end()) e->instances.erase(bi);
}

/*!
	\brief Frees a brMethod structure.

	If you have generated a brMethod structure with \ref brMethodFind, 
	you must free it using this method when you are done with it.
*/

void brMethodFree(brMethod *m) {
	delete m;
}

/*!
	\brief Frees a breve object.
*/

void brObjectFree(brObject *o) {
	unsigned int n;

	for(n=0;n<o->collisionHandlers->count;n++) {
		brCollisionHandler *h = (brCollisionHandler*)o->collisionHandlers->data[n];

		if(h->method) brMethodFree(h->method);
		delete h;
	}

	delete o->collisionHandlers;
	slFree(o->name);
	delete o;
}

/*!
	\brief Destroys a \ref brInstance structure.

	Immediately frees all of the data associated with a \ref brInstance 
	structure.  This function should not be used while a breve engine 
	is being actively iterated.  It may be used for cleanup when the
	engine isn't running, but otherwise use \ref brInstanceRelease instead.
*/

void brInstanceFree(brInstance *i) {
	slList *olist;
	brObserver *observer;

	if(i && i->userData) i->object->type->destroyInstance(i->userData);

    olist = slListCopy(i->observers);

    while(olist) {
        observer = (brObserver*)olist->data;
        delete observer;
        olist = olist->next;
    }

    slListFree(olist);
    slListFree(i->observers);

    i->observers = NULL;

	// removing observers will modify the observee list,
	// so copy the list first

	olist = slListCopy(i->observees);

    while(olist) {
        brEngineRemoveInstanceObserver((brInstance*)olist->data, i, NULL);
		olist = olist->next;
    }

    slListFree(olist);

    slListFree(i->observees);

	unsigned int n;

    for(n=0;n<i->menus->count;n++) {
		brMenuEntry *menu = (brMenuEntry*)i->menus->data[ n];

        slFree(menu->title);
        slFree(menu->method);
		delete menu;
    }

	delete i->menus;

	if(i->iterate) brMethodFree(i->iterate);
	if(i->postIterate) brMethodFree(i->postIterate);

	i->userData = NULL;

	// delete i;
	i->engine->freedInstances.push_back(i);
}

/*!
	\brief Adds a collision handler to an object.

    Defines a behavior for when object type "handler" collides with "collider".
    The behavior is to call the specified method in the handler object
    with the collider instance as an argument.
*/

int brObjectAddCollisionHandler(brObject *handler, brObject *collider, char *name) {
	brCollisionHandler *ch;
	bool nomethods = true;
	const int maxargs = 5;
	brMethod* method[maxargs+1];
	unsigned int n;
	unsigned char types[] = { AT_INSTANCE, AT_DOUBLE };
	for(int i=0; i<=maxargs; i++) method[i] = NULL;
	
	//Dont add a second collisionHandler for the same collider?
	for(n=0;n<handler->collisionHandlers->count;n++) {
		ch = (brCollisionHandler*)handler->collisionHandlers->data[n];
		if(ch->object == collider) return EC_STOP;
	}
	
	//searching for methods
	for(int i=0; i<=maxargs; i++){
		method[i] = brMethodFindWithArgRange(handler, name, types, i, i);
		if(method[i])nomethods = false;
	}
	
	
	if(nomethods) {
		slMessage(DEBUG_ALL, "Error adding collision handler: cannot locate method \"%s\" for class \"%s\"\n", name, handler->name);
		return EC_ERROR;
    }

	//Adding all methods
	for(int i=0; i<=maxargs; i++){
		if(method[i]){
			ch = new brCollisionHandler;
			ch->object = collider;
			ch->method = method[i];
			ch->ignore = 0;
			slStackPush(handler->collisionHandlers, ch);
		}
	}
	return EC_OK;
}

int brObjectSetIgnoreCollisionsWith(brObject *handler, brObject *collider, int ignore) {
	brCollisionHandler *ch;
	unsigned int n;

	for(n=0;n<handler->collisionHandlers->count;n++) {
		ch = (brCollisionHandler*)handler->collisionHandlers->data[n];

		if(ch->object == collider) {
			ch->ignore = ignore;
			return EC_OK;
		}
	}

	ch = new brCollisionHandler;

	ch->object = collider;
	ch->method = NULL;
	ch->ignore = ignore;

	slStackPush(handler->collisionHandlers, ch);

	return EC_OK;
}

/*@}*/
