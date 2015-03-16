// Copyright 2009-2015 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2015, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include <QSharedMemory>

#include "sstbuilder.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory sharedMemory;

    // See if this is the first instance of the app, if not, then exit
    sharedMemory.setKey("+__SST_BUILDER_KEY__");
    if(sharedMemory.attach()) {
        return -1;  // If we attach, prev instance of app has already created memory
    }

    if (!sharedMemory.create(1)) {
        return -1; // We cannot create mem
    }

    // Load the main window, and show it
    SSTBuilder w;
    w.show();

    // Run the message loop
    return a.exec();
}

