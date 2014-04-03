#!/bin/sh
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

TARGET=MAC_DEBUG
APP=SSTWorkbench

## Create the Target Directory and cd into it
mkdir -p $TARGET
cd $TARGET

## Build the Application
qmake ../$APP.pro -r -spec macx-clang CONFIG+=debug CONFIG-=release CONFIG+=x86_64 CONFIG+=declarative_debug CONFIG+=qml_debug
make 

## On Mac, copy the .plist and .icns files into the .app (directory)
cd ..
cp DistributionSupportFiles/Info.plist $TARGET/$APP.app/Contents/
cp DistributionSupportFiles/$APP.icns $TARGET/$APP.app/Contents/Resources/

## On Mac, perform macdeployqt to copy the QT framework components into the .app 
## .app (directoryu) to make the .app a self contained unit
macdeployqt $TARGET/$APP.app -dmg

## Run otool to verify libraries are internal
otool -L $TARGET/$APP.app/Contents/MacOS/$APP

