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

#include "tabpageentry.h"

#define REQUIREDSTRINGFLAG "<Required>"

TabPageEntry::TabPageEntry(QString PageName, QWidget *parent) :
    QWidget(parent)
{
    // Create the Label for the Page Name
    m_pPageName = new QLabel(PageName, this);

    // Setup the layout for this tab page
    m_pVBoxLayout = new QVBoxLayout(this);
    m_pVBoxLayout->setSpacing(6);
    m_pVBoxLayout->setContentsMargins(11, 11, 11, 11);

    m_pGridLayout = new QGridLayout();
    m_pGridLayout->setSpacing(6);
    m_pGridLayout->setContentsMargins(11, 11, 11, 11);

    // Add the PageName and Grid Layout to the main Layout
    m_pVBoxLayout->addWidget(m_pPageName, Qt::AlignHCenter);
    m_pVBoxLayout->addLayout(m_pGridLayout);

    m_pHeaderData = NULL;
}

TabPageEntry::~TabPageEntry()
{
    if (NULL != m_pHeaderData) {
        delete m_pHeaderData;
    }
}

void TabPageEntry::setParentSize(QSize parentSize)
{
    // Resoze our page to the width of the parent, and leave our heigh alone
    resize(parentSize.width() - 11, size().height());
}

void TabPageEntry::populateHeaderInfo(HeaderData* pHeaderData)
{
    int             x;
    int             numEntries;
    int             startingOffset = 5;
    int             rowIndex = 0;
    QString         VariableText;
    QString         strLineNum;
    EntryData*      pEntryData;

    QCheckBox*      EntryNameCheckBox;
    QLabel*         EntryVarNameLabel;
    QLineEdit*      EntryVarDataEdit;
    QPushButton*    EntryVarDataButton;

    QString         EntryPersistValue;
    bool            EntryPersistEnabled;

    // Have this page Save a pointer to the headerdata
    m_pHeaderData = pHeaderData;

    numEntries = pHeaderData->getNumEntries();
    for (x = 0; x < numEntries; x++) {

        // Get the Entry Data
        pEntryData = pHeaderData->getEntry(x);

        // Create the Variable Text depending upon if it is optional or not
        VariableText = pEntryData->getConfigVariableName() + " =";

        // Get the Line Number of the Entry into a string
        strLineNum = "Configuration file Entry on Line number " + QString::number(pEntryData->getConfigEntryLineNumber());

        // Get the settings from the persistent data
        EntryPersistEnabled = m_PersistData.GetEntryState(m_pPageName->text(), pEntryData->getConfigEntryName());
        EntryPersistValue   = m_PersistData.GetEntryValue(m_pPageName->text(), pEntryData->getConfigEntryName());

        // We need to Set the persistant data value to the object
        pEntryData->setUserEntryVarData(EntryPersistValue);

        // Create Entry Checkbox Control.  This sets up the tooltip, statustip, property of the row index, and then any signals
        // necessay to process the control.  Then add it to the Entry Data structure so we can access it
        EntryNameCheckBox = new QCheckBox(pEntryData->getConfigEntryName(), this);
        EntryNameCheckBox->setToolTip(strLineNum);                        // Tooltip the line number found in the configuration file
        EntryNameCheckBox->setStatusTip(pEntryData->getConfigInfoText()); // Display the InfoText on the status bar when user mouses over control
        EntryNameCheckBox->setProperty("PageRow", x);                     // Save the row index of this control
        connect(EntryNameCheckBox, SIGNAL(stateChanged(int)), this, SLOT(on_checkbox_clicked(int))); // Setup the handler for when this control is clicked
        pEntryData->setControlEntryNameCheckBox(EntryNameCheckBox);

        // Now do the Variable controls if the Variable exists for this entry
        if (pEntryData->getConfigVariableExists()) {
            EntryVarNameLabel = new QLabel(VariableText, this);
            EntryVarNameLabel->setStatusTip(pEntryData->getConfigInfoText()); // Display the InfoText on the status bar when user mouses over control

            // Create Entry LineEdit Control.  This sets up the statustip, property of the row index, and then any signals
            // necessay to process the control.  Then add it to the Entry Data structure so we can access it
            EntryVarDataEdit = new QLineEdit("", this);
            EntryVarDataEdit->setStatusTip(pEntryData->getConfigInfoText()); // Display the InfoText on the status bar when user mouses over control
            EntryVarDataEdit->setEnabled(false);
            EntryVarDataEdit->setProperty("PageRow", x);                     // Save the row index of this control
            connect(EntryVarDataEdit, SIGNAL(textChanged(const QString&)), this, SLOT(on_text_changed(const QString&))); // Setup the handler for when this control is clicked
            pEntryData->setControlEntryVarDataEdit(EntryVarDataEdit);

            // Check to see if the VariableText is one of the Legal Text's to enable the Button
            if (m_PersistData.GetEntryVarsThatEnableDirOptionList().contains(VariableText)) {
                // Create Entry Pushbutton Control.  This sets up the statustip, property of the row index, and then any signals
                // necessay to process the control.  Then add it to the Entry Data structure so we can access it
                EntryVarDataButton = new QPushButton(tr("..."), this);
                EntryVarDataButton->setStatusTip(pEntryData->getConfigInfoText()); // Display the InfoText on the status bar when user mouses over control
                EntryVarDataButton->setEnabled(false);
                EntryVarDataButton->setProperty("PageRow", x);                     // Save the row index of this control
                connect(EntryVarDataButton, SIGNAL(clicked(bool)), this, SLOT(on_button_clicked(bool))); // Setup the handler for when this control is clicked
                pEntryData->setDirButtonEnabled(true);
                pEntryData->setControlEntryVarDataButton(EntryVarDataButton);
            }
        }

        // Set some values for the Grid Layout
        m_pGridLayout->setRowMinimumHeight(x+startingOffset, 21);
        rowIndex = 0;

        // Add the controls to the Grid Layout
        m_pGridLayout->addWidget(EntryNameCheckBox, x+startingOffset, rowIndex++);
        m_pGridLayout->setColumnMinimumWidth(rowIndex++, 20);
        if (true == EntryPersistEnabled) {
            EntryNameCheckBox->setCheckState(Qt::Checked);
        }

        if (pEntryData->getConfigVariableExists()) {
            m_pGridLayout->addWidget(EntryVarNameLabel, x+startingOffset, rowIndex++, Qt::AlignRight);
            m_pGridLayout->addWidget(EntryVarDataEdit, x+startingOffset, rowIndex++);
            if (true == pEntryData->getDirButtonEnabled()) {
                m_pGridLayout->addWidget(EntryVarDataButton, x+startingOffset, rowIndex++);
            }
            // Set the data from the persist Value
            EntryVarDataEdit->setText(EntryPersistValue);
        }
    }
}

