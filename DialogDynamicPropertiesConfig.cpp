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

#include "DialogDynamicPropertiesConfig.h"
#include "ui_DialogDynamicPropertiesConfig.h"

//////////////////////////////////////////////////////////////////////////////

DialogDynamicPropertiesConfig::DialogDynamicPropertiesConfig(ItemProperty* SelectedProperty, QWidget* parent /*-0*/) :
    QDialog(parent),
    ui(new Ui::DialogDynamicPropertiesConfig)
{
    QString LabelText;

    // Init Variables
    m_SelectedProperty = NULL;
    m_SpinBoxEditor = NULL;

    // Build the UI
    ui->setupUi(this);

    // Set the title
    setWindowTitle(CONFIGPROPDLG_DLGTITLE);

    // Save the selected property
    m_SelectedProperty = SelectedProperty;

    // Set the label text for this component
    LabelText = CONFIGPROPDLG_COMPONENTTITLE;
    ui->SetDynamicPropertiesTitle->setText(LabelText);

    // Create The SpinBox Delegate (Special Editor & Keypress handler for the 2nd column)
    m_SpinBoxEditor = new SpinBoxEditor(0, CONFIGPROPDLG_MAX_NUM_DYNAMIC_PROPS, this);

    // Setup the Table for Dynamic Propertys
    SetupTable();
}

DialogDynamicPropertiesConfig::~DialogDynamicPropertiesConfig()
{
    delete ui;
}

void DialogDynamicPropertiesConfig::SetupTable()
{
    QStringList         HeaderList;

    // Setup the headers for the table
    HeaderList << CONFIGPROPDLG_HEADER_DYN_PROP_NAME << CONFIGPROPDLG_HEADER_SET_NUM_PROPS;
    ui->TableWidget_DynProperties->setColumnCount(2);                                // Two Columns
    ui->TableWidget_DynProperties->setHorizontalHeaderLabels(HeaderList);            // Header Names
    ui->TableWidget_DynProperties->horizontalHeader()->resizeSection(0, CONFIGPROPDLG_HEADER_0_WIDTH);  // Header index 0 width
    ui->TableWidget_DynProperties->horizontalHeader()->setStretchLastSection(true);  // Header Index 1 consumes remaining space
    ui->TableWidget_DynProperties->setSortingEnabled(false);                         // Disable Sorting

    // Set table headers to resize per their contents
    ui->TableWidget_DynProperties->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Set the Third column to use the SpinBoxEditor
    ui->TableWidget_DynProperties->setItemDelegateForColumn(1, m_SpinBoxEditor);

    // Fill the entries of the port data
    PopulatePropertyData();
}

void DialogDynamicPropertiesConfig::PopulatePropertyData()
{
    AddPropertyDataEntry(m_SelectedProperty->GetOriginalPropertyName(), QString("%1").arg(m_SelectedProperty->GetNumInstances()));
}

void DialogDynamicPropertiesConfig::AddPropertyDataEntry(QString Name, QString Value)
{
    QTableWidgetItem* pItem;
    int               rowIndex;

    // Add a new Row to the table
    ui->TableWidget_DynProperties->insertRow(ui->TableWidget_DynProperties->rowCount());
    rowIndex = ui->TableWidget_DynProperties->rowCount() - 1;

    // Add the Property Name to the table
    pItem = new QTableWidgetItem(Name);
    pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItem->setFlags(Qt::NoItemFlags);
    ui->TableWidget_DynProperties->setItem(rowIndex, 0, pItem);

    // Add the Property Value to the table, also set it as editable
    pItem = new QTableWidgetItem(Value);
    pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->TableWidget_DynProperties->setItem(rowIndex, 1, pItem);
}

void DialogDynamicPropertiesConfig::SavePropertyData()
{
    ItemProperties*   ParentPropertiesList;
    int               CurrentNumInstances;
    int               NewNumInstances;
    QTableWidgetItem* pItem;
    QString           Value;

    // Disable the table to lock down any cells that are being edited
    ui->TableWidget_DynProperties->setDisabled(true);
    ui->TableWidget_DynProperties->setDisabled(false);

    pItem = ui->TableWidget_DynProperties->item(0, 1);  // Property Value
    Value = pItem->text();
    NewNumInstances = Value.toInt();

    ParentPropertiesList = m_SelectedProperty->GetParentProperties();
    CurrentNumInstances = m_SelectedProperty->GetNumInstances();

    if (NewNumInstances != CurrentNumInstances) {
        ParentPropertiesList->AdjustDynamicPropertyInList(m_SelectedProperty->GetName(), NewNumInstances, true);
    }
}

void DialogDynamicPropertiesConfig::on_CloseBtn_clicked()
{
    // Save the Data entered
    SavePropertyData();

    // Close the Modal Dialog
    done(0);
}

void DialogDynamicPropertiesConfig::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SavePropertyData();

        // Close the Modal Dialog
        this->close();
    }
}

