CONFIG   += qt warn_on console xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
INSTALLS += target sources

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qmng qgif
}

LIBS += LuaPlus_1100.lib

SOURCES += main.cpp

HEADERS += imageviewer.h
SOURCES += imageviewer.cpp

HEADERS += SliceDisplayWidget.h
SOURCES += SliceDisplayWidget.cpp

HEADERS += EditWidget.h
SOURCES += EditWidget.cpp

HEADERS += MyScrollArea.h
SOURCES += MyScrollArea.cpp

HEADERS += SegmentationModel.h
SOURCES += SegmentationModel.cpp

HEADERS += SegmentationSlice.h
SOURCES += SegmentationSlice.cpp
