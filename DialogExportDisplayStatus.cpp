////////////////////////////////////////////////////////////////////////
// Copyright 2009-2018 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2018, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#include "DialogExportDisplayStatus.h"
#include "ui_DialogExportDisplayStatus.h"

//////////////////////////////////////////////////////////////////////////////

DialogExportDisplayStatus::DialogExportDisplayStatus(QString& ExportResults, QStringList& ExportErrorWarningList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExportDisplayStatus)
{
    // Setup the Dialog
    ui->setupUi(this);

    // Populate the Dialog Controls
    ui->Results->setText(ExportResults);

    // Display all the warnings
    for (int x = 0; x < ExportErrorWarningList.count(); x++) {
        new QListWidgetItem(ExportErrorWarningList.at(x), ui->ResultsList);
    }
}

DialogExportDisplayStatus::~DialogExportDisplayStatus()
{
    delete ui;
}
