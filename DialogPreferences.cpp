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

#include "DialogPreferences.h"
#include "ui_DialogPreferences.h"

//////////////////////////////////////////////////////////////////////////////

DialogPreferences::DialogPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreferences)
{
    ui->setupUi(this);
    setWindowTitle("Preferences");
}

DialogPreferences::~DialogPreferences()
{
    delete ui;
}

void DialogPreferences::SetFlagReturnToSelectAfterWirePlaced(bool flag)
{
    ui->ReturnToSelectAfterWire->setChecked(flag);
}

void DialogPreferences::SetFlagReturnToSelectAfterTextPlaced(bool flag)
{
    ui->ReturnToSelectAfterText->setChecked(flag);
}

bool DialogPreferences::GetFlagReturnToSelectAfterWirePlaced()
{
    return ui->ReturnToSelectAfterWire->isChecked();
}

bool DialogPreferences::GetFlagReturnToSelectAfterTextPlaced()
{
    return ui->ReturnToSelectAfterText->isChecked();
}

void DialogPreferences::on_buttonBox_accepted()
{
}

void DialogPreferences::on_buttonBox_rejected()
{
}

