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

#include "setupenvdialog.h"
#include "ui_setupenvdialog.h"

SetupEnvDialog::SetupEnvDialog(QString GroupName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupEnvDialog)
{
    QString LabelText;

    ui->setupUi(this);
    setWindowTitle("Setup Environment Variables");

    // Build the text to put on the main label
    m_GroupName = GroupName;
    LabelText = "Environment Variables For Group (" + GroupName + ")";

    ui->EnvVarName->setText(LabelText);

    // Setup the Env Tables
    setupEnvTable();
}

SetupEnvDialog::~SetupEnvDialog()
{
    delete ui;
}

void SetupEnvDialog::setupEnvTable()
{
    QStringList         HeaderList;

    HeaderList << "Env. Variable" << "Value";
    ui->tableWidget_Env->setColumnCount(2);                                // Two Colums
    ui->tableWidget_Env->setHorizontalHeaderLabels(HeaderList);            // Header Names
    ui->tableWidget_Env->horizontalHeader()->resizeSection(0, 200);        // Header index 0 width
    ui->tableWidget_Env->horizontalHeader()->setStretchLastSection(true);  // Header Index 1 consumes remaining space
    ui->tableWidget_Env->setSortingEnabled(true);                          // Enable Sorting

    // Fill the entries from the system environment and what we read from the INI file
    PopulateEnvVars();
}

void SetupEnvDialog::PopulateEnvVars()
{
    int x;
    QString Name;
    QString Value;

    for (x = 0; x < m_PersistantData.GetEnvVarNamesList(m_GroupName).count(); x++) {
        Name    = m_PersistantData.GetEnvVarNamesList(m_GroupName).at(x);
        Value  = m_PersistantData.GetEnvVarValuesList(m_GroupName).at(x);

        AddEnvEntry(Name, Value);
    }
}

void SetupEnvDialog::AddEnvEntry(QString EnvName, QString EnvValue)
{
    QTableWidgetItem* pItem;
    int               rowIndex;

    // Add a new Row
    ui->tableWidget_Env->insertRow(ui->tableWidget_Env->rowCount());
    rowIndex = ui->tableWidget_Env->rowCount() - 1;

    // Add the Env Name to the table
    pItem = new QTableWidgetItem(EnvName);
    pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->tableWidget_Env->setItem(rowIndex, 0, pItem);

    // Add the Env Value to the table, also decide set it it is editable
    pItem = new QTableWidgetItem(EnvValue);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->tableWidget_Env->setItem(rowIndex, 1, pItem);
}

void SetupEnvDialog::SaveEnvData()
{
    QTableWidgetItem* pItem;
    int               x;
    QStringList       EnvNames;
    QStringList       EnvValues;
    QStringList       EnvGroups;

    // Disable the table to lock down any cells that are being edited
    ui->tableWidget_Env->setDisabled(true);
    ui->tableWidget_Env->setDisabled(false);

    // Get the data for each row
    for (x = 0; x < ui->tableWidget_Env->rowCount(); x++) {
        pItem = ui->tableWidget_Env->item(x, 0);  // Env Name
        EnvNames += pItem->text();

        pItem = ui->tableWidget_Env->item(x, 1);  // Env Value
        EnvValues += pItem->text();

        EnvGroups += m_GroupName;
    }

    // Now send the data to the persistant storage
    m_PersistantData.SetEnvVarNamesList(m_GroupName, EnvNames);
    m_PersistantData.SetEnvVarValuesList(m_GroupName, EnvValues);
}

void SetupEnvDialog::on_CloseBtn_clicked()
{
    // Save the Data entered
    SaveEnvData();

    // Close the Modal Dialog
    done(0);
}

void SetupEnvDialog::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SaveEnvData();

        // Close the Modal Dialog
        this->close();
    }
}

void SetupEnvDialog::on_AddBtn_clicked()
{
    // Add a  env var entry
    AddEnvEntry("", "");
}

void SetupEnvDialog::on_RemoveBtn_clicked()
{
    int selectedRow;

    // Delete a  env var entry
    selectedRow = ui->tableWidget_Env->currentRow();

    if (selectedRow >= 0) {
        ui->tableWidget_Env->removeRow(selectedRow);
    }
}

