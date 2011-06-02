TEMPLATE  = app
TARGET    = MicroscribeLogger
LANGUAGE  = C++
CONFIG   += qt warn_on

QMAKE_LFLAGS_RELEASE += /LTCG
QMAKE_CXXFLAGS_RELEASE += /Ox

DEFINES += NOMINMAX

INCLUDEPATH += $(AWT_WORK)
DESTDIR  = $(AWT_WORK)/$(ConfigurationName)

SOURCES += MicroscribeLogger.cpp

FORMS   += Forms/MicroscribeLoggerForm.ui
HEADERS += include/MicroscribeLoggerForm.h
SOURCES += MicroscribeLoggerForm.cpp

HEADERS += include/DrawPositionWidget.h
SOURCES += DrawPositionWidget.cpp

HEADERS += include/PositionHistory.h
SOURCES += PositionHistory.cpp

LIBS += armdll32.lib