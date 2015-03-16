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

#include "DialogParametersConfig.h"
#include "ui_DialogParametersConfig.h"

//////////////////////////////////////////////////////////////////////////////

DialogParametersConfig::DialogParametersConfig(ItemProperty* SelectedProperty, QWidget* parent /*-0*/) :
    QDialog(parent),
    ui(new Ui::DialogParametersConfig)
{
    QString LabelText;

    // Init Variables
    m_SelectedProperty = NULL;
    m_SpinBoxEditor = NULL;

    // Build the UI
    ui->setupUi(this);

    // Set the title
    setWindowTitle(CONFIGPARAMDLG_DLGTITLE);

    // Save the selected property
    m_SelectedProperty = SelectedProperty;

    // Set the label text for this component
    LabelText = CONFIGPARAMDLG_COMPONENTTITLE;
    ui->SetDynamicParametersTitle->setText(LabelText);

    // Create The SpinBox Delegate (Special Editor & Keypress handler for the 2nd column)
    m_SpinBoxEditor = new SpinBoxEditor(0, CONFIGPARAMDLG_MAX_NUM_DYNAMIC_PARAMS, this);

    // Setup the Table for Dynamic Parameters
    SetupTable();
}

DialogParametersConfig::~DialogParametersConfig()
{
    delete ui;
}

void DialogParametersConfig::SetupTable()
{
    QStringList         HeaderList;

    // Setup the headers for the table
    HeaderList << CONFIGPARAMDLG_HEADER_DYN_PARAM_NAME << CONFIGPARAMDLG_HEADER_SET_NUM_PARAMS;
    ui->TableWidget_DynParameters->setColumnCount(2);                                // Two Columns
    ui->TableWidget_DynParameters->setHorizontalHeaderLabels(HeaderList);            // Header Names
    ui->TableWidget_DynParameters->horizontalHeader()->resizeSection(0, CONFIGPARAMDLG_HEADER_0_WIDTH);  // Header index 0 width
    ui->TableWidget_DynParameters->horizontalHeader()->setStretchLastSection(true);  // Header Index 1 consumes remaining space
    ui->TableWidget_DynParameters->setSortingEnabled(false);                         // Disable Sorting

    // Set table headers to resize per their contents
    ui->TableWidget_DynParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Set the Third column to use the SpinBoxEditor
    ui->TableWidget_DynParameters->setItemDelegateForColumn(1, m_SpinBoxEditor);

    // Fill the entries of the port data
    PopulateParameterData();
}

void DialogParametersConfig::PopulateParameterData()
{
    AddParameterDataEntry(m_SelectedProperty->GetOriginalPropertyName(), QString("%1").arg(m_SelectedProperty->GetNumInstances()));
}

void DialogParametersConfig::AddParameterDataEntry(QString Name, QString Value)
{
    QTableWidgetItem* pItem;
    int               rowIndex;

    // Add a new Row to the table
    ui->TableWidget_DynParameters->insertRow(ui->TableWidget_DynParameters->rowCount());
    rowIndex = ui->TableWidget_DynParameters->rowCount() - 1;

    // Add the Parameter Name to the table
    pItem = new QTableWidgetItem(Name);
    pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItem->setFlags(Qt::NoItemFlags);
    ui->TableWidget_DynParameters->setItem(rowIndex, 0, pItem);

    // Add the Parameter Value to the table, also set it as editable
    pItem = new QTableWidgetItem(Value);
    pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->TableWidget_DynParameters->setItem(rowIndex, 1, pItem);
}

void DialogParametersConfig::SaveParameterData()
{
    ItemProperties*   ParentPropertiesList;
    int               CurrentNumInstances;
    int               NewNumInstances;
    QTableWidgetItem* pItem;
    QString           Value;

    // Disable the table to lock down any cells that are being edited
    ui->TableWidget_DynParameters->setDisabled(true);
    ui->TableWidget_DynParameters->setDisabled(false);

    pItem = ui->TableWidget_DynParameters->item(0, 1);  // Parameter Value
    Value = pItem->text();
    NewNumInstances = Value.toInt();

    ParentPropertiesList = m_SelectedProperty->GetParentProperties();
    CurrentNumInstances = m_SelectedProperty->GetNumInstances();

    if (NewNumInstances != CurrentNumInstances) {
        ParentPropertiesList->AdjustDynamicPropertyInList(m_SelectedProperty->GetName(), NewNumInstances, true);
    }
}

void DialogParametersConfig::on_CloseBtn_clicked()
{
    // Save the Data entered
    SaveParameterData();

    // Close the Modal Dialog
    done(0);
}

void DialogParametersConfig::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SaveParameterData();

        // Close the Modal Dialog
        this->close();
    }
}

