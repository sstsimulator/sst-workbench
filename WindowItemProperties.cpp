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

#include "WindowItemProperties.h"

////////////////////////////////////////////////////////////

WindowItemProperties::WindowItemProperties(QWidget* parent /*=0*/)
    : QFrame(parent)
{
    m_CurrentProperties = NULL;

    // Create the table widget
    m_PropertiesTable = new QTableWidget();

    // Create a Group Box
    m_PropertiesGroupBox = new QGroupBox(this);
    m_PropertiesGroupBox->setTitle("Properties:");

    // Build a Vertical layout for the PropertiesGroupBox and
    // put the TableWidget inside of it
    QVBoxLayout* pPropertiesGroupBoxLayout = new QVBoxLayout();
    pPropertiesGroupBoxLayout->addWidget(m_PropertiesTable);
    m_PropertiesGroupBox->setLayout(pPropertiesGroupBoxLayout);

    // Create a layout with the sub windows inside
    QVBoxLayout* pLayout = new QVBoxLayout();
    pLayout->addWidget(m_PropertiesGroupBox);

    // Set the layout to this window
    setLayout(pLayout);

    // Set the Row and Column sizes
    m_PropertiesTable->setColumnCount(2);
    m_PropertiesTable->setRowCount(0);

    // Set the Headers
    QStringList HeaderList;
    HeaderList.append("Property");
    HeaderList.append("Value");
    m_PropertiesTable->setHorizontalHeaderLabels(HeaderList);
    m_PropertiesTable->verticalHeader()->setVisible(false);
    m_PropertiesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_PropertiesTable->horizontalHeader()->setStretchLastSection(true);
    m_PropertiesTable->setMouseTracking(true);  // Enable StatusTips to work

    // Handle when an Item changes
    connect(m_PropertiesTable, SIGNAL(cellChanged(int, int)), this, SLOT(HandleCellChanged(int, int)));
    connect(m_PropertiesTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(HandleItemDoubleClicked(QTableWidgetItem*)));

    //setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    setMinimumWidth(m_PropertiesTable->sizeHint().width());

    // Flag that indicates that we are populating the window
    m_PopulatingWindow = false;
}

WindowItemProperties::~WindowItemProperties()
{
}

void WindowItemProperties::AddPropertyData(QString Property, QString Value, QString Desc, bool ReadOnly /* =false*/)
{
    int NumRows;

    // Increment the number of rows
    NumRows = m_PropertiesTable->rowCount();
    NumRows++;
    m_PropertiesTable->setRowCount(NumRows);

    // Create new TableWidgetItem for the Property and turn off Editing for that cell
    QTableWidgetItem* PropertyItem = new QTableWidgetItem(Property);
    PropertyItem->setFlags(PropertyItem->flags() ^ Qt::ItemIsEditable);
    PropertyItem->setStatusTip(Desc);

    // Create new TableWidgetItem for the Value Control cell editing based upon ReadOnlyFlag
    QTableWidgetItem* ValueItem = new QTableWidgetItem(Value);
    if (ReadOnly == true) {
        ValueItem->setFlags(ValueItem->flags() ^ Qt::ItemIsEditable);
        ValueItem->setFlags(ValueItem->flags() ^ Qt::ItemIsEnabled);
    }
    ValueItem->setStatusTip(Desc);

    // Add the data to the Table
    m_PropertiesTable->setItem(NumRows - 1, 0, PropertyItem);
    m_PropertiesTable->setItem(NumRows - 1, 1, ValueItem);

    // Set the Headers to show the Columns
    m_PropertiesTable->resizeColumnsToContents();
    m_PropertiesTable->horizontalHeader()->setStretchLastSection(true);
}

void WindowItemProperties::SetGraphicItemProperties(ItemProperties* Properties)
{
    int           x;
    ItemProperty* Prop;

    // Clear the properties window
    ClearProperiesWindow();

    // Save the ptr to the current set of properties
    m_CurrentProperties = Properties;

    m_PopulatingWindow = true;

    // Add each property to the graphic display
    if (m_CurrentProperties != NULL) {
        for (x = 0; x < Properties->GetNumProperties(); x++) {
            Prop = Properties->GetProperty(x);
            AddPropertyData(Prop->GetName(), Prop->GetValue(), Prop->GetDesc(), Prop->GetReadOnly());
        }
    }

    m_PopulatingWindow = false;
}

void WindowItemProperties::ClearProperiesWindow()
{
    // Set the row count to 0, and remove the graphic display of properties
    m_PropertiesTable->clearContents();
    m_PropertiesTable->setRowCount(0);
    m_CurrentProperties = NULL;

    // Set the Headers to show the Columns
    m_PropertiesTable->resizeColumnsToContents();
    m_PropertiesTable->horizontalHeader()->setStretchLastSection(true);
}

void WindowItemProperties::RefreshProperiesWindowProperty(QString PropertyName, QString NewPropertyValue)
{
    int               RowCount;
    int               row;
    QString           PropName;
    QTableWidgetItem* ItemProp;
    QTableWidgetItem* ItemValue;

    if (PropertyName.isEmpty() == false) {
        RowCount = m_PropertiesTable->rowCount();
        // Walk all the rows and find the Property Name that matches
        for (row = 0; row < RowCount; row++) {
            ItemProp = m_PropertiesTable->item(row, 0);
            ItemValue = m_PropertiesTable->item(row, 1);
            PropName = ItemProp->text();

            if (PropName == PropertyName) {
                ItemValue->setText(NewPropertyValue);
            }
        }
    }
}

void WindowItemProperties::HandleCellChanged(int row, int col)
{
    QTableWidgetItem* ItemProp;
    QTableWidgetItem* ItemValue;
    QString           Prop;
    QString           NewValue;

    // Only process changes to Values
    if (col == 1) {
        ItemProp = m_PropertiesTable->item(row, 0);
        ItemValue = m_PropertiesTable->item(row, 1);
        if ((ItemProp != NULL) && (ItemValue != NULL)) {
            Prop = ItemProp->text();
            NewValue = ItemValue->text();

            // Now Figure out which Property it is
            ItemProperty* ptrPropertyData = m_CurrentProperties->GetProperty(Prop);
            if (ptrPropertyData != NULL) {
                if (m_PopulatingWindow == false) {
                    //qDebug() << "WindowItemProperties::HandleCellChanged() --- UPDATING VALUE: Prop = " << Prop << " New Value = " << NewValue;
                    ptrPropertyData->SetValue(NewValue, !m_PopulatingWindow);
                }
            }
        }
    }
}

void WindowItemProperties::HandleItemDoubleClicked(QTableWidgetItem* Item)
{
    QString           PropName;
    ItemProperty*     Property;
    QString           ControllingParam;
    QString           OrigPropertyName;

    // Make sure this is a Dynamic Property & has no controlling parameter
    PropName = Item->text();
    Property = m_CurrentProperties->GetProperty(PropName);
    if ((Item->column() == 0)  && (Property != NULL)) {
        if ((Property->GetDynamicFlag() == true) && (Property->GetControllingProperty().isEmpty())) {
            ControllingParam = Property->GetControllingProperty();
            OrigPropertyName = Property->GetOriginalPropertyName();

            m_ConfigureDynamicParameter = new DialogParametersConfig(Property);

            // Run the dialog box (Modal)
            m_ConfigureDynamicParameter->exec();

            // Delete the Dialog
            delete m_ConfigureDynamicParameter;
        }
    }
}