void TabPageEntry::on_checkbox_clicked(int state)
{
    int             PageRow;
    EntryData*      pEntryData;
    QLineEdit*      EntryVarDataEdit;
    QPushButton*    EntryVarDataButton;

    // Get the ptr to the calling checkbox, then get its row index where it was
    // instantiated on the page, then lastly get the pointer to its assocated
    // EntryData
    PageRow = sender()->property("PageRow").toInt();
    pEntryData = m_pHeaderData->getEntry(PageRow);

    // Now see if we need to Enable/Disable Variable Controls
    if (pEntryData->getConfigVariableExists()) {
        // Get the pointers to the controls
        EntryVarDataEdit = pEntryData->getControlEntryVarDataEdit();
        EntryVarDataButton = pEntryData->getControlEntryVarDataButton();

        // Now set them Enabled/Disabled depending upon the checkbox
        EntryVarDataEdit->setEnabled(state == Qt::Checked);
        if (true == pEntryData->getDirButtonEnabled()) {
            EntryVarDataButton->setEnabled(state == Qt::Checked);
        }

        // Populate the Edit field as appropriate depending upon if the field is enabled or disabled
        // Test if the edit field is Enabled
        if (true == EntryVarDataEdit->isEnabled()) {
            // The Edit field is enabled, does the user have any data saved?
            if ("" == pEntryData->getUserEntryVarData()) {
                // No User Var Data exists, but is the data required or not?
                if (true == pEntryData->getConfigVariableRequired()) {
                    // Data is required, tell the user
                    EntryVarDataEdit->setText(REQUIREDSTRINGFLAG);
                } else {
                    // Data is optional, fill it with an empty string
                    EntryVarDataEdit->setText("");
                }
            } else {
                // There is User Var Data, so copy that into the edit field
                EntryVarDataEdit->setText(pEntryData->getUserEntryVarData());
            }
        } else {
            // The edit field is Disabled, does the user have any data saved?
            if ("" == pEntryData->getUserEntryVarData()) {
                // No data is saved, so just clear the entry field
                EntryVarDataEdit->setText("");
            }
        }
    }

    // Set the Entry to either Selected or not depending upon the Checkbox
    pEntryData->setUserEntrySelected(state == Qt::Checked);

    // CHECKBOX
    // Store the data in persistant storage
    m_PersistData.SetEntryState(m_pPageName->text(), pEntryData->getConfigEntryName(), state == Qt::Checked);
    m_PersistData.SetEntryValue(m_pPageName->text(), pEntryData->getConfigEntryName(), pEntryData->getUserEntryVarData());
}

