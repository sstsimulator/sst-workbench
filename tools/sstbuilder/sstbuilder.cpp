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

#include "sstbuilder.h"
#include "ui_sstbuilder.h"

SSTBuilder::SSTBuilder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSTBuilder)
{
    // Setup the User Interface Window
    ui->setupUi(this);

    // Init member varibles
    m_sstRootFileDir = "";
    m_sstAutogenFilePath = "";
    m_sstConfigFilePath = "";
    m_sstConfigFileExists = true;
    m_sstConfigTabsLoaded = false;

    // Init sub-widgets here
    ui->sstRootPathEdit->setText(m_PersistData.GetSSTRootPath());

    // Setup the Rt Click operation for the runOutputEdit control
    ui->runOutputEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->runOutputEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowRunOutputEditContextMenu(const QPoint&)));

    // General Initialization (force a repopulate)
    testForConfigureFileExists(true);

    setWindowTitle("SSTBuilder - Default Profile");
}

SSTBuilder::~SSTBuilder()
{
    // Destroy the User Interface
    delete ui;
}

void SSTBuilder::ShowRunOutputEditContextMenu(const QPoint& pos)
{
    QMenu*   pMenu;
    QAction* pSelectedItem;
    QAction* pMenuAction;
    QPoint   GlobalPos;

    // Get the Position of the mouse
    GlobalPos = ui->runOutputEdit->mapToGlobal(pos);

    // Build the Popup Menu for the runOutputEdit control
    pMenu = new QMenu;

    pMenuAction = new QAction("Select All", NULL);
    pMenuAction->setShortcut(QKeySequence::SelectAll);
    pMenu->addAction(pMenuAction);
    pMenu->addSeparator();
    pMenuAction = new QAction("Copy", NULL);
    pMenuAction->setShortcut(QKeySequence::Copy);
    pMenu->addAction(pMenuAction);
    pMenu->addSeparator();
    pMenu->addAction("Clear");
    pMenu->addSeparator();
    pMenu->addAction("Print");

    // Now Run the Popup Menu
    pSelectedItem = pMenu->exec(GlobalPos);
    if (pSelectedItem) {  // Was something selected
        if ("Clear" == pSelectedItem->text()) {
            // Clear the output display
            ui->runOutputEdit->setText("");
        } else if ("Print" == pSelectedItem->text()) {
            // Setup and select the printer
            QPrinter printer;
            QTextDocument *document = ui->runOutputEdit->document();
            QPrintDialog *dlg = new QPrintDialog(&printer, this);
            if (QDialog::Accepted == dlg->exec()) {
                printer.setResolution(12);
                // Perform the actual printing
                document->print(&printer);
            }
        } else if ("Copy" == pSelectedItem->text()) {
            ui->runOutputEdit->copy();
        } else if ("Select All" == pSelectedItem->text()) {
            // Select all text
            ui->runOutputEdit->selectAll();
        }
    }

    delete pMenu;
}

void SSTBuilder::on_exitButton_clicked()
{
    // Exit the Application
    qApp->quit();
}

void SSTBuilder::on_sstRootPathEdit_textChanged(const QString &arg1)
{
    QString         dirName;
    QString         autogenpath;
    QString         errorMsg;
    QFileInfo       fileInfo;

    // The Text changed in the path to the SST root Edit Field.
    // We need to get the text, and check it to see if its valid.
    // If valid allow the user to process the autogen.sh

    // Get the path typed in the edit box & Build the path to autogen.sh
    dirName = arg1;
    autogenpath = dirName + "/autogen.sh";
    ui->sstAutoGenPathEdit->setText(autogenpath);  // Set the other box to show the user what we are looking for

    if (true == testForAutoGenFileExists()) {
        // Save off the directory names
        m_sstRootFileDir = dirName;
        m_sstAutogenFilePath = autogenpath;

        // Save the Root path to the persistant data
        m_PersistData.SetSSTRootPath(dirName);
    }

    // Set the new proposed path to the configure file and see if it exists
    m_sstConfigFilePath = dirName + "/configure";
    testForConfigureFileExists();
}

