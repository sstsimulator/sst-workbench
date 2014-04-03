////////////////////////////////////////////////////////////////////////
// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    QSharedMemory SharedMemory;

    // Create the Application
    QApplication app(argc, argv);

    // See if this is the first instance of the app, if not, then exit
    SharedMemory.setKey(COREAPP_SINGLEINSTANCEKEY);
    if (SharedMemory.attach()) {
        return -1;  // If we attach, prev instance of app has already created memory
    }

    if (!SharedMemory.create(1)) {
        return -1; // We cannot create memory
    }

    // Build the Main window and show it
    MainWindow mainwin;
    mainwin.show();

    // Set the Core Application data - Used by QSettings
    // This is std formatting of the persistent data
    QCoreApplication::setApplicationName(COREAPP_APPNAME);
    QCoreApplication::setApplicationVersion(COREAPP_VERSION);
    QCoreApplication::setOrganizationName(COREAPP_ORGNAME);
    QCoreApplication::setOrganizationDomain(COREAPP_DOMAINNAME);

    // Set the Application Icon
    app.setWindowIcon(QIcon(":/images/Sandia_Icon.png"));
    
    // Execute the Application (Run the Message Pump)
    return app.exec();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// FOR FUTURE IMPLEMENTATION - A MORE SOPHISTICATED INSTANCE HANDLER
// THAT ALLOWS FOR DETECTING CHANGES TO THE SAME FILE ON THEIR VIEWS
// See: http://www.qtcentre.org/wiki/index.php?title=SingleApplication
// and: http://qt-project.org/forums/viewthread/22994
////////////////////////////////////////////////////////////////////////
