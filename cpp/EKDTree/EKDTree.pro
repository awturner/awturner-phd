#----------------------------------------------------------------------------------------------
# File: $RCSfile: EKDTree.pro,v $
# Module path: $Source: /CodePrototypes/EKDTree/EKDTree.pro,v $
# Revision: $Revision: 13 $
# Date: $Date: 2008-06-09 09:01:45 +0100 (Mon, 09 Jun 2008) $
# Author: $Author: aturner $
# 
# Created By: Andy Turner 2007/02/16
# 
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.
# 
#----------------------------------------------------------------------------------------------
#
# REVISION HISTORY:
#----------------------------------------------------------------------------------------------

# ----------------------------------------------------------
# QT project file for EKDTree Module
# ----------------------------------------------------------

TEMPLATE       = lib
TARGET         = EKDTree
LANGUAGE       = C++
CONFIG        += qt warn_on staticlib console
UI_DIR         = src/QTForms

# Compiler flags for Visual Studio .Net
# /GR ... Enables run-time type info, which is required for ITK
# /Od, /Ox  ... debug flags (disabled and full respectively)
win32-msvc.net {
   OBJECTS_DIR = $(ConfigurationName)
   
   QMAKE_CXXFLAGS_DEBUG   += /GR /Od

   QMAKE_CXXFLAGS_RELEASE += /GR /Ox
}

win32-msvc2005 {
   OBJECTS_DIR = $(ConfigurationName)
   
   QMAKE_CXXFLAGS_DEBUG   += /GR /Od

   QMAKE_CXXFLAGS_RELEASE += /GR /Ox
}

unix {
   UI_DIR      = .ui
   MOC_DIR     = .moc
   OBJECTS_DIR = .obj
}

INCLUDEPATH += $(ACROBOT_PROTOTYPES)/EKDTree/include/

HEADERS += include/EKDTree/EKDSearcher.h
HEADERS += include/EKDTree/EKDTree.h
HEADERS += include/EKDTree/EKDTreeBranch.h
HEADERS += include/EKDTree/FirstRayIntersectionSearch.h
HEADERS += include/EKDTree/KDTreeDataElement.h
HEADERS += include/EKDTree/NearestPointSearch.h
HEADERS += include/EKDTree/PointDataElement.h
HEADERS += include/EKDTree/SimpleVectorOps.h
HEADERS += include/EKDTree/TriangleDataElement.h