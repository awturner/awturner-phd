TEMPLATE  = lib
TARGET    = QtForm
LANGUAGE  = C++
CONFIG   += qt warn_on console staticlib
QT       += opengl

QMAKE_LFLAGS_RELEASE += /LTCG
QMAKE_CXXFLAGS_RELEASE += /Ox

DEFINES += NOMINMAX

INCLUDEPATH += $(AWT_WORK)
DESTDIR  = $(AWT_WORK)/$(ConfigurationName)

LIBS += /L$(VTK_BIN)/bin/$(ConfigurationName)
LIBS += $(VTK_LIBS)

HEADERS += include/MouseListener.h
HEADERS += include/WorldViewer.h

HEADERS += include/OpenGlWidget.h
SOURCES += OpenGlWidget.cpp

HEADERS += include/QSplitterEx.h
SOURCES += QSplitterEx.cpp

HEADERS += include/QSplitterGrid.h
SOURCES += QSplitterGrid.cpp

HEADERS += include/QtWorldViewWidget.h
SOURCES += QtWorldViewWidget.cpp

HEADERS += include/WorldTreeModel.h
SOURCES += WorldTreeModel.cpp

HEADERS += include/OpenGLWidgetMouseBehaviour.h
SOURCES += OpenGLWidgetMouseBehaviour.cpp

HEADERS += include/MoveCameraMouseBehaviour.h
SOURCES += MoveCameraMouseBehaviour.cpp

HEADERS += include/PointPickMouseBehaviour.h
SOURCES += PointPickMouseBehaviour.cpp

HEADERS += include/Notifiable.h

HEADERS += include/QtForm.h
SOURCES += QtForm.cpp
FORMS   += QtForm.ui

SOURCES += StateMachineThread.cpp
HEADERS += include/StateMachineThread.h