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

#include "WindowItemProperties.h"

////////////////////////////////////////////////////////////

WindowItemProperties::WindowItemProperties(QWidget* parent /*=0*/)
    : QFrame(parent)
{
    m_CurrentProperties = NULL;

    // Create the table widget
    m_PropertiesTable = new QTableWidget();

    // Create the Buttons to manage Properties
    QIcon Icon_Add(":/images/PropertyAdd.png");
    QIcon Icon_Remove(":/images/PropertyDelete.png");
    QIcon Icon_Edit(":/images/PropertyEdit.png");

    m_BtnAddProperty = new QToolButton();
    m_BtnAddProperty->setIcon(Icon_Add);
    m_BtnAddProperty->setIconSize(QSize(10, 10));
    m_BtnAddProperty->setEnabled(false);
    m_BtnAddProperty->setToolTip("Add a Property");

    m_BtnRemoveProperty = new QToolButton();
    m_BtnRemoveProperty->setIcon(Icon_Remove);
    m_BtnRemoveProperty->setIconSize(QSize(10, 10));
    m_BtnRemoveProperty->setEnabled(false);
    m_BtnRemoveProperty->setToolTip("Remove Properties");

    m_BtnEditProperty = new QToolButton();
    m_BtnEditProperty->setIcon(Icon_Edit);
    m_BtnEditProperty->setIconSize(QSize(10, 10));
    m_BtnEditProperty->setEnabled(false);
    m_BtnEditProperty->setToolTip("Edit this Property");

    // Create a Group Box
    m_PropertiesGroupBox = new QGroupBox(this);
    m_PropertiesGroupBox->setTitle("Properties:");

    // Build a Horizontal layout for the Tool Buttons
    QHBoxLayout* pPropertieButtonsLayout = new QHBoxLayout();
    pPropertieButtonsLayout->addWidget(m_BtnAddProperty);
    pPropertieButtonsLayout->addWidget(m_BtnEditProperty);
    pPropertieButtonsLayout->addWidget(m_BtnRemoveProperty);
    pPropertieButtonsLayout->addStretch();

    // Build a Vertical layout for the PropertiesGroupBox and
    // put the TableWidget inside of it
    QVBoxLayout* pPropertiesGroupBoxLayout = new QVBoxLayout();
    pPropertiesGroupBoxLayout->addWidget(m_PropertiesTable);
    pPropertiesGroupBoxLayout->addLayout(pPropertieButtonsLayout);

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
    connect(m_PropertiesTable, SIGNAL(itemSelectionChanged()), this, SLOT(HandleSelectionChanged()));
    connect(m_PropertiesTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(HandleItemDoubleClicked(QTableWidgetItem*)));
    connect(m_BtnAddProperty, SIGNAL(clicked()), this, SLOT(HandleBtnAddPropertyClicked()));
    connect(m_BtnRemoveProperty, SIGNAL(clicked()), this, SLOT(HandleBtnRemovePropertyClicked()));
    connect(m_BtnEditProperty, SIGNAL(clicked()), this, SLOT(HandleBtnEditPropertyClicked()));

    //setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    setMinimumWidth(m_PropertiesTable->sizeHint().width());

    // Flag that indicates that we are populating the window
    m_PopulatingWindow = false;
}

WindowItemProperties::~WindowItemProperties()
{
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
            AddPropertyData(Prop);
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

    // Disable the Add Property Button
    m_BtnAddProperty->setEnabled(false);
    m_BtnRemoveProperty->setEnabled(false);
    m_BtnEditProperty->setEnabled(false);
}