void SSTBuilder::on_selectSSTRootPathButton_clicked()
{
    QString currDirName;
    QString dirName;

    // Display to the user a select directory dialog box

    // Get the path typed in the edit box (if it exists)
    currDirName = ui->sstRootPathEdit->text();
    if (currDirName.isEmpty()) {
        currDirName = "/.";
    }

    // Get the path to the SST Directory
    dirName = QFileDialog::getExistingDirectory(this, tr("Select SST Root Directory"), currDirName,
              QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks/* | QFileDialog::DontUseNativeDialog*/);

    // Set the path in the edit control (NOTE; This will force a call to on_sstRootPathEdit_textChanged() )
    if ("" != dirName) {
        ui->sstRootPathEdit->setText(dirName);
    }
}

void SSTBuilder::on_setupButton_clicked()
{
    m_pSetupGeneralDialog = new SetupGeneralDialog(this);
    m_pSetupGeneralDialog->exec();
    delete m_pSetupGeneralDialog;
    m_pSetupGeneralDialog = NULL;
}

void SSTBuilder::on_SaveProfile_clicked()
{
    QString currDirName;
    QString FilePath;

    // Display to the user a select directory dialog box

    // Get the path typed in the edit box (if it exists)
    currDirName = QDir::homePath();

    // Get the path to the SST Directory
    FilePath = QFileDialog::getSaveFileName(this, tr("Save SSTBuilder Profile"), currDirName,
              tr("SSTBuilder Profiles (*.sbdr);;All Files (*.*)"));

    // Set the path in the edit control (NOTE; This will force a call to on_sstRootPathEdit_textChanged() )
    if ("" != FilePath) {
        // Save data to the file
        m_PersistData.SavePersistantDataToFile(FilePath);

        // Resave to the default file, to reset the default Persistant data file
        m_PersistData.SavePersistantDataToFile(m_PersistData.GetDefaultPersistantDataFile());
    }
}

void SSTBuilder::on_LoadProfile_clicked()
{
    QString currDirName;
    QString FilePath;

    // Display to the user a select directory dialog box

    // Get the path typed in the edit box (if it exists)
    currDirName = QDir::homePath();

    // Get the path to the SST Directory
    FilePath = QFileDialog::getOpenFileName(this, tr("Open SSTBuilder Profile"), currDirName,
              tr("SSTBuilder Profiles (*.sbdr);;All Files (*.*)"));

    // Set the path in the edit control (NOTE; This will force a call to on_sstRootPathEdit_textChanged() )
    if ("" != FilePath) {
        // Load the data from a file
        m_PersistData. LoadPersistantDataFromFile(FilePath);

        // Build a fileinfo object to get the file basename
        QFileInfo FileInfo(FilePath);
        setWindowTitle("SSTBuilder - " + FileInfo.baseName() + " Profile");

        // Resave to the default file, to reset the default Persistant data file
        m_PersistData.SavePersistantDataToFile(m_PersistData.GetDefaultPersistantDataFile());
    }

    // Clear all the tabs and Create a tab telling the user we are rebuilding the configuration file
    deleteAllTabs();
    addNoConfigTab(tr("Loading new SSTBuilder Profile..."));
    ui->sstRootPathEdit->setText(m_PersistData.GetSSTRootPath());
    // See if the configure file was created, and if yes repopulate
    testForConfigureFileExists(true);
}

void SSTBuilder::on_LeftBtn_clicked()
{
    int CurrentIndex = ui->TabPagesStackedWidget->currentIndex();
    int NewIndex = CurrentIndex - 1;

    if (NewIndex < 0) {
        NewIndex = 0;
    }

    if (CurrentIndex != NewIndex) {
        ui->TabPagesStackedWidget->setCurrentIndex(NewIndex);
        ui->TabPageTitlesCombo->setCurrentIndex(NewIndex);

        // Check the left right buttons to enable/disable
        checkLeftRightButtonEnables();

        // When the Tab Changes, save the persistant data
        m_PersistData.SavePersistantData();
    }
}

