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

#include "setupgeneraldialog.h"
#include "ui_setupgeneraldialog.h"

// Column Names
#define COL_ENVVAR_NAME        0
#define COL_ENVVAR_VALUE       1
#define COL_ENVVAR_NUMCOLUMNS  2

#define COL_GROUP_NAME         0
#define COL_GROUP_ENABLED      1
#define COL_GROUP_NUMENTRIES   2
#define COL_GROUP_DETAILS      3
#define COL_GROUP_NUMCOLUMNS   4

SetupGeneralDialog::SetupGeneralDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupGeneralDialog)
{
    m_PopulatingEnvGroupData = false;

    // Setup the UI
    ui->setupUi(this);
    setWindowTitle("General Setup");

    // Tell the tab what layout to use
    ui->TabEnv->setLayout(ui->verticalLayout_2);

    // Disable the Remove and Copy buttons by default
    ui->RemoveBtn->setDisabled(true);
    ui->CopyBtn->setDisabled(true);

    // Setup the Env Tables
    setupTables();
}

SetupGeneralDialog::~SetupGeneralDialog()
{
    delete ui;
}

void SetupGeneralDialog::setupTables()
{
    QStringList         HeaderListEnv;
    QStringList         HeaderListGroup;

    // Setup the Header of the tables
    HeaderListEnv << "Env. Variable" << "Value";
    ui->tableWidget_StdEnv->setColumnCount(COL_ENVVAR_NUMCOLUMNS);                       // Env Variable Columns
    ui->tableWidget_StdEnv->setHorizontalHeaderLabels(HeaderListEnv);                    // Header Names
    ui->tableWidget_StdEnv->horizontalHeader()->resizeSection(COL_ENVVAR_NAME, 200);     // Header index 0 width
    ui->tableWidget_StdEnv->horizontalHeader()->setStretchLastSection(true);             // Header Index 1 consumes remaining space
    ui->tableWidget_StdEnv->setSortingEnabled(true);                                     // Enable Sorting

    HeaderListGroup << "Group Name" << "Enabled" << "Num. Entries" << "Details";
    ui->tableWidget_EnvGroups->setColumnCount(COL_GROUP_NUMCOLUMNS);                      // Group Name Columns
    ui->tableWidget_EnvGroups->setHorizontalHeaderLabels(HeaderListGroup);                // Header Names
    ui->tableWidget_EnvGroups->horizontalHeader()->resizeSection(COL_GROUP_NAME, 200);    // Header index 0 width
    ui->tableWidget_EnvGroups->horizontalHeader()->resizeSection(COL_GROUP_ENABLED, 50);  // Header index 0 width
    ui->tableWidget_EnvGroups->horizontalHeader()->resizeSection(COL_GROUP_DETAILS, 100); // Header index 0 width
    ui->tableWidget_EnvGroups->horizontalHeader()->setStretchLastSection(true);           // Header Index 1 consumes remaining space
    ui->tableWidget_EnvGroups->setSortingEnabled(false);                                  // Disable Sorting

    // Fill the entries from the System Environment and what we read from the INI file
    PopulateEnvVars();
    PopulateEnvGroupData();

    // Finally Sort the Environment keys
    ui->tableWidget_StdEnv->sortItems(0);
}

void SetupGeneralDialog::PopulateEnvVars()
{
    QProcessEnvironment Env;
    QStringList         EnvKeyList;
    QString             EnvKeyValue;
    int                 x;

    // Get the Current System Environment
    Env = QProcessEnvironment::systemEnvironment();
    EnvKeyList = Env.keys();

    // Now add all the System Environment variables
    for (x = 0; x < EnvKeyList.count(); x++) {
        EnvKeyValue = Env.value(EnvKeyList[x]);
        AddEnvEntry(EnvKeyList[x], EnvKeyValue);
    }
}

