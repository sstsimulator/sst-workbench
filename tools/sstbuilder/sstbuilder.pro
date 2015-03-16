## Copyright 2009-2015 Sandia Corporation. Under the terms
## of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
## Government retains certain rights in this software.
##
## Copyright (c) 2009-2015, Sandia Corporation
## All rights reserved.
##
## This file is part of the SST software package. For license
## information, see the LICENSE file in the top level directory of the
## distribution.

#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T14:26:16
#
#-------------------------------------------------

macx {
    CONFIG(debug) {
        message("QMAKE RUNNING FOR MAC-OSX DEBUG")
        DESTDIR = MAC_DEBUG
    } else {
        message("QMAKE RUNNING FOR MAC-OSX RELEASE")
        DESTDIR = MAC_RELEASE
    }
}

linux {
    CONFIG(debug) {
        message("QMAKE RUNNING FOR LINUX DEBUG")
        DESTDIR = LINUX_DEBUG
    } else {
        message("QMAKE RUNNING FOR LINUX RELEASE")
        DESTDIR = LINUX_RELEASE
    }
}

QT += core gui widgets printsupport

TARGET = sstbuilder

TEMPLATE = app

###OBJECTS_DIR = $${DESTDIR}/obj
###MOC_DIR = $${DESTDIR}/moc
###RCC_DIR = $${DESTDIR}/rcc
###UI_DIR = $${DESTDIR}/ui

OBJECTS_DIR = $${OUT_PWD}/obj
MOC_DIR = $${OUT_PWD}/moc
RCC_DIR = $${OUT_PWD}/rcc
UI_DIR = $${OUT_PWD}/ui

SOURCES += main.cpp \
           sstbuilder.cpp \
           configfileparser.cpp \
           entrydata.cpp \
           headerdata.cpp \
           persistantdata.cpp \
           setupgeneraldialog.cpp \
           setupenvdialog.cpp \
           aboutdialog.cpp \
           tabpageentry.cpp

HEADERS += sstbuilder.h \
           configfileparser.h \
           entrydata.h \
           headerdata.h \
           persistantdata.h \
           setupgeneraldialog.h \
           setupenvdialog.h \
           aboutdialog.h \
           tabpageentry.h

FORMS += sstbuilder.ui \
         setupgeneraldialog.ui \
         setupenvdialog.ui \
         aboutdialog.ui

RESOURCES +=

OTHER_FILES += \
    build_linux_release.sh \
    build_mac_release.sh \
    build_linux_debug.sh \
    build_mac_debug.sh
