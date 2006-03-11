TEMPLATE        = app
LANGUAGE        = C++

CONFIG  += qt warn_on release opengl thread

REQUIRES	= opengl

LIBS    += -L../lib -lbreve -lode -lgsl -lpng -lqgame++ -lpush -lavformat -lavcodec -lgslcblas -lreadline -lhistory -lncurses -ltermcap -lsndfile -lportaudio -ltiff -ljpeg -lexpat -lz  -lm -lglut -lGLU -lGL -lXmu -lXi  -lX11  -lSM -lICE 

mac:LIBS        += -framework OpenGL

INCLUDEPATH     += ../include/breve

HEADERS += breveGLWidget.h \
        brqtEngine.h \
        brqtMethodPopup.h \
        brqtSteveSyntaxHighlighter.h

SOURCES += breveGLWidget.cpp \
        main.cpp \
        brqtEngine.cpp \
        brqtMethodPopup.cpp

FORMS   = brqtMainWindow.ui \
        brqtLogWindow.ui \
        brqtObjectInspector.ui \
        brqtEditorWindow.ui \
        brqtErrorWindow.ui

IMAGES  = app_icon.png \
        pause.png \
        play.png \
        stop.png


LEXSOURCES      += brqtQuickparser.l

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