void SetupGeneralDialog::AddEnvEntry(QString EnvName, QString EnvValue)
{
    QTableWidgetItem* pItem;
    int               rowIndex;

    // Add a new Row
    ui->tableWidget_StdEnv->insertRow(ui->tableWidget_StdEnv->rowCount());
    rowIndex = ui->tableWidget_StdEnv->rowCount() - 1;

    // Add the Env Name to the table
    pItem = new QTableWidgetItem(EnvName);
    pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tableWidget_StdEnv->setItem(rowIndex, COL_ENVVAR_NAME, pItem);

    // Add the Env Value to the table, also decide set it it is editable
    pItem = new QTableWidgetItem(EnvValue);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tableWidget_StdEnv->setItem(rowIndex, COL_ENVVAR_VALUE, pItem);
}

void SetupGeneralDialog::PopulateEnvGroupData()
{
    int x;
    QString GroupName;
    QString Enabled;

    // Indicate that we are populating the data
    m_PopulatingEnvGroupData = true;

    for (x = 0; x < m_PersistantData.GetEnvGroupNamesList().count(); x++) {
        GroupName = m_PersistantData.GetEnvGroupNamesList().at(x);
        Enabled  = m_PersistantData.GetEnvGroupEnablesList().at(x);

        AddEnvGroupEntry(GroupName, Enabled == "1");
    }

    // Indicate that we are done populating the data
    m_PopulatingEnvGroupData = false;
}

void SetupGeneralDialog::AddEnvGroupEntry(QString GroupName, bool Enabled)
{
    int               rowIndex;
    QTableWidgetItem* pItem;
    QPushButton*      pNewButton;
    QStringList       EnvVarNamesList;

    // Add a new Row
    ui->tableWidget_EnvGroups->insertRow(ui->tableWidget_EnvGroups->rowCount());
    rowIndex = ui->tableWidget_EnvGroups->rowCount() - 1;

    // Add the Env Name to the table
    pItem = new QTableWidgetItem(GroupName);
    pItem->setTextAlignment(Qt::AlignVCenter);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->tableWidget_EnvGroups->setItem(rowIndex, COL_GROUP_NAME, pItem);

    // Add the Enabled Checkbox to the Env variable
    pItem = new QTableWidgetItem("");
    pItem->data(Qt::CheckStateRole);
    pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    pItem->setCheckState(Enabled ? Qt::Checked : Qt::Unchecked);
    ui->tableWidget_EnvGroups->setItem(rowIndex, COL_GROUP_ENABLED, pItem);

    // Get the list of Env Names for this group
    EnvVarNamesList = m_PersistantData.GetEnvVarNamesList(GroupName);
    // Add the number of Env Variables located in this group
    pItem = new QTableWidgetItem(QString::number(EnvVarNamesList.count()));
    pItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    pItem->setFlags(Qt::ItemIsEnabled);
    ui->tableWidget_EnvGroups->setItem(rowIndex, COL_GROUP_NUMENTRIES, pItem);

    // Add the Details Button to the Env variable
    pNewButton = new QPushButton();
    pNewButton->setText("Setup");
    pNewButton->setProperty("Row", rowIndex);
    connect(pNewButton, SIGNAL(clicked(bool)), this, SLOT(handle_setup_button_clicked(bool))); // Setup the handler for when this control is clicked
    ui->tableWidget_EnvGroups->setCellWidget(rowIndex, COL_GROUP_DETAILS, pNewButton);
}

void SetupGeneralDialog::CopyEnvGroupEntry(QString ExistingGroupName, QString NewGroupName, bool Enabled)
{
    m_PersistantData.CopyEnvGroup(ExistingGroupName, NewGroupName, Enabled ? "1":"0");

    // Delete all the rows
    ui->tableWidget_EnvGroups->setRowCount(0);

    // Repopulate the Env Data TableWidget
    PopulateEnvGroupData();
}

void SetupGeneralDialog::RemoveEnvGroupEntry(QString GroupName)
{
    m_PersistantData.RemoveEnvGroup(GroupName);

    // Delete all the rows
    ui->tableWidget_EnvGroups->setRowCount(0);

    // Repopulate the Env Data TableWidget
    PopulateEnvGroupData();
}

