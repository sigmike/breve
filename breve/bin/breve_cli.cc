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

/*
	+ breve.c
	= this is a hack-ish wrapper around the breve engine for a command line
	= based GLUT version of breve.  this version is used mainly just for 
	= testing of the engine without dealing with the whole OSX app.
*/

#define OSMESA_WINDOW_SIZE 300

#include <stdio.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "steve.h"
#include "breve.h"

#include "glIncludes.h"

double gNotify = 40.0;
double gSimMax = 0.0;

void brCatchSignal(int signal);

char *gSimFile;

int gSlave = 0, gMaster = 0;
char *gSlaveHost;

int gShouldQuit = 0;

extern char *interfaceID;

breveFrontend *frontend;

int main(int argc, char **argv) {
	int index;
	char *text;
	double nextNotify;
	char wd[10240];

	srand(time(NULL));
	srandom(time(NULL));

	interfaceID = "cli/1.9.1";

	gSimFile = NULL;

#ifdef MEMORY_DEBUG
	slMessage(DEBUG_ALL, "Running with breve slMalloc debugging enabled.\n\n");
#ifdef MACOSX
	slMessage(DEBUG_ALL, "Set the environment variable MallocStackLogging\n");
	slMessage(DEBUG_ALL, "to see stack traces of unfreed pointers on exit.\n\n");
#endif /* MACOSX */
#endif /* MEMORY_DEBUG */

	/* parse the command line args. */

	index = brParseArgs(argc, argv);

	/* offset argc and argv to account for the options parsed out */

	argc -= index;
	argv += index;

	if((argc) < 2) {
		brPrintUsage(argv[0]);
		exit(1);
	}

	signal(SIGINT, brCatchSignal);

    frontend = breveFrontendInit(argc, argv);
    frontend->data = breveFrontendInitData(frontend->engine);

	brEngineSetIOPath(frontend->engine, getcwd(wd, 10239));

	frontend->engine->camera = slNewCamera(400, 400, GL_POLYGON);
	frontend->engine->camera->enabled = 0;
	frontend->engine->camera->updated = 0;

	frontend->engine->argc = argc - 1;
	frontend->engine->argv = argv + 1;

	frontend->engine->getLoadname = getLoadname;
	frontend->engine->getSavename = getSavename;
	frontend->engine->dialogCallback = brCLIDialogCallback;
	frontend->engine->interfaceTypeCallback = interfaceVersionCallback;

#ifdef HAVE_LIBOSMESA
	frontend->engine->osBuffer = slMalloc(OSMESA_WINDOW_SIZE * OSMESA_WINDOW_SIZE * 4 * sizeof(GLubyte));
	frontend->engine->osContext = OSMesaCreateContext(OSMESA_RGBA, NULL);
	frontend->engine->useOSMesa = 1;

	if(!OSMesaMakeCurrent(frontend->engine->osContext, steveData->engine->osBuffer, GL_UNSIGNED_BYTE, OSMESA_WINDOW_SIZE, OSMESA_WINDOW_SIZE)) 
		slMessage(DEBUG_ALL, "Could not activate offscreen rendering context\n");

	frontend->engine->camera->x = OSMESA_WINDOW_SIZE;
	frontend->engine->camera->y = OSMESA_WINDOW_SIZE;
#endif

	text = slUtilReadFile(argv[1]);

	if(!text) {
		fprintf(stderr, "Error reading file \"%s\"\n", argv[1]);
		exit(1);
	}

	if(gSimFile) {
		if(breveFrontendLoadSavedSimulation(frontend, text, argv[1], gSimFile) != EC_OK) {
			slFree(text);
			slFree(gSimFile);
			brQuit(frontend->engine);
		}

		slFree(gSimFile);
	} else {
		if(breveFrontendLoadSimulation(frontend, text, argv[1]) != EC_OK) {
			slFree(text);
			brQuit(frontend->engine);
		}
	}

	slFree(text);

	nextNotify = gNotify;

	if(gMaster) slWorldStartNetsimServer(frontend->engine->world);

	if(gSlave) {
		slWorldStartNetsimSlave(frontend->engine->world, gSlaveHost);
		slFree(gSlaveHost);
	}

	while(!gShouldQuit && brEngineIterate(frontend->engine) == EC_OK) {
		if(gNotify && frontend->engine->world->age > nextNotify) {
			printf("%f seconds elapsed\n", frontend->engine->world->age);
			nextNotify += gNotify;
		}

		if(gSimMax > 0.0 && frontend->engine->world->age >= gSimMax) {
			printf("%f simulation seconds completed\n", gSimMax);
			brQuit(frontend->engine);
		}

#ifdef HAVE_LIBOSMESA
		if(frontend->engine->world->detectLightExposure) slDetectLightExposure(steveData->engine->world, steveData->engine->camera, OSMESA_WINDOW_SIZE, steveData->engine->osBuffer);
#endif
	}

	brQuit(frontend->engine);

	return 0;
}