void SSTBuilder::on_RightBtn_clicked()
{
    int CurrentCount = ui->TabPagesStackedWidget->count();
    int CurrentIndex = ui->TabPagesStackedWidget->currentIndex();
    int NewIndex = CurrentIndex + 1;

    if (NewIndex >= CurrentCount) {
        NewIndex = CurrentCount - 1;
    }

    if (CurrentIndex != NewIndex) {
        ui->TabPagesStackedWidget->setCurrentIndex(NewIndex);
        ui->TabPageTitlesCombo->setCurrentIndex(NewIndex);

        // Check the left right buttons to enable/disable
        checkLeftRightButtonEnables();

        // When the Tab Changes, save the persistant data
        m_PersistData.SavePersistantData();
    }
}

void SSTBuilder::on_TabPageTitlesCombo_currentIndexChanged(int index)
{
    ui->TabPagesStackedWidget->setCurrentIndex(index);

    // Check the left right buttons to enable/disable
    checkLeftRightButtonEnables();

    // When the Tab Changes, save the persistant data
    m_PersistData.SavePersistantData();

}

void SSTBuilder::on_actionQuit_triggered()
{
    on_exitButton_clicked();
}

void SSTBuilder::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}

void SSTBuilder::on_actionPreferences_triggered()
{
    on_setupButton_clicked();
}

void SSTBuilder::on_actionSave_Profile_triggered()
{
    on_SaveProfile_clicked();
}

void SSTBuilder::on_actionLoad_Profile_triggered()
{
    on_LoadProfile_clicked();
}

void SSTBuilder::on_runAutoGenScriptButton_clicked()
{
    QString ProcessName       = "autogen.sh";
    QString ShellProcessCall  = m_sstAutogenFilePath;
    QString ProcessingRunMsg  = "BUILDING THE CONFIGURATION FILE...";

    // Create the Logger
    CreateLogFile("sstbuilderlog_autogen_sh");

    // Run the configure script
    RunShellProcess(ProcessName, ShellProcessCall, ProcessingRunMsg, SLOT(processFinishedAutogen(int, QProcess::ExitStatus)));
}

void SSTBuilder::on_runConfigureButton_clicked()
{
    QString ProcessName       = "configure";
    QString ShellProcessCall  = "bash -c \"" + BuildConfigureExecutionString() + "\"";
    QString ProcessingRunMsg  = "RUNNING THE CONFIGURATION FILE...";

    // Create the Logger
    CreateLogFile("sstbuilderlog_configure");

    // Run the configure script
    RunShellProcess(ProcessName, ShellProcessCall, ProcessingRunMsg, SLOT(processFinishedConfiguration(int, QProcess::ExitStatus)),
                    true, BuildConfigureExecutionString());
}

void SSTBuilder::on_runMakeButton_clicked()
{
    QString ProcessName       = "make";
    QString ShellProcessCall  = "bash -c make";
    QString ProcessingRunMsg  = "RUNNING THE MAKE FILE...";

    // Create the Logger
    CreateLogFile("sstbuilderlog_make");

    // Run the configure script
    RunShellProcess(ProcessName, ShellProcessCall, ProcessingRunMsg, SLOT(processFinishedMakefile(int, QProcess::ExitStatus)));
}

void SSTBuilder::on_runMakeInstallButton_clicked()
{
    QString ProcessName       = "make install";
    QString ShellProcessCall  = "bash -c make install";
    QString ProcessingRunMsg  = "RUNNING THE MAKE FILE (WITH INSTALL OPTION)...";

    // Create the Logger
    CreateLogFile("sstbuilderlog_make_install");

    // Run the configure script
    RunShellProcess(ProcessName, ShellProcessCall, ProcessingRunMsg, SLOT(processFinishedMakefile(int, QProcess::ExitStatus)));
}

void SSTBuilder::processFinishedAutogen(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Use the default Finished Process Hander
    FinishShellProcess(exitCode, exitStatus);
}

void SSTBuilder::processFinishedConfiguration(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Use the default Finished Process Hander
    FinishShellProcess(exitCode, exitStatus);
}

void SSTBuilder::processFinishedMakefile(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Use the default Finished Process Hander
    FinishShellProcess(exitCode, exitStatus);
}

void SSTBuilder::processOutput()
{
    QString ProcessStdout;

    // Receive the output from the running proess and display it in the text field

    //Get the output
    ProcessStdout = m_pshellProcess->readAllStandardOutput();
    outputGenericText(ProcessStdout, "black");
    return;
}

