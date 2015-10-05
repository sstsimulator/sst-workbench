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
// NOTE: MainWindow Processing routines are located in MainWindow.cpp.
//       The routines here are for the MainWindow Print processing.
//       The files are separated to reduce single file code bloat.
////////////////////////////////////////////////////////////

void MainWindow::PrintStandardSetup(QPrinter& Printer, QPainter& Painter)
{
    Q_UNUSED(Printer)

    QFont PrintFont;

    PrintFont = QFont("Tahoma",8);
    Painter.setFont(PrintFont);
}

void MainWindow::PrintWiringWindow(QPrinter& Printer, QPainter& Painter, int& PageNum)
{
    // Print the Header
    PrintPageHeader(Printer, Painter, PageNum);

    // Render the current view to the Painter
    m_CurrentWiringView->PrintRenderer(&Painter);
}

void MainWindow::PrintComponentParameters(QPrinter& Printer, QPainter& Painter, int& PageNum)
{
    int                   FontHeight;
    int                   LinePos;
    QRect                 PageRect;
    GraphicItemComponent* ptrComponent;
    int                   NumProperties;
    ItemProperties*       Properties;
    ItemProperty*         Property;
    QString               OutputLine;
    QStringList           OutputList;
    int                   x;
    QString               CompUserName;
    QString               PropName;
    QString               PropValue;
    int                   StartingLinePos;
    int                   EndingLinePos;

    // Get information on the Height of the Font, and the Height of the Page
    FontHeight = QFontMetrics(Painter.font()).height();
    PageRect = Printer.pageRect();

    // Walk through each Component, and get its Name and properties, and add them to the List of QStrings
    // These strings will be printed out at the bottom of this function.
    foreach (QGraphicsItem* item, m_CurrentWiringScene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemComponent::Type) {
            // Get the item as a Graphic Ccomponent item, and then get its properties
            ptrComponent = (GraphicItemComponent*)item;
            Properties = ptrComponent->GetItemProperties();
            NumProperties = Properties->GetNumProperties();
            if (ptrComponent->GetComponentType() != COMP_SSTSTARTUPCONFIGURATION) {
                CompUserName = Properties->GetPropertyValue(COMPONENT_PROPERTY_USERNAME);
            }
            else {
                CompUserName = "SST Startup Configuration";
            }
            OutputLine = QString("Component: %1").arg(CompUserName);
            OutputList.append(OutputLine);

            // Walk all the properties for this Component and add them to the output
            for (x = 0; x < NumProperties; x++) {
                Property = Properties->GetProperty(x);
                PropName = Property->GetName();
                PropValue = Property->GetValue();
                if (PropValue.isEmpty() == true) {
                    PropValue = "<EMPTY>";
                }
                OutputLine = QString("\tProp: %1 = %2").arg(PropName).arg(PropValue);
                OutputList.append(OutputLine);
            }
            // Add a blank line
            OutputList.append("");
        }
    }


    /////////////////////////////////////////////////////////////////
    // Now Print the Strings of data

    // Create a new page (this will also print the headers)
    PrintStartNewPage(Printer, Painter, PageNum);

    // Set the Staring the Ending positions for the lines
    StartingLinePos = 3 * FontHeight;
    EndingLinePos = PageRect.height() - (3 * FontHeight);

    // Start the text at the starting position
    LinePos = StartingLinePos;

    // Take the QString List and send each line one by one to the printer
    for (x = 0; x < OutputList.size(); x++) {
        OutputLine = OutputList.at(x);
        Painter.drawText(0, LinePos, OutputLine);
        LinePos += FontHeight;

        // Check stop printing on this page within a few lines of the bottom
        if (LinePos > EndingLinePos) {
            // We are at the bottom, Create a new page
            PrintStartNewPage(Printer, Painter, PageNum);
            LinePos = StartingLinePos;
        }
    }
}

void MainWindow::PrintStartNewPage(QPrinter& Printer, QPainter& Painter, int& PageNum)
{
    // Create a New Page & increment the page number
    Printer.newPage();
    PageNum++;

    // Print the Header for the new Page
    PrintPageHeader(Printer, Painter, PageNum);
}

void MainWindow::PrintPageHeader(QPrinter& Printer, QPainter& Painter, int PageNum)
{
    QRect   PageRect;
    QString Title;
    QString PageNumStr;

    PageRect = Printer.pageRect();
    Title = m_TabWiringWindow->tabText(0);
    PageNumStr = QString("%1").arg(PageNum);

    Painter.drawText(0, 0, PageRect.width(), PageRect.height(), Qt::AlignTop|Qt::AlignHCenter, Title);
    Painter.drawText(0, 0, PageRect.width(), PageRect.height(), Qt::AlignBottom|Qt::AlignLeft, m_LoadedProjectDataFilePathName);
    Painter.drawText(0, 0, PageRect.width(), PageRect.height(), Qt::AlignBottom|Qt::AlignRight, PageNumStr);
}

void MainWindow::MenuActionHandlerPrint()
{
    QPrinter     Printer;
    int          CurrentPageNum = 1;

    //  Ask the user what printer to go to
    if (QPrintDialog(&Printer).exec() == QDialog::Accepted) {
        // Setup the Document Name

        // Build the painter canvas using the printer info
        QPainter Painter(&Printer);
        Painter.setRenderHint(QPainter::Antialiasing);

        // Setup the Printer and the Painter
        PrintStandardSetup(Printer, Painter);

        // Render the Wiring View to the Printer
        PrintWiringWindow(Printer, Painter, CurrentPageNum);

        // Print all the parameters of each component
        PrintComponentParameters(Printer, Painter, CurrentPageNum);
    }
}


