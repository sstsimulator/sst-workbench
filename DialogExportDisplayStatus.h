////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////

#ifndef DIALOGEXPORTDISPLAYSTATUS_H
#define DIALOGEXPORTDISPLAYSTATUS_H

#include "GlobalIncludes.h"

//#include "GraphicItemComponent.h"

//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class DialogExportDisplayStatus;
}

//////////////////////////////////////////////////////////////////////////////

class DialogExportDisplayStatus : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExportDisplayStatus(QString& ExportResults, QStringList& ExportErrorWarningList, QWidget *parent = 0);
    ~DialogExportDisplayStatus();

private:
    Ui::DialogExportDisplayStatus *ui;
};

#endif // DIALOGEXPORTDISPLAYSTATUS_H