void SSTBuilder::RunShellProcess(QString ProcessName, QString ShellProcessCall, QString ProcessingRunMsg,
                                 const char* ProcessFinshedSlotMethod, bool DisplayCallFirst/*=false*/,
                                 QString DisplayCallText/*=""*/)
{
    QProcessEnvironment env;
    QString             ExecString;
    QString             UserInfo;

    if (true == DisplayCallFirst) {
        UserInfo = "Running this Command:\n\n" + DisplayCallText;
        if (QMessageBox::question(this, "Running This Command...", UserInfo, QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) {
            // The user cancels the call
            return;
        }
    }

    // We want to run the Shell Process, however due to the nature of operation of the app on the system,
    // we must setup any required process environment variables first.  Then we run the process redirecting its
    // output to our text edit control to display the results.

    // Disable controls while we are running
    ui->runConfigureButton->setEnabled(false);
    ui->runAutoGenScriptButton->setEnabled(false);
    ui->setupButton->setEnabled(false);
    ui->SaveProfile->setEnabled(false);
    ui->LoadProfile->setEnabled(false);
    ui->runMakeButton->setEnabled(false);
    ui->runMakeInstallButton->setEnabled(false);
    ui->sstRootPathEdit->setReadOnly(true);

    // Build the execution string
    ExecString = ShellProcessCall;

    // Clear all the tabs and Create a tab telling the user we are running the process
    deleteAllTabs();
    addNoConfigTab(ProcessingRunMsg);

    // Clear the output display
    ui->runOutputEdit->setText("");

    // Get the Environment
    env = QProcessEnvironment::systemEnvironment();

    // Add the custom environment variables defined by the user
    BuildProcessEnvWithCustomVariables(env);

    // Create a new Shell Process
    m_pshellProcess = new QProcess();

    // Set the working dir and environment for the process
    m_pshellProcess->setWorkingDirectory(m_sstRootFileDir);
    m_pshellProcess->setProcessEnvironment(env);

    // Send both stderr and stdout to stdout and interleave them
    m_pshellProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_pshellProcess->setReadChannel(QProcess::StandardOutput);

    // Connect signals from the process to handler (This will send all output to the processOutput() method,
    // and call the processFinishedAutogen() when the process finishes)
    connect(m_pshellProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()) );
    connect(m_pshellProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, ProcessFinshedSlotMethod);

    // Set the cursor to show that we are busy (autogen.sh takes some time)
    qApp->setOverrideCursor(QCursor(Qt::BusyCursor));

    // Display the process environment for the user
    outputCurrentProcessEnvironment(env);

    // Indicate to the User that we are running
    outputGenericText("*** STARTING RUN OF " + ProcessName + " ***\n", "dodgerblue");

    // Output the String we will be running
    outputGenericText(ExecString + "\n", "green");

    // Start the shell process for the Exec String
    m_pshellProcess->start(ExecString);
}

void SSTBuilder::FinishShellProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)

    // Did the autogen.sh finish correctly
    if ((QProcess::NormalExit == exitStatus)) {

        // We are finished running the process, reenable the controls and reset the cursor
        ui->setupButton->setEnabled(true);
        ui->SaveProfile->setEnabled(true);
        ui->LoadProfile->setEnabled(true);
        ui->sstRootPathEdit->setReadOnly(false);

        // Indicate to the User that we are done
        outputGenericText(tr("*** FINISHED RUNNING ***\n"), "dodgerblue");
        outputGenericText("\n");

        qApp->restoreOverrideCursor();

        // See if we can enable the AutoGen Script Button
        testForAutoGenFileExists();

        // See if the configure file was created, and if yes repopulate
        testForConfigureFileExists(true);

        // See if we can enable the Make File buttons
        ui->runMakeButton->setEnabled(true);
        ui->runMakeInstallButton->setEnabled(true);
    }

    // Close the Logger
    void CloseLogFileStream();

    // Delete the Shell Process
    delete m_pshellProcess;
    m_pshellProcess = NULL;
}