void SetupGeneralDialog::SaveEnvGroupData()
{
    QTableWidgetItem* pItem;
    int               x;
    QString           GroupName;
    QString           GroupEnable;

    // Disable the table to lock down any cells that are being edited
    ui->tableWidget_EnvGroups->setDisabled(true);
    ui->tableWidget_EnvGroups->setDisabled(false);

    // Get the data for each row
    for (x = 0; x < ui->tableWidget_EnvGroups->rowCount(); x++) {
        pItem = ui->tableWidget_EnvGroups->item(x, COL_GROUP_NAME);  // Group Name
        GroupName = pItem->text();
        // Make sure entry is not empty
        if ("" != GroupName) {
            pItem = ui->tableWidget_EnvGroups->item(x, COL_GROUP_ENABLED);  // Group Append Checkbox
            if (Qt::Checked == pItem->checkState()) {
                GroupEnable = "1";
            } else {
                GroupEnable = "0";
            }
        }
        // Add / Set the persistant data
        m_PersistantData.SetEnvGroupNameAndEnable(GroupName, GroupEnable);
    }
}

void SetupGeneralDialog::on_CloseBtn_clicked()
{
    // Save the Data entered
    SaveEnvGroupData();

    // Close the Modal Dialog
    done(0);
}

void SetupGeneralDialog::keyPressEvent(QKeyEvent* evt)
{
    // Did the user click Escape?
    if (Qt::Key_Escape == evt->key()) {
        // Save the Data entered
        SaveEnvGroupData();

        // Close the Modal Dialog
        this->close();
    }
}

void SetupGeneralDialog::on_AddBtn_clicked()
{
    // Save the currently set data (Just in case the user added an entry)
    SaveEnvGroupData();

    // Add a Group
    AddEnvGroupEntry("", false);
}

void SetupGeneralDialog::on_CopyBtn_clicked()
{
    int               selectedRow;
    QString           SelectedGroupName;
    QString           NewGroupName;
    QTableWidgetItem* pItem;

    // Save the currently set data (Just in case the user added an entry)
    SaveEnvGroupData();

    // Get the a Group entry index
    selectedRow = ui->tableWidget_EnvGroups->currentRow();

    if (selectedRow >= 0) {
        // Get the Group Name
        SelectedGroupName = m_PersistantData.GetEnvGroupNamesList().at(selectedRow);

        // Get the TableItem at the current row
        pItem = ui->tableWidget_EnvGroups->item(selectedRow, COL_GROUP_ENABLED);

        // Build the new group name
        NewGroupName = SelectedGroupName + "_COPY";

        CopyEnvGroupEntry(SelectedGroupName, NewGroupName, pItem->checkState() == Qt::Checked);

        // Reselect the selected row
        if (selectedRow >= ui->tableWidget_EnvGroups->rowCount()) {
            selectedRow = ui->tableWidget_EnvGroups->rowCount() - 1;
        }
        // Disable the Remove Button and then Set the new selected cell (may re-enable the remove button)
        ui->RemoveBtn->setDisabled(true);
        ui->CopyBtn->setDisabled(true);
        ui->tableWidget_EnvGroups->setCurrentCell(selectedRow, COL_GROUP_NAME);
    }
    // Restore the lost focus
    ui->tableWidget_EnvGroups->setFocus();
}

