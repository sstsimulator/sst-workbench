#!/bin/sh
## Copyright 2009-2018 NTESS. Under the terms
## of Contract DE-NA0003525 with NTESS, the U.S.
## Government retains certain rights in this software.
##
## Copyright (c) 2009-2018, NTESS
## All rights reserved.
##
## Portions are copyright of other developers:
## See the file CONTRIBUTORS.TXT in the top level directory
## the distribution for more information.
##
## This file is part of the SST software package. For license
## information, see the LICENSE file in the top level directory of the
## distribution.

TARGET=MAC_RELEASE
APP=SSTWorkbench

## Create the Target Directory and cd into it
mkdir -p $TARGET
cd $TARGET

## Build the application
qmake ../$APP.pro -r -spec macx-clang CONFIG-=debug CONFIG+=release CONFIG+=x86_64
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