void SSTBuilder::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)  // To avoid warning
    QSize         tabWidgetSize;
    TabPageEntry* curTabPage;
    QScrollArea*  page;
    int           x;

    // When the main window resizes, we need to resize all the tab pages

    // Get the size of the TabWidget
    tabWidgetSize = ui->TabPagesStackedWidget->size();

    for (x = 0; x < ui->TabPagesStackedWidget->count(); x++) {
        // Get the currently selected widget (ScrollArea then the TabPage)
        page = (QScrollArea*)ui->TabPagesStackedWidget->widget(x);
        curTabPage = (TabPageEntry*)page->widget();

        // If the Configuration Tabs are loaded, then update them all
        if ((curTabPage != 0) && m_sstConfigTabsLoaded) {
            curTabPage->setParentSize(tabWidgetSize);
        }
    }
}

void SSTBuilder::outputGenericText(QString text, QString color/* = "black" */)
{
    QColor   OldColor;
    QColor   ColorOrchid = QColor("orchid");
    QColor   ColorRed    = QColor("red");
    QColor   NewColor    = QColor(color);
    QColor   FoundColor;
    QString  OutputText;
    QString  FoundText;
    QString  TestLine;
    int      FoundIndex;

    // NOTE: USING THIS ROUTINE, THE USER MUST INCLUE ANY \n CHARACTERS THAT THEY NEED

    // Make sure we are adding text to the end of the line
    ui->runOutputEdit->moveCursor(QTextCursor::End);

    // Now we scan the line for the word "warning" or "error", if found, we output those strings with a different color
    TestLine = text;
    do {
        // Test for Warning
        FoundIndex = TestLine.indexOf("warning", 0, Qt::CaseInsensitive);
        if (-1 != FoundIndex) {
            FoundColor = ColorOrchid;
            OutputText = TestLine.left(FoundIndex);
            FoundText = TestLine.mid(FoundIndex, 7);      // The length of "warning"
            TestLine = TestLine.mid(FoundIndex + 7, -1);  // Reset the TestLine to Start past the "warning"
        } else {
            // Test for Error
            FoundIndex = TestLine.indexOf("error", 0, Qt::CaseInsensitive);
            if (-1 != FoundIndex) {
                FoundColor = ColorRed;
                OutputText = TestLine.left(FoundIndex);
                FoundText = TestLine.mid(FoundIndex, 5);      // The length of "error"
                TestLine = TestLine.mid(FoundIndex + 5, -1);  // Reset the TestLine to Start past the "error"
            } else {
                FoundColor = NewColor;
                OutputText = TestLine;
                FoundText = "";
            }
        }

        // Now lets output the text with the NewColor, then the found text with the FoundColor

        // Get a copy of the current color
        OldColor = ui->runOutputEdit->textColor();

        // Switch to the new color
        ui->runOutputEdit->setTextColor(NewColor);

        // Output the text
        ui->runOutputEdit->insertPlainText(OutputText);

        // Send the output to the logger also
        WriteLogFile(OutputText);

        // Setup the color and output the found text
        if (!FoundText.isEmpty()) {
            ui->runOutputEdit->setTextColor(FoundColor);
            ui->runOutputEdit->insertPlainText(FoundText);
        }

        // Reset back to the old color
        ui->runOutputEdit->setTextColor(OldColor);

    } while (-1 != FoundIndex);

    // scroll down to show it.
    ui->runOutputEdit->ensureCursorVisible();
}