void SetupGeneralDialog::on_RemoveBtn_clicked()
{
    int     selectedRow;
    QString GroupName;
    QString Question;

    // Save the currently set data (Just in case the user added an entry)
    // before deleting another one
    SaveEnvGroupData();

    // Get the Group entry index
    selectedRow = ui->tableWidget_EnvGroups->currentRow();

    if (selectedRow >= 0) {
        // Get the Group Name
        GroupName = m_PersistantData.GetEnvGroupNamesList().at(selectedRow);

        Question = "Are you sure you want to delete group " + GroupName;
        if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", Question)) {

            // Remove Name from Presistant storage
            RemoveEnvGroupEntry(GroupName);

            // Reselect the selected row
            if (selectedRow >= ui->tableWidget_EnvGroups->rowCount()) {
                selectedRow = ui->tableWidget_EnvGroups->rowCount() - 1;
            }
            // Disable the Remove Button and then Set the new selected cell (may re-enable the remove button)
            ui->RemoveBtn->setDisabled(true);
            ui->CopyBtn->setDisabled(true);
            ui->tableWidget_EnvGroups->setCurrentCell(selectedRow, COL_GROUP_NAME);
        }
        // Restore the lost focus
        ui->tableWidget_EnvGroups->setFocus();
    }
}

void SetupGeneralDialog::handle_setup_button_clicked(bool checked /*= false*/)
{
    Q_UNUSED(checked)

    QString           Name;
    int               Row;
    QTableWidgetItem* pItem;
    QStringList       EnvVarNamesList;

    // Save the Data entered
    SaveEnvGroupData();

    // Get the ptr to the calling PushButton, then get its row index where it was
    // instantiated on the page, then lastly get the pointer to its assocated
    // EntryData
    Row = sender()->property("Row").toInt();

    // Check to make sure we have a legal row
    if (Row >= ui->tableWidget_EnvGroups->rowCount()) {
        return;
    }

    // Get the Item and its Group Name
    pItem = ui->tableWidget_EnvGroups->item(Row, COL_GROUP_NAME);
    Name = pItem->text();
    // Make sure entry is not empty
    if (Name != "") {
        // Call the Modal Dialog for the Users Environment variables
        m_pSetupEnvDialog = new SetupEnvDialog(Name, this);
        m_pSetupEnvDialog->exec();
        delete m_pSetupEnvDialog;
        m_pSetupEnvDialog = NULL;
    }

    // Refresh the Number of Env Variables counts for each group
    for (Row = 0; Row < ui->tableWidget_EnvGroups->rowCount(); Row++) {
        // Get the Item and its Group Name
        pItem = ui->tableWidget_EnvGroups->item(Row, COL_GROUP_NAME);
        Name = pItem->text();
        // Get the list of Env Names for this group
        EnvVarNamesList = m_PersistantData.GetEnvVarNamesList(Name);
        pItem = ui->tableWidget_EnvGroups->item(Row, COL_GROUP_NUMENTRIES);
        // Update the text for this row and column
        pItem->setText(QString::number(EnvVarNamesList.count()));
    }
}

void SetupGeneralDialog::on_tableWidget_EnvGroups_itemSelectionChanged()
{
    // Check if we need to enable the Remove and Copy Buttons
    if (ui->tableWidget_EnvGroups->selectedItems().count() >= 0) {
        ui->RemoveBtn->setDisabled(false);
        ui->CopyBtn->setDisabled(false);
    } else {
        ui->RemoveBtn->setDisabled(true);
        ui->CopyBtn->setDisabled(true);
    }
}

void SetupGeneralDialog::on_tableWidget_EnvGroups_cellChanged(int row, int column)
{
    QTableWidgetItem* pItem;
    QString           NewGroupName;

    if (false == m_PopulatingEnvGroupData) {
        if (COL_GROUP_NAME == column) {
            // Get the Item and the new text
            pItem = ui->tableWidget_EnvGroups->item(row, column);
            NewGroupName = pItem->text();

            m_PersistantData.RenameEnvGroup(m_EnvGroupNameBeingEdited, NewGroupName);
        }
    }
}

void SetupGeneralDialog::on_tableWidget_EnvGroups_itemDoubleClicked(QTableWidgetItem *item)
{
    QString GroupName;

    if (false == m_PopulatingEnvGroupData) {
        GroupName = item->text();

        m_EnvGroupNameBeingEdited = GroupName;
    }
}
