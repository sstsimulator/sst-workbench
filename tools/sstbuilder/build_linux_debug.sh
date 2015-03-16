#!/bin/sh
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

TARGET=LINUX_DEBUG

## Create the Target Directory and cd into it
mkdir -p $TARGET
cd $TARGET

## Build the application
qmake ../sstbuilder.pro -r -spec linux-g++ CONFIG+=debug CONFIG-=release CONFIG+=declarative_debug CONFIG+=qml_debug
make 

## Move back up one directory
cd ..