void SSTBuilder::BuildProcessEnvWithCustomVariables(QProcessEnvironment& env)
{
    QString             Key;
    QString             Value;
    QString             ExpandVar;
    QString             ReplaceValue;
    QString             FinalValue;
    QString             GroupName;
    QString             GroupEnabled;
    int                 x;
    int                 y;
    int                 startIndex = 0;
    int                 endIndex = 0;

    // Look at each Environment Group and see if it is enabled
    for (x = 0; x < m_PersistData.GetEnvGroupNamesList().count(); x++) {
        GroupName = m_PersistData.GetEnvGroupNamesList().at(x);
        GroupEnabled = m_PersistData.GetEnvGroupEnablesList().at(x);

        // Only add the group environment variables if the group is enabled
        if ("1" == GroupEnabled) {
            for (y = 0; y < m_PersistData.GetEnvVarNamesList(GroupName).count(); y++) {
                Key    = m_PersistData.GetEnvVarNamesList(GroupName).at(y);
                Value  = m_PersistData.GetEnvVarValuesList(GroupName).at(y);

                startIndex = 0;
                // Now scan the Value for any variable names to expand
                // Repeat until all variables are expanded
                while (-1 != startIndex) {
                    // Search for the first $
                    startIndex = Value.indexOf("$", startIndex);
                    if (-1 != startIndex) {
                        // We found a $, now find where the variable ends, we use a regular expression here
                        // We are looking for a character that is not a letter or non a number or not an underscore
                        endIndex = Value.indexOf(QRegExp("[^a-zA-Z0-9_]"), startIndex + 1);
                        if (-1 != endIndex) {
                            // We found the end of the variable to expand,
                            // so find its real value from the env and replace the variable
                            ExpandVar = Value.mid(startIndex + 1, endIndex - startIndex - 1);
                            ReplaceValue = env.value(ExpandVar);

                            // Now replace the value with the Data we retrieved
                            Value.replace(startIndex, endIndex - startIndex, ReplaceValue);

                            // Reset the startIndex back to 0 to restart the search
                            startIndex = 0;
                        } else {
                            // We did not find the end of the variable, so just copy till the end and then
                            // convert it
                            ExpandVar = Value.mid(startIndex + 1, -1);
                            ReplaceValue = env.value(ExpandVar);

                            // Now replace the value with the Data we retrieved
                            Value.replace(startIndex, 9999, ReplaceValue);

                            // Reset the startIndex back to 0 to restart the search
                            startIndex = 0;
                        }
                    } else {
                        // We did not find the $, so copy the value to the final value
                        FinalValue = Value;
                    }
                }

                // Add the entry
                env.insert(Key, FinalValue);
            }
        }
    }
}

QString SSTBuilder::BuildConfigureExecutionString()
{
    QString       rtnString = "";
    QScrollArea*  page;
    TabPageEntry* curTabPage;
    int           x;
    int           y;
    HeaderData*   headerData;
    EntryData*    entryData;

    // Create the configure string
    rtnString = m_sstRootFileDir + "/configure";

    // Now go through all the TabPages and if anything is selected
    for (x = 0; x < ui->TabPagesStackedWidget->count(); x++) {
        // Get the currently selected widget (ScrollArea then the TabPage)
        page = (QScrollArea*)ui->TabPagesStackedWidget->widget(x);
        curTabPage = (TabPageEntry*)page->widget();

        // If the Configuration Tabs are loaded, then it is valid
        // Get the Header data for this page
        if ((curTabPage != 0) && m_sstConfigTabsLoaded) {
            headerData = curTabPage->GetHeaderData();
            // Now check each entry
            for (y = 0; y < headerData->getNumEntries(); y++) {
                // Get the entry
                entryData = headerData->getEntry(y);

                // Did the user select this entry?
                if (true == entryData->getUserEntrySelected()) {
                    // Check to see if this is an Normal entry or not, if not then dont add the "--"
                    if ("Normal" == headerData->getHeaderType()) {
                        rtnString += " --";
                    } else {
                        rtnString += " ";
                    }
                    // Add the entry name to the string
                    rtnString += entryData->getConfigEntryName();
                    // See if there is a variable assisted with this entry
                    if ("" != entryData->getUserEntryVarData()) {
                        rtnString += "=";
                        rtnString += entryData->getUserEntryVarData();
                    }
                }
            }
        }
    }

    return rtnString;
}

void SSTBuilder::outputCurrentProcessEnvironment(QProcessEnvironment& env)
{
    QStringList envVarList;
    QString     envVarText;
    int         x;

    outputGenericText(tr("*** START DUMP OF PROCESS ENVIRONMENT VARS ***\n"), "orange");

    // Get the List of Env Variables
    envVarList = env.toStringList();

    // Display the one at a time
    for (x = 0; x < envVarList.count(); x++) {
        envVarText = envVarList[x];
        outputGenericText(envVarText + "\n");
    }

    outputGenericText(tr("*** END DUMP OF PROCESS ENVIRONMENT VARS ***\n"), "orange");
    outputGenericText("\n");
}

