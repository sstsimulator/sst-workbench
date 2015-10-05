##########################################################################
## Copyright 2009-2014 Sandia Corporation. Under the terms
## of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
## Government retains certain rights in this software.
##
## Copyright (c) 2009-2014, Sandia Corporation
## All rights reserved.
##
## This file is part of the SST software package. For license
## information, see the LICENSE file in the top level directory of the
## distribution.
##########################################################################

#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T14:52:41
#
#-------------------------------------------------

message("QMAKE RUNNING FOR SSTWORKBENCH")

macx {
    CONFIG(debug, debug|release) {
        message("--- BUILDING FOR MAC-OSX DEBUG")
        DESTDIR = MAC_DEBUG
    }
    CONFIG(release, debug|release) {
        message("--- BUILDING FOR MAC-OSX RELEASE")
        DESTDIR = MAC_RELEASE
    }
}

linux {
    CONFIG(debug, debug|release) {
        message("--- BUILDING FOR LINUX DEBUG")
        DESTDIR = LINUX_DEBUG
    }
    CONFIG(release, debug|release) {
        message("--- BUILDING FOR LINUX RELEASE")
        DESTDIR = LINUX_RELEASE
    }
}

message("BUILDING USING QT VERSION = "$$QT_MAJOR_VERSION"."$$QT_MINOR_VERSION"."$$QT_PATCH_VERSION)

QT += core gui widgets xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SSTWorkbench
TEMPLATE = app

OBJECTS_DIR = $${OUT_PWD}/obj
MOC_DIR = $${OUT_PWD}/moc
RCC_DIR = $${OUT_PWD}/rcc
UI_DIR = $${OUT_PWD}/ui

# Use Precompiled headers (PCH) if possible
PRECOMPILED_HEADER = GlobalIncludes.h

SOURCES += \
    Main.cpp \
    MainWindow.cpp \
    MainWindow_UI_Handling.cpp \
    MainWindow_Print_Handling.cpp \
    MainWindow_Page_Handling.cpp \
    DialogPortsConfig.cpp \
    DialogExportDisplayStatus.cpp \
    DialogPreferences.cpp \
    DialogDynamicPropertiesConfig.cpp \
    DialogManageModules.cpp \
    DialogAddEditProperty.cpp \
    WiringScene.cpp \
    WiringView.cpp \
    WindowItemProperties.cpp \
    WindowComponentToolbox.cpp \
    SSTInfoData.cpp \
    SSTInfoXMLFileParser.cpp \
    GraphicItemBase.cpp \
    GraphicItemComponent.cpp \
    GraphicItemText.cpp \
    GraphicItemWire.cpp \
    GraphicItemWireHandle.cpp \
    GraphicItemWireLineSegment.cpp \
    GraphicItemPort.cpp \
    ItemProperties.cpp \
    PortInfoData.cpp \
    SupportClasses.cpp \
    UndoRedoCommands.cpp \
    PythonExporter.cpp

HEADERS += \
    GlobalDef.h \
    GlobalIncludes.h \
    MainWindow.h \
    DialogPortsConfig.h \
    DialogExportDisplayStatus.h \
    DialogPreferences.h \
    DialogDynamicPropertiesConfig.h \
    DialogManageModules.h \
    DialogAddEditProperty.h \
    WiringScene.h \
    WiringView.h \
    WindowItemProperties.h \
    WindowComponentToolbox.h \
    SSTInfoData.h \
    SSTInfoXMLFileParser.h \
    GraphicItemBase.h \
    GraphicItemComponent.h \
    GraphicItemText.h \
    GraphicItemWire.h \
    GraphicItemWireHandle.h \
    GraphicItemWireLineSegment.h \
    GraphicItemPort.h \
    ItemProperties.h \
    PortInfoData.h \
    SupportClasses.h \
    UndoRedoCommands.h \
    PythonExporter.h

RESOURCES += \
    SSTWorkbench.qrc

FORMS += \
    DialogPortsConfig.ui \
    DialogExportDisplayStatus.ui \
    DialogPreferences.ui \
    DialogManageModules.ui \
    DialogDynamicPropertiesConfig.ui \
    DialogAddEditProperty.ui

OTHER_FILES += \
    build_linux_release.sh \
    build_mac_release.sh \
    build_linux_debug.sh \
    build_mac_debug.sh

