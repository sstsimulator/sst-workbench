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

#include "DialogManageModules.h"
#include "ui_DialogManageModules.h"

//////////////////////////////////////////////////////////////////////////////

DialogManageModules::DialogManageModules(GraphicItemComponent* SelectedComponent, SSTInfoData* InfoData, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DialogManageModules)
{
    // Init Variables
    m_SelectedComponent = NULL;
    m_SSTInfoData = NULL;

    // Build the UI
    ui->setupUi(this);

    // Set the title
    setWindowTitle(MANAGEMODULESDLG_DLGTITLE);

    // Save the selected component
    m_SelectedComponent = SelectedComponent;
    m_SSTInfoData = InfoData;

    // Setup the Tables
    SetupTables();
}

DialogManageModules::~DialogManageModules()
{
    delete ui;
}

void DialogManageModules::SetupTables()
{
    PopulateAvailableModulesTable();
    PopulateComponentModulesTable();
}

void DialogManageModules::PopulateComponentModulesTable()
{
    int     x;
    int     NumModulesInComponent;
    QString ModuleName;

    NumModulesInComponent = m_SelectedComponent->GetModulesList().size();

    for (x = 0; x < NumModulesInComponent; x++) {
        ModuleName = m_SelectedComponent->GetModulesList().at(x);
        AddModuleNameToComponentSide(ModuleName);
    }
}

void DialogManageModules::PopulateAvailableModulesTable()
{
    int                 x;
    int                 y;
    int                 NumElements;
    int                 NumModules;
    SSTInfoDataElement* Element;
    SSTInfoDataModule*  Module;
    QString             FullModuleName;
    QListWidgetItem*    NewItem;

    // Walk through all the elements and find all the modules.
    NumElements = m_SSTInfoData->GetNumOfElements();
    for (x = 0; x < NumElements; x++) {
        Element = m_SSTInfoData->GetElement(x);

        // Walk through all the Modules
        NumModules = Element->GetNumOfModules();
        for (y = 0; y < NumModules; y++) {
            Module = Element->GetModule(y);
            FullModuleName = Element->GetElementName() + "." + Module->GetModuleName();

            // Create a new list item & set its data ptr to the parameter list
            NewItem = new QListWidgetItem(FullModuleName);
            // Save a Pointer to the Module as part of this list item so we can get it later
            NewItem->setData(Qt::UserRole, VPtr<SSTInfoDataModule>::asQVariant(Module));
            // NOTE: To get the data back out from this list itme use:
            // SSTInfoDataModule* ptrModule = VPtr<SSTInfoDataModule>::asPtr(item->data(Qt::UserRole));

            // Now add this Module name to the list
            ui->listWidgetAvailableModules->addItem(NewItem);
        }
    }
}

void DialogManageModules::AddModuleNameToComponentSide(QString ModuleName)
{
    int                     x;
    QList<QListWidgetItem*> FoundAvailableListItems;

    // Find all the items that match this name from the Available Modules  List
    FoundAvailableListItems = ui->listWidgetAvailableModules->findItems(ModuleName, Qt::MatchFixedString|Qt::MatchCaseSensitive);

    // For all the items found, add them to the Component List, and HIDE them from the Available List
    for (x = 0; x < FoundAvailableListItems.size(); x++) {
        ui->listWidgetComponentModules->addItem(ModuleName);

        FoundAvailableListItems.at(x)->setHidden(true);
    }
}

void DialogManageModules::RemoveModuleNameFromComponentSide(QString ModuleName)
{
    int                     x;
    QList<QListWidgetItem*> FoundAvailableListItems;
    QList<QListWidgetItem*> FoundComponentListItems;

    // Find all the items that match this name from the Available Modules List and Components Modules List
    FoundAvailableListItems = ui->listWidgetAvailableModules->findItems(ModuleName, Qt::MatchFixedString|Qt::MatchCaseSensitive);
    FoundComponentListItems = ui->listWidgetComponentModules->findItems(ModuleName, Qt::MatchFixedString|Qt::MatchCaseSensitive);

    // For all the items found, remove them to the Component List, and UNHIDE them from the Available List
    for (x = 0; x < FoundComponentListItems.size(); x++) {
        ui->listWidgetComponentModules->takeItem(ui->listWidgetComponentModules->row(FoundComponentListItems.at(x)));
    }

    for (x = 0; x < FoundAvailableListItems.size(); x++) {
        FoundAvailableListItems.at(x)->setHidden(false);
    }
}

