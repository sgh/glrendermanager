
INCLUDES = -I/usr/share/qt4/mkspecs/linux-g++ -I /usr/include/freetype2 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4

CFLAGS = -pipe -O2 -Wall
CXXFLAGS = -D_REENTRANT -DQT_WEBKIT -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED 
LIBS = -lGL -lGLU -lQtGui -lQtCore -lpng -lpthread

simplegl:
	moc -o moc_simplegl.h simplegl.h
	uic -o ui_mainwidget.h mainwidget.ui 
# 	g++ -c ${FLAGS} ${CXXFLAGS} ${INCLUDES} moc_simplegl.cpp 
	g++ -c ${FLAGS} ${CXXFLAGS} ${INCLUDES} glcompositor.cpp
	g++ -c ${FLAGS} ${CXXFLAGS} ${INCLUDES} simplegl.cpp 
	g++ -o simplegl *.o ${LIBS}

clean:
	rm -f simplegl simplegl.o glcompositor.o moc_* ui_*