bool SSTBuilder::testForAutoGenFileExists()
{
    QString         autogenpath;
    QFileInfo       fileInfo;

    // Get the path typed in the edit box
    autogenpath = ui->sstAutoGenPathEdit->text();

    // See if the autogenpath is empty
    if (!autogenpath.isEmpty()) {

        // Test to make sure the autogen.sh exists
        fileInfo.setFile(autogenpath);
        if (fileInfo.exists()) {
            // Enable the run autogen.sh button
            ui->runAutoGenScriptButton->setEnabled(true);
            return true;
        } else {
            // Disable the run autogen.sh button
            ui->runAutoGenScriptButton->setEnabled(false);
            return false;
        }
    }
    return false;
}

bool SSTBuilder::testForConfigureFileExists(bool forceRepopulate /*= false*/)
{
    QFileInfo fileInfo;

    // Test to see if configure file exists
    fileInfo.setFile(m_sstConfigFilePath);
    if (fileInfo.exists()) {
        // Are we going from no config to existing config or are we forcing a repopulate?
        // We test this to prevent repeated delete and create of tabs
        if ((false == m_sstConfigFileExists) || (true == forceRepopulate)) {
            // Clear out the tabs and then repopulate the tabs
            deleteAllTabs();
            populateConfigureTabInfo();
        }

        // Enable the run configure button
        ui->runConfigureButton->setEnabled(true);

        // Also check to see if the make file exists
        testForMakeFileExists();

        m_sstConfigFileExists = true;  // This must happen at end
    } else {
        // Clear out the tabs and tell user about no config file
        deleteAllTabs();
        addNoConfigTab(tr("Configuration file not found - May need to run autogen.sh"));

        // Disable the run configure button
        ui->runConfigureButton->setEnabled(false);

        // Also disable the MakeFile buttons (we want a configure file to exist first)
        ui->runMakeButton->setEnabled(false);
        ui->runMakeInstallButton->setEnabled(false);

        m_sstConfigFileExists = false;  // This must happen at end
    }
    return m_sstConfigFileExists;
}

bool SSTBuilder::testForMakeFileExists()
{
    QString         MakeFilePath;
    QFileInfo       fileInfo;

    // Get the path for the makefile
    MakeFilePath = m_sstRootFileDir + "/Makefile";

    // See if the autogenpath is empty
    if (!MakeFilePath.isEmpty()) {

        // Test to make sure the autogen.sh exists
        fileInfo.setFile(MakeFilePath);
        if (fileInfo.exists()) {
            return true;
        } else {
            // Disable the MakeFile buttons
            ui->runMakeButton->setEnabled(false);
            ui->runMakeInstallButton->setEnabled(false);
            return false;
        }
    }
    return false;
}

void SSTBuilder::checkLeftRightButtonEnables()
{
    int CurrentCount = ui->TabPagesStackedWidget->count();
    int CurrentIndex = ui->TabPagesStackedWidget->currentIndex();

    ui->LeftBtn->setEnabled(CurrentIndex != 0);
    ui->RightBtn->setEnabled(CurrentIndex < CurrentCount - 1);
}

void SSTBuilder::ForceResize()
{
    QSize newSize(0,0);
    QResizeEvent evtResize(newSize, newSize);

    this->resizeEvent(&evtResize);
}

int SSTBuilder::addSingleTab(QString Name, QScrollArea* pNewScrollArea)
{
    int nRtn;
    int CurrentCount = ui->TabPagesStackedWidget->count();

    // Add the Scroll area (containing the new TabPage widget) to the Stacked Widget control
    nRtn = ui->TabPagesStackedWidget->insertWidget(CurrentCount, pNewScrollArea);
    ui->TabPagesStackedWidget->setCurrentIndex(0);

    ui->TabPageTitlesCombo->insertItem(CurrentCount, Name);
    ui->TabPageTitlesCombo->setCurrentIndex(0);

    checkLeftRightButtonEnables();

    return nRtn;
}