void DialogManageModules::SaveData()
{
    int                x;
    int                y;
    QStringList        ModuleNamesList;
    QListWidgetItem*   ComponentListItem;
    SSTInfoDataModule* ptrModule;
    SSTInfoDataParam*  ptrParam;

    // Build a string list of modules
    for (x = 0; x < ui->listWidgetComponentModules->count(); x++) {
        ComponentListItem = ui->listWidgetComponentModules->item(x);
        ModuleNamesList.append(ComponentListItem->text());
    }

    // Add the list of modules to the component
    m_SelectedComponent->SetModulesList(ModuleNamesList);

    // Now Add the properties of each module to the Component,
    // NOTE: Duplicate properties already in the component list will not be added
    //       by the properties object

    // Look at each module to see if it is hidden (yes means its in the Component Modules list)
    for (x = 0; x < ui->listWidgetAvailableModules->count(); x++) {
        ComponentListItem = ui->listWidgetAvailableModules->item(x);
        ptrModule = VPtr<SSTInfoDataModule>::asPtr(ComponentListItem->data(Qt::UserRole));

        if (ComponentListItem->isHidden() == true) {
            // For each param of the module, add it to the Component
            for (y = 0; y < ptrModule->GetNumOfParams(); y++) {
                ptrParam = ptrModule->GetParam(y);
                m_SelectedComponent->AddModuleParameter(ptrParam->GetParamName(), ptrParam->GetDefaultValue(), ptrParam->GetParamDesc());
            }
        }
    }

    // Update the Visual display of the component and its properties
    emit m_SelectedComponent->ItemComponentRefreshPropertiesWindow(m_SelectedComponent->GetItemProperties());
    m_SelectedComponent->UpdateVisualLayoutOfComponent();
}

void DialogManageModules::on_BtnMoveLeft_clicked()
{
    int                     x;
    QList<QListWidgetItem*> SelectedListItems;

    // Get all the selected items from the Available Modules List
    SelectedListItems = ui->listWidgetAvailableModules->selectedItems();

    // For all the items selected, add them to the Component List
    for (x = 0; x < SelectedListItems.size(); x++) {
        AddModuleNameToComponentSide(SelectedListItems.at(x)->text());
        // Turn off selection
        SelectedListItems.at(x)->setSelected(false);
    }
}

void DialogManageModules::on_BtnMoveRight_clicked()
{
    int                     x;
    QList<QListWidgetItem*> SelectedListItems;

    // Get all the selected items from the Component Modules List
    SelectedListItems = ui->listWidgetComponentModules->selectedItems();

    // For all the items selected, remove them from the Component List
    for (x = 0; x < SelectedListItems.size(); x++) {
        RemoveModuleNameFromComponentSide(SelectedListItems.at(x)->text());
        // Turn off selection
        SelectedListItems.at(x)->setSelected(false);
    }
}

void DialogManageModules::on_closeButton_clicked()
{
    // Save the Data entered
    SaveData();

    // Close the Modal Dialog
    done(0);
}

void DialogManageModules::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SaveData();

        // Close the Modal Dialog
        this->close();
    }
}

void DialogManageModules::on_listWidgetAvailableModules_itemDoubleClicked(QListWidgetItem *item)
{
    AddModuleNameToComponentSide(item->text());
}

void DialogManageModules::on_listWidgetComponentModules_itemDoubleClicked(QListWidgetItem *item)
{
    RemoveModuleNameFromComponentSide(item->text());
}
