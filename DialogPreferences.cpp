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

void DialogPreferences::SetFlagAutomaticallyDeleteShortWires(bool flag)
{
    ui->AutoDeleteShortWires->setChecked(flag);
}

void DialogPreferences::SetFlagDisplayGridEnabled(bool flag)
{
    ui->DisplayGrid->setChecked(flag);
}

void DialogPreferences::SetFlagSnapToGridEnabled(bool flag)
{
    ui->SnapToGrid->setChecked(flag);
}

void DialogPreferences::SetGridSize(int GridSize)
{
    QString str;
    str = QString("%1").arg(GridSize);

    ui->GridSizeComboBox->setCurrentText(str);
}

///////////////////////////////

bool DialogPreferences::GetFlagReturnToSelectAfterWirePlaced()
{
    return ui->ReturnToSelectAfterWire->isChecked();
}

bool DialogPreferences::GetFlagReturnToSelectAfterTextPlaced()
{
    return ui->ReturnToSelectAfterText->isChecked();
}

bool DialogPreferences::GetFlagAutomaticallyDeleteShortWires()
{
    return ui->AutoDeleteShortWires->isChecked();
}

bool DialogPreferences::GetFlagDisplayGridEnabled()
{
    return ui->DisplayGrid->isChecked();
}

bool DialogPreferences::GetFlagSnapToGridEnabled()
{
    return ui->SnapToGrid->isChecked();
}

int DialogPreferences::GetGridSize()
{
    QString StrSize = ui->GridSizeComboBox->currentText();
    return StrSize.toInt();
}

///////////////////////////////

void DialogPreferences::on_buttonBox_accepted()
{
}

void DialogPreferences::on_buttonBox_rejected()
{
}

void DialogPreferences::on_BackgroundImageFileSelect_clicked()
{
}
