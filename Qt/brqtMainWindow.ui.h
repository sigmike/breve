/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <sys/types.h>
#include <dirent.h>
#include <QString.h>

#include "kernel.h"
#include "steve.h"
#include "brqtEditorWindow.h"
#include "brqtEngine.h"

breveFrontend *frontend;
brqtEditorWindow *gW;

void demoMenuForDirectory(QPopupMenu *menu, QString &directory) {
	int count, n;
	struct dirent **demos;

	count = scandir(directory.ascii(), &demos, NULL, alphasort);

	for(n=count-1;n>-1;n--) {
		QString name = demos[n]->d_name;

		if(demos[n]->d_type == DT_DIR) {
			QPopupMenu *submenu = new QPopupMenu(0, name);

			if(name != ".." && name != ".") {
				QString subname = directory + "/" + name;
				
				demoMenuForDirectory(submenu, subname);

				if(submenu->count()) menu->insertItem( name, submenu, 0, 0);
			}	

		} else {
			if(name.endsWith(".tz")) menu->insertItem( name, 0, 0);
		}
	}
}

void brqtMainWindow::init() {
	QString directory = "/Users/jk/dev/breve/demos";
	demoMenuForDirectory(Demos, directory);
}

void brqtMainWindow::fileNew()
{
    brqtEditorWindow *w = new brqtEditorWindow( 0);
	
    w->setCaption("Untitled");
    w->show();
    
    gW = w;
}


void brqtMainWindow::fileOpen()
{

}


void brqtMainWindow::fileSave()
{

}


void brqtMainWindow::fileSaveAs()
{

}


void brqtMainWindow::filePrint()
{

}


void brqtMainWindow::fileExit()
{
    exit(0);
}


void brqtMainWindow::editUndo()
{

}


void brqtMainWindow::editRedo()
{

}


void brqtMainWindow::editCut()
{

}


void brqtMainWindow::editCopy()
{

}


void brqtMainWindow::editPaste()
{

}


void brqtMainWindow::editFind()
{

}


void brqtMainWindow::helpIndex()
{

}


void brqtMainWindow::helpContents()
{

}


void brqtMainWindow::helpAbout()
{

}


void brqtMainWindow::toggleSimulation()
{
    brqtEditorWindow *w = gW;
    
    frontend = breveFrontendInit(0, NULL);
    frontend->data = breveFrontendInitData(frontend->engine);
    frontend->engine->camera = slCameraNew(400, 400);

    breveFrontendLoadSimulation(frontend, w->getString(), "<untitled>");

    new brqtEngine(frontend->engine, breveGLWidget1);
    
}