void TabPageEntry::on_text_changed(const QString& text)
{
    int             PageRow;
    EntryData*      pEntryData;
    QLineEdit*      EntryVarDataEdit;

    // Get the ptr to the calling LineEdit, then get its row index where it was
    // instantiated on the page, then lastly get the pointer to its assocated
    // EntryData
    PageRow = sender()->property("PageRow").toInt();
    pEntryData = m_pHeaderData->getEntry(PageRow);

    // Get the ptr to the LineEdit Control
    EntryVarDataEdit = pEntryData->getControlEntryVarDataEdit();

    QString myText = text;

    // Handle the change in the the Edit field as appropriate depending upon if the field is enabled or disabled
    // Note: Change may be due to enabling the field, it will be enabled/disabled before the text has changed.
    // Test if the edit field is Enabled
    if (true == EntryVarDataEdit->isEnabled()) {
        // The Edit field is enabled, does the field contain the REQUIREDSTRINGFLAG?
        if (REQUIREDSTRINGFLAG == text) {
            // Yes, the field equals REQUIREDSTRINGFLAG, so clear the user stored var data
            pEntryData->setUserEntryVarData("");
        } else {
            // No, this is updated user data store the updated text
            pEntryData->setUserEntryVarData(text);
        }
    }

    // TEXT CHANGED
    // Store the data in persistant storage
    m_PersistData.SetEntryState(m_pPageName->text(), pEntryData->getConfigEntryName(), pEntryData->getUserEntrySelected());
    m_PersistData.SetEntryValue(m_pPageName->text(), pEntryData->getConfigEntryName(), pEntryData->getUserEntryVarData());
}

void TabPageEntry::on_button_clicked(bool checked /*= false*/)
{
    Q_UNUSED(checked)

    int          PageRow;
    EntryData*   pEntryData;
    QLineEdit*   EntryVarDataEdit;
    QString      currDirName;
    QString      dirName;

    // Get the ptr to the calling PushButton, then get its row index where it was
    // instantiated on the page, then lastly get the pointer to its assocated
    // EntryData
    PageRow = sender()->property("PageRow").toInt();
    pEntryData = m_pHeaderData->getEntry(PageRow);

    // Get the ptr to the LineEdit Control
    EntryVarDataEdit = pEntryData->getControlEntryVarDataEdit();

    // Display to the user a select directory dialog box

    // Get the path typed in the edit box (if it exists)
    currDirName = "/.";

    // Get the path to the SST Directory
    dirName = QFileDialog::getExistingDirectory(this, tr("Select A Directory"), currDirName,
              QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks/* | QFileDialog::DontUseNativeDialog*/);

    // Make sure the user picked something before setting the text
    if(!dirName.isEmpty()) {
        // Set the path in the edit control (NOTE; This will force a call to on_sstRootPathEdit_textChanged() )
       EntryVarDataEdit->setText(dirName);
    }
}