void SSTBuilder::deleteAllTabs()
{
    TabPageEntry*  pTabPage;
    int            x;
    int            tabCount;

    tabCount = ui->TabPagesStackedWidget->count();

    // Remove any existing tabs
    for (x = 0; x < tabCount; x++) {
        pTabPage = (TabPageEntry*)ui->TabPagesStackedWidget->widget(0);
        if (NULL != pTabPage) {
            delete pTabPage;
        }
        // Remove the Widget from the Stacked Widget
        ui->TabPagesStackedWidget->removeWidget(0);
        // Remove the text from the combobox
        ui->TabPageTitlesCombo->removeItem(0);
    }

    m_sstConfigTabsLoaded = false;
}

void SSTBuilder::addNoConfigTab(QString Msg)
{
    int              tabIndex;
    TabPageEntry*    pNewTabPage;
    QScrollArea*     pNewScrollArea;

    m_sstConfigTabsLoaded = false;

    // Build the tab and set its title
    pNewTabPage = (TabPageEntry*)new TabPageEntry(QString(""));

    // Build a scroll Area and add the NewPage to it
    pNewScrollArea = new QScrollArea(this);
    pNewScrollArea->setWidget(pNewTabPage);
    pNewScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the Scroll area (containing the new TabPage widget) to the Main Screen
    tabIndex = addSingleTab(Msg, pNewScrollArea);
}

void SSTBuilder::populateConfigureTabInfo()
{
    ConfigFileParser FileParser;
    int              x;
    int              tabIndex;
    TabPageEntry*    pNewTabPage;
    HeaderData*      pHeaderData;
    QScrollArea*     pNewScrollArea;

    // Parse the config file and then add the main headers as TabPages (1 for each header)
    // with their associated entries displayed

    // Setup the Configuration File Parser
    FileParser.setConfigFilePath(m_sstConfigFilePath);
    FileParser.parseConfigFile();

    // Check to see if no Config Headers were Found
    if (0 == FileParser.getNumberOfHeaders()) {
        // Clear out the tabs and tell user about no config file
        deleteAllTabs();
        addNoConfigTab(tr("No Headers were found in the configuration file - Check your ~/.sstbuilder and configure files"));
        return;
    }

    // Now Build a TabPage for each Header
    for (x = 0; x < FileParser.getNumberOfHeaders(); x++) {
        // Get the Header Data
        pHeaderData = FileParser.getHeader(x);

        // Build the tab and set its title
        pNewTabPage = (TabPageEntry*)new TabPageEntry(pHeaderData->getHeaderName());
        pNewTabPage->populateHeaderInfo(pHeaderData);

        // Build a scroll Area and add the NewPage to it
        pNewScrollArea = new QScrollArea(this);
        pNewScrollArea->setWidget(pNewTabPage);
        pNewScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // Add the Scroll area (containing the new TabPage widget) to the Main Screen
        tabIndex = addSingleTab(pHeaderData->getHeaderName(), pNewScrollArea);
    }

    // Call the Resize routine to re-calculate the size of the TabPages
    m_sstConfigTabsLoaded = true;
    ForceResize();
}

void SSTBuilder::CreateLogFile(QString FileName)
{
    QString   LogFilePath;
    QDateTime CurrentTime = QDateTime::currentDateTime();
    QString   Timestamp;
    QString   FirstText;

    // Build the timestamp
    Timestamp = CurrentTime.toString("yyyy-MM-dd-hh-mm-ss");

    // Create the Log File
    LogFilePath = m_sstRootFileDir + "/" + FileName + "-"+ Timestamp + ".txt";

    m_pLoggingFile = new QFile(LogFilePath);
    if (NULL != m_pLoggingFile) {
        if (m_pLoggingFile->open(QFile::WriteOnly | QFile::Truncate)) {

            // Tell the user some info inside the file
            FirstText = "Logging output started at " + Timestamp + " for " + FileName;
            WriteLogFile(FirstText);
        }
    }
}

void SSTBuilder::WriteLogFile(QString OutputText)
{
    if ((NULL != m_pLoggingFile) && (true == m_pLoggingFile->exists())) {
        // Create the stream, and use it
        QTextStream outputStream(m_pLoggingFile);
        outputStream << OutputText;
    }
}

void SSTBuilder::CloseLogFile()
{
    if (NULL != m_pLoggingFile) {
        m_pLoggingFile->close();
        delete m_pLoggingFile;
        m_pLoggingFile = NULL;
    }
}


