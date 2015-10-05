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

#include "DialogAddEditProperty.h"
#include "ui_DialogAddEditProperty.h"

//////////////////////////////////////////////////////////////////////////////

DialogAddEditProperty::DialogAddEditProperty(ItemProperties* Properties, QString Name /*=""*/, QString Value /*=""*/, QString Desc /*=""*/, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DialogAddEditProperty)
{
    ui->setupUi(this);

    m_Properties = Properties;
    m_StartingProperyName = Name;

    ui->lineEditName->setText(Name);
    ui->lineEditValue->setText(Value);
    ui->lineEditDesc->setText(Desc);

    if (Name.isEmpty() == true) {
        m_EditMode = false;
        setWindowTitle("Add Property");
    } else {
        m_EditMode = true;
        setWindowTitle("Edit Property");
    }

    // Setup Unlock Button Controls
    m_Icon_Locked = QIcon(":/images/PropertyDlgLocked.png");
    m_Icon_Unlocked = QIcon(":/images/PropertyDlgUnlocked.png");
    ui->BtnUnlockName->setIcon(m_Icon_Locked);
    ui->BtnUnlockDesc->setIcon(m_Icon_Locked);
    ui->BtnUnlockName->setToolTip("Press To Toggle Lock on Property Name");
    ui->BtnUnlockDesc->setToolTip("Press To Toggle Lock on Property Description");

    // Make unlock buttons visible based upon the Edit Mode
    ui->BtnUnlockName->setVisible(m_EditMode);
    ui->BtnUnlockDesc->setVisible(m_EditMode);

    // Set flags for Name & Desc Edit locked based upon the Edit Mode
    m_NameFieldLocked = m_EditMode;
    m_DescFieldLocked = m_EditMode;
    ui->lineEditName->setEnabled(!m_NameFieldLocked);
    ui->lineEditDesc->setEnabled(!m_DescFieldLocked);
}

DialogAddEditProperty::~DialogAddEditProperty()
{
    delete ui;
}

void DialogAddEditProperty::on_buttonBox_accepted()
{
    QString NewName;
    QString Value;
    QString Desc;

    NewName = ui->lineEditName->text();
    Value = ui->lineEditValue->text();
    Desc =  ui->lineEditDesc->text();

    if (m_EditMode == false) {
        // Only add data if the Propery name is populated
        if (NewName.isEmpty() == false) {
            m_Properties->AddProperty(NewName, "", Desc);

            // Change the value, this will cause the project to be dirty
            m_Properties->SetPropertyValue(NewName, Value);
        }
    } else {
        // Dont change name to empty string
        if (NewName.isEmpty() == false) {
            m_Properties->ChangePropertyName(m_StartingProperyName, NewName);
        }
        m_Properties->SetPropertyDesc(m_StartingProperyName, Desc);
        m_Properties->SetPropertyValue(m_StartingProperyName, Value);
    }

}

void DialogAddEditProperty::on_BtnUnlockName_clicked()
{
    if (m_NameFieldLocked == true) {
        m_NameFieldLocked = false;
        ui->BtnUnlockName->setIcon(m_Icon_Unlocked);
    } else {
        m_NameFieldLocked = true;
        ui->BtnUnlockName->setIcon(m_Icon_Locked);
    }
    ui->lineEditName->setEnabled(!m_NameFieldLocked);
}

void DialogAddEditProperty::on_BtnUnlockDesc_clicked()
{
    if (m_DescFieldLocked == true) {
        m_DescFieldLocked = false;
        ui->BtnUnlockDesc->setIcon(m_Icon_Unlocked);
    } else {
        m_DescFieldLocked = true;
        ui->BtnUnlockDesc->setIcon(m_Icon_Locked);
    }
    ui->lineEditDesc->setEnabled(!m_DescFieldLocked);
}