void brCatchSignal(int signal) {
	char *line;
	static int waiting = 0;

	if(waiting) return;

	waiting = 1;

	brPauseTimer(frontend->engine);

	printf("\n\nSimulation interupted.  Type a steve command, 'x' to quit, or hit enter to continue\n");
	fflush(stdout);

	line = readline("breve> ");
	if(*line) add_history(line);

	if(*line && line[0] == 'x') {
		brUnpauseTimer(frontend->engine);
		gShouldQuit = 1;
		waiting = 0;
		return;
	}

	if(*line && line[0] == '\n') {
		waiting = 0;
		return;
	}

	stRunSingleStatement(frontend->data, frontend->engine, line);

	brUnpauseTimer(frontend->engine);

	waiting = 0;
}

int brParseArgs(int argc, char **argv) {
	int r, error = 0;
	int level;

	while((r = getopt_long(argc, argv, "t:d:r:f:n:l:vhS:M", gCLIOptions, NULL)) != EOF) {
		switch(r) {
			case 'd':
				level = atoi(optarg);
				printf("debug level: %d\n", level);
				slSetDebugLevel(level);
				break;
			case 'r':
				srandom(atoi(optarg));
				srand(atoi(optarg));
				printf("random seed: %d\n", atoi(optarg));
				break;
			case 'n':
				gNotify = atof(optarg);
				printf("notification every %f seconds\n", gNotify);
				break;
			case 't':
				gSimMax = atof(optarg);
				printf("running for %f seconds\n", gSimMax);
				break;
			case 'l':
				gSimFile = slStrdup(optarg);
				break;
			case 'h':
				brPrintUsage(argv[0]);
				break;
			case 'v':
				brPrintVersion();
				break;
			case 'S':
				gSlaveHost = slStrdup(optarg);
				gSlave = 1;
				break;
			case 'M':
				gMaster = 1;
				break;
			default:
				printf("unknown option: '%c'\n", r);
				error++;
		}
	}

	if(error) brPrintUsage(argv[0]);

	return optind - 1;
}

void brPrintUsage(char *name) {
	fprintf(stderr, "usage: %s [options] simulation_file\n", name);
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -t, --terminate <seconds>  Stops the simulation after <sim_seconds>\n");
	fprintf(stderr, "  -n, --notify <seconds>     Prints a message every time <notify_seconds>\n");
	fprintf(stderr, "                             have passed\n");
	fprintf(stderr, "  -r, --random <seed>        Sets the random seed to <seed>.\n");
	fprintf(stderr, "  -a, --archive <xml_file>   Dearchive simulation from <xml_file>.  <xml_file> should be\n");
	fprintf(stderr, "                             an archive of the simulation contained in the input file.\n");
	fprintf(stderr, "  -v, --version              Display the current version number.\n");
	fprintf(stderr, "  -h, --help                 Display this information.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "For full documentation, or to submit a bug report, visit the breve homepage:\n");
	fprintf(stderr, "http://www.spiderland.org/breve\n\n");
	exit(1);
}

void brQuit(brEngine *e) {
    double diff;

    brPauseTimer(e);

    diff = e->realTime.tv_sec + (e->realTime.tv_usec / 1000000.0);

    if(e->world->age != 0.0) {
        printf("%f simulated seconds elapsed\n", e->world->age);
        printf("%f real seconds elapsed\n", diff);
        printf("%f simulated/real\n", e->world->age/diff);
    }

    breveFrontendCleanupData(frontend->data);
	brEngineFree(frontend->engine);
	slFree(frontend);

#ifdef MEMORY_DEBUG
    slMemoryReport();
	slUtilMemoryUnfreed();
#endif

    exit(0);
}

int brCLIDialogCallback(void *data, char *title, char *message, char *b1, char *b2) {
	int result;

	while (1) {
		printf("%s\n\n%s\n[Y/N]? ", title, message);

		result = getc(stdin);
		getc(stdin);

		if(result == EOF) {
			printf("EOF found, assuming answer is N\n");
			return 0; 
		}

		if(result == 'n' || result == 'N') return 0;

		if(result == 'y' || result == 'Y') return 1;
	} 
}

char *interfaceVersionCallback(void *data) {
	return interfaceID;
}

char *getSavename(void *data) {
	char *name = slMalloc(1024);
	printf("filename to save: ");
	fgets(name, 1023, stdin);
	if(strlen(name) > 0) name[strlen(name) - 1] = 0;
	return name;
}

char *getLoadname(void *data) {
	char *name = slMalloc(1024);
	printf("filename to load: ");
	fgets(name, 1023, stdin);
	if(strlen(name) > 0) name[strlen(name) - 1] = 0;
	return name;
}
