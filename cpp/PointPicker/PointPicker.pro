TEMPLATE  = app
TARGET    = PointPicker
LANGUAGE  = C++
CONFIG   += qt warn_on console xml
QT       += opengl

QMAKE_LFLAGS_RELEASE += /LTCG
QMAKE_CXXFLAGS_RELEASE += /Ox

DEFINES += NOMINMAX

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


INCLUDEPATH += $(AWT_WORK)
DESTDIR  = $(AWT_WORK)/$(ConfigurationName)

LIBS += /L$(VTK_BIN)/bin/$(ConfigurationName)
LIBS += $(VTK_LIBS)

SOURCES += PointPicker.cpp

FORMS += FormPointPicker.ui
HEADERS += include/FormPointPicker.h
SOURCES += FormPointPicker.cpp

HEADERS += include/PointListModel.h
SOURCES += PointListModel.cpp

HEADERS += include/PickedPoint.h
SOURCES += PickedPoint.cpp