void WindowItemProperties::RefreshAllProperties()
{
    // If Properties are set, reset them (this will clear the window first)
    if (m_CurrentProperties != NULL) {
        SetGraphicItemProperties(m_CurrentProperties);
    }
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

void WindowItemProperties::AddPropertyData(ItemProperty* Property)
{
    int NumRows;

    QIcon Icon_Protected(":/images/PropertyDlgLocked.png");

    // Increment the number of rows
    NumRows = m_PropertiesTable->rowCount();
    NumRows++;
    m_PropertiesTable->setRowCount(NumRows);

    // Create new TableWidgetItem for the Property and turn off Editing for that cell
    QTableWidgetItem* PropertyItem = new QTableWidgetItem(Property->GetName());
    PropertyItem->setFlags(PropertyItem->flags() ^ Qt::ItemIsEditable);
    PropertyItem->setStatusTip(Property->GetDesc());
    PropertyItem->setToolTip(Property->GetDesc());
    if (Property->GetProtected() == true) {
        PropertyItem->setIcon(Icon_Protected);
    }

    // Save a Pointer to the ItemProperty as part of this object so we can get it later
    PropertyItem->setData(Qt::UserRole, VPtr<ItemProperty>::asQVariant(Property));
    // NOTE: To get the data back out from this Object use:
    // ItemProperty* ptrProperty = VPtr<ItemProperty>::asPtr(PropertyItem->data(Qt::UserRole));

    // Create new TableWidgetItem for the Value Control cell editing based upon ReadOnlyFlag
    QTableWidgetItem* ValueItem = new QTableWidgetItem(Property->GetValue());
    if (Property->GetReadOnly() == true) {
        ValueItem->setFlags(ValueItem->flags() ^ Qt::ItemIsEditable);
        ValueItem->setFlags(ValueItem->flags() ^ Qt::ItemIsEnabled);
    }
    ValueItem->setStatusTip(Property->GetDesc());
    ValueItem->setToolTip(Property->GetDesc());

    // Save a Pointer to the ItemProperty as part of this object so we can get it later
    ValueItem->setData(Qt::UserRole, VPtr<ItemProperty>::asQVariant(Property));
    // NOTE: To get the data back out from this Object use:
    // ItemProperty* ptrProperty = VPtr<ItemProperty>::asPtr(ValueItem->data(Qt::UserRole));

    // Add the data to the Table
    m_PropertiesTable->setItem(NumRows - 1, 0, PropertyItem);
    m_PropertiesTable->setItem(NumRows - 1, 1, ValueItem);

    // Set the Headers to show the Columns
    m_PropertiesTable->resizeColumnsToContents();
    m_PropertiesTable->horizontalHeader()->setStretchLastSection(true);

    // Enable the Add Property Button
    m_BtnAddProperty->setEnabled(true);
}

QString WindowItemProperties::GetPropertyName(int row)
{
    QTableWidgetItem* PropertyItem;
    QString           Name;

    PropertyItem = m_PropertiesTable->item(row, 0);

    Name = PropertyItem->text();

    return Name;
}

QString WindowItemProperties::GetPropertyValue(int row)
{
    QTableWidgetItem* ValueItem;
    QString           Value;

    ValueItem = m_PropertiesTable->item(row, 1);

    Value = ValueItem->text();

    return Value;
}

QString WindowItemProperties::GetPropertyDesc(int row)
{
    QTableWidgetItem* ValueItem;
    QString           Desc;

    ValueItem = m_PropertiesTable->item(row, 1);

    Desc = ValueItem->toolTip();

    return Desc;
}

bool WindowItemProperties::IsPropertyReadOnly(int row)
{
    ItemProperty*     ptrProperty;
    QTableWidgetItem* ValueItem;
    bool              ReadOnly;

    ValueItem = m_PropertiesTable->item(row, 1);
    ptrProperty = VPtr<ItemProperty>::asPtr(ValueItem->data(Qt::UserRole));

    ReadOnly = ptrProperty->GetReadOnly();

    return ReadOnly;
}

bool WindowItemProperties::IsPropertyProtected(int row)
{
    ItemProperty*     ptrProperty;
    QTableWidgetItem* ValueItem;
    bool              Protected;

    ValueItem = m_PropertiesTable->item(row, 1);
    ptrProperty = VPtr<ItemProperty>::asPtr(ValueItem->data(Qt::UserRole));

    Protected = ptrProperty->GetProtected();

    return Protected;
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

void WindowItemProperties::HandleSelectionChanged()
{
    QList<QTableWidgetItem*> SelectedItems;
    int                      SelectedRow;
    int                      x;

    SelectedItems = m_PropertiesTable->selectedItems();

    if (SelectedItems.count() == 1) {
        // One Item Selected, Turn On Edit and Remove Buttons

        // If the field is readonly or protected do not allow edit
        SelectedRow = m_PropertiesTable->row(SelectedItems.at(0));

        if ((IsPropertyReadOnly(SelectedRow) == true) || IsPropertyProtected(SelectedRow) == true) {
            m_BtnEditProperty->setEnabled(false);
        } else {
            m_BtnEditProperty->setEnabled(true);
        }

        // Enable the Remove Button if the property is NOT protected
        if (IsPropertyProtected(SelectedRow) == false) {
            m_BtnRemoveProperty->setEnabled(true);
        } else {
            m_BtnRemoveProperty->setEnabled(false);
        }
        return;
    }

    if (SelectedItems.count() == 0) {
        // Nothing Selected, Turn off Edit and Remove Buttons
        m_BtnRemoveProperty->setEnabled(false);
        m_BtnEditProperty->setEnabled(false);
        return;
    }

    if (SelectedItems.count() > 1) {
        // Multiple Items Selected, Turn off Edit Button
        m_BtnEditProperty->setEnabled(false);

        // By default turn on the Remove Btn
        m_BtnRemoveProperty->setEnabled(true);

        // Now see if any of the Items are protected, if yes turn off Remove Btn
        for (x = 0; x < SelectedItems.count(); x++) {
            SelectedRow = m_PropertiesTable->row(SelectedItems.at(x));
            if (IsPropertyProtected(SelectedRow) == true) {
                m_BtnRemoveProperty->setEnabled(false);
            }
        }

        return;
    }
}

void WindowItemProperties::HandleItemDoubleClicked(QTableWidgetItem* Item)
{
    QString           PropName;
    ItemProperty*     Property;
    QString           ControllingProperty;
    QString           OrigPropertyName;

    // Double clicking on Column 1 allows direct editing of the field
    // Double Clicking on Column 0 will bring up one of 2 dialogs depending upon
    // if the Propery is a Dynamic Property or a static Property.

    PropName = Item->text();
    Property = m_CurrentProperties->GetProperty(PropName);
    // Make sure the item is a property and is in column 0
    if ((Item->column() == 0)  && (Property != NULL)) {
        // See if this is a Dynamic Property & has no controlling property
        if ((Property->GetDynamicFlag() == true) && (Property->GetControllingProperty().isEmpty())) {
            ControllingProperty = Property->GetControllingProperty();
            OrigPropertyName    = Property->GetOriginalPropertyName();

            m_ConfigureDynamicProperties = new DialogDynamicPropertiesConfig(Property);

            // Run the dialog box (Modal)
            m_ConfigureDynamicProperties->exec();

            // Delete the Dialog
            delete m_ConfigureDynamicProperties;
        } else {
            // Make sure the property is not protected and then show up the Property Edit dialog
            if (Property->GetProtected() == false) {
                HandleBtnEditPropertyClicked();
            }
        }
    }
}

void WindowItemProperties::HandleBtnAddPropertyClicked()
{
    int nDlgRtn;

    m_AddEditProperty = new DialogAddEditProperty(m_CurrentProperties);

    // Run the dialog box (Modal)
    nDlgRtn = m_AddEditProperty->exec();

    if (nDlgRtn == QDialog::Accepted) {
        RefreshAllProperties();
    }

    // Delete the Dialog
    delete m_AddEditProperty;
}


void WindowItemProperties::HandleBtnRemovePropertyClicked()
{
    QString                           Question;
    QList<QTableWidgetItem*>          SelectedItems;
    QList<int>                        UniqueRows;
    int                               SelectedRow;
    QString                           Name;
    bool                              Protected;
    int                               NumItems;
    int                               NumRows = 0;
    int                               x;

    // Get the List of Selected Items
    SelectedItems = m_PropertiesTable->selectedItems();
    NumItems = SelectedItems.count();

    // Figure out how many unique rows were selected
    for (x = 0; x < NumItems; x++) {
        // There will only only be a single selected item
        SelectedRow = m_PropertiesTable->row(SelectedItems.at(x));
        // Is the selected row already in the UniqueRows List?
        if (UniqueRows.contains(SelectedRow) == false) {
            // Not in the list, so add it
            UniqueRows.append(SelectedRow);
        }
    }
    NumRows = UniqueRows.count();

    Question = QString("Are you sure you want to delete %1 Properties?").arg(NumRows);

    if (QMessageBox::question(NULL, "Delete These Properties?", Question) == QMessageBox::No) {
        return;
    }

    // Delete each item (except for Protected versions)
    for (x = 0; x < NumRows; x++) {
        // There will only only be a single selected item
        SelectedRow = UniqueRows.at(x);
        Name  = GetPropertyName(SelectedRow);
        Protected = IsPropertyProtected(SelectedRow);

        if (Protected == false) {
            m_CurrentProperties->RemoveProperty(Name);
        }
    }
    RefreshAllProperties();
}

void WindowItemProperties::HandleBtnEditPropertyClicked()
{
    int                      nDlgRtn;
    int                      SelectedRow;
    QList<QTableWidgetItem*> SelectedItems;
    QString                  Name;
    QString                  Value;
    QString                  Desc;

    SelectedItems = m_PropertiesTable->selectedItems();

    // There will only only be a single selected item
    SelectedRow = m_PropertiesTable->row(SelectedItems.at(0));
    Name  = GetPropertyName(SelectedRow);
    Value = GetPropertyValue(SelectedRow);
    Desc  = GetPropertyDesc(SelectedRow);

    m_AddEditProperty = new DialogAddEditProperty(m_CurrentProperties, Name, Value, Desc);

    // Run the dialog box (Modal)
    nDlgRtn = m_AddEditProperty->exec();

    if (nDlgRtn == QDialog::Accepted) {
        RefreshAllProperties();
    }

    // Delete the Dialog
    delete m_AddEditProperty;
}
