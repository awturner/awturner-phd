TEMPLATE  = app
TARGET    = QtSandbox
LANGUAGE  = C++
CONFIG   += qt warn_on console
QT       += opengl

QMAKE_LFLAGS_RELEASE += /LTCG
QMAKE_CXXFLAGS_RELEASE += /Ox

DEFINES += NOMINMAX

INCLUDEPATH += $(AWT_WORK)
DESTDIR  = $(AWT_WORK)/$(ConfigurationName)

LIBS += /L$(VTK_BIN)/bin/$(ConfigurationName)
LIBS += $(VTK_LIBS)

LIBS += LuaPlus_1100.lib

#D:\Toolkits\xvxl-1.8.0-bin\lib\release\
LIBS += /LD:/Toolkits/xvxl-1.8.0-bin/lib/$(ConfigurationName)
LIBS += v3p_netlib.lib
LIBS += vcl.lib
LIBS += vnl.lib
LIBS += vnl_algo.lib

LIBS += /LD:/MimlabEnvironment/bin/release
LIBS += MimlabCore.lib
LIBS += MimlabMaths.lib
LIBS += MimlabMatrices.lib
LIBS += MimlabMatricesImpls.lib
LIBS += MimlabMatricesTNT.lib

SOURCES += CollisionDemo.cpp
SOURCES += LoadingThread.cpp
SOURCES += QtSandbox.cpp
SOURCES += VTKTorusDemo.cpp

HEADERS += CollisionDemo.h
HEADERS += LoadingThread.h
HEADERS += VTKTorusDemo.h