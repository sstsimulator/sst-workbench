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

#include "DialogPortsConfig.h"
#include "ui_DialogPortsConfig.h"

//////////////////////////////////////////////////////////////////////////////

DialogPortsConfig::DialogPortsConfig(GraphicItemComponent* SelectedComponent, QWidget* parent /*=0*/) :
    QDialog(parent),
    ui(new Ui::DialogPortsConfig)
{
    QString LabelText;

    // Init Variables
    m_SelectedComponent = NULL;
    m_SpinBoxEditor = NULL;

    // Build the UI
    ui->setupUi(this);

    // Set the title
    setWindowTitle(CONFIGPORTDLG_DLGTITLE);

    // Save the selected component
    m_SelectedComponent = SelectedComponent;

    // Set the label text for this component
    LabelText = CONFIGPORTDLG_COMPONENTTITLE + m_SelectedComponent->GetComponentDisplayName() ;
    ui->SetDynamicPortsTitle->setText(LabelText);

    // Create The SpinBox Delegate (Special Editor & Keypress handler for the 2nd column)
    m_SpinBoxEditor = new SpinBoxEditor(0, CONFIGPORTDLG_MAX_NUM_DYNAMIC_PORTS, this);

    // Setup the Table for Dynamic Ports
    SetupTable();
}

DialogPortsConfig::~DialogPortsConfig()
{
    delete ui;
}

void DialogPortsConfig::SetupTable()
{
    QStringList         HeaderList;

    // Setup the headers for the table
    HeaderList << CONFIGPORTDLG_HEADER_INDEX << CONFIGPORTDLG_HEADER_DYN_PORT_NAME << CONFIGPORTDLG_HEADER_SET_NUM_PORTS;
    ui->TableWidget_DynPorts->setColumnCount(3);                                // Two Columns
    ui->TableWidget_DynPorts->setHorizontalHeaderLabels(HeaderList);            // Header Names
    ui->TableWidget_DynPorts->horizontalHeader()->resizeSection(0, CONFIGPORTDLG_HEADER_0_WIDTH); // Header index 0 width
    ui->TableWidget_DynPorts->horizontalHeader()->resizeSection(1, CONFIGPORTDLG_HEADER_1_WIDTH);  // Header index 1 width
    ui->TableWidget_DynPorts->horizontalHeader()->setStretchLastSection(true);  // Header Index 2 consumes remaining space
    ui->TableWidget_DynPorts->setSortingEnabled(false);                         // Disable Sorting

    // Set table headers to resize per their contents
    ui->TableWidget_DynPorts->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Set the Third column to use the SpinBoxEditor
    ui->TableWidget_DynPorts->setItemDelegateForColumn(2, m_SpinBoxEditor);

    // Fill the entries of the port data
    PopulatePortData();
}

void DialogPortsConfig::PopulatePortData()
{
    int           x;
    QString       Name;
    QString       Value;
    PortInfoData* PortInfo;

    // Add info for all dynamic variables
    for (x = 0; x < m_SelectedComponent->GetPortInfoDataArray().count(); x++)
    {
        // Get the port, and check if it is dynamic
        PortInfo = m_SelectedComponent->GetPortInfoDataArray().at(x);
        if (PortInfo->IsPortDynamic() == true) {
            // This port is dynamic, get its data
            Name = PortInfo->GetSSTInfoPortName();
            Value = QString("%1").arg(PortInfo->GetNumTotalInstances());

            // Add Port Data
            AddPortDataEntry(x, Name, Value);
        }
    }
}

void DialogPortsConfig::AddPortDataEntry(int Index, QString& EnvName, QString& EnvValue)
{
    QTableWidgetItem* pItem;
    int               rowIndex;

    // Add a new Row to the table
    ui->TableWidget_DynPorts->insertRow(ui->TableWidget_DynPorts->rowCount());
    rowIndex = ui->TableWidget_DynPorts->rowCount() - 1;

    // Add the Index to the table
    pItem = new QTableWidgetItem(QString("%1").arg(Index));
    pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pItem->setFlags(Qt::NoItemFlags);
    ui->TableWidget_DynPorts->setItem(rowIndex, 0, pItem);

    // Add the Port Name to the table
    pItem = new QTableWidgetItem(EnvName);
    pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItem->setFlags(Qt::NoItemFlags);
    ui->TableWidget_DynPorts->setItem(rowIndex, 1, pItem);

    // Add the Port Value to the table, also set it as editable
    pItem = new QTableWidgetItem(EnvValue);
    pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->TableWidget_DynPorts->setItem(rowIndex, 2, pItem);
}

void DialogPortsConfig::SavePortData()
{
    QTableWidgetItem* pItem;
    int               x;
    int               Index;
    QString           PortName;
    QString           PortValue;
    PortInfoData*     PortInfo;
    QString           ControllingParam;
    QString           NumInstancesStr;
    ItemProperty*     Property;

    // Disable the table to lock down any cells that are being edited
    ui->TableWidget_DynPorts->setDisabled(true);
    ui->TableWidget_DynPorts->setDisabled(false);

    // Get the data for each row
    for (x = 0; x < ui->TableWidget_DynPorts->rowCount(); x++) {
        pItem = ui->TableWidget_DynPorts->item(x, 0);  // Index
        Index = pItem->text().toInt();

        pItem = ui->TableWidget_DynPorts->item(x, 1);  // Port Name
        PortName = pItem->text();

        pItem = ui->TableWidget_DynPorts->item(x, 2);  // Port Value
        PortValue = pItem->text();  // Ports will range from 0 to PORT_MAX_NUM_DYNAMIC_PORTS

        // Get the Port Info
        PortInfo = m_SelectedComponent->GetPortInfoDataArray().at(Index);
        PortInfo->SetNumTotalInstances(PortValue.toInt());

    }

    // Tell the component to redraw itself
    m_SelectedComponent->UpdateVisualLayoutOfComponent();

    // Now for all Ports Sync any changes to the port number to the its Component Property (if one exists)
    for (x = 0; x < m_SelectedComponent->GetPortInfoDataArray().count(); x++) {
        PortInfo = m_SelectedComponent->GetPortInfoDataArray().at(x);
        NumInstancesStr = QString("%1").arg(PortInfo->GetNumCreatedInstances());
        ControllingParam = PortInfo->GetDynamicPortContollingParameterName();
        if (ControllingParam.isEmpty() == false) {
            // Get the property set by the controlling property
            Property = m_SelectedComponent->GetItemProperties()->GetProperty(ControllingParam);
            if (Property != NULL) {
                Property->SetValue(NumInstancesStr, false);
                emit m_SelectedComponent->ItemComponentRefreshPropertiesWindowProperty(ControllingParam, NumInstancesStr);
            }
        }
    }
}

void DialogPortsConfig::on_CloseBtn_clicked()
{
    // Save the Data entered
    SavePortData();

    // Close the Modal Dialog
    done(0);
}

void DialogPortsConfig::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SavePortData();

        // Close the Modal Dialog
        this->close();
    }
}

