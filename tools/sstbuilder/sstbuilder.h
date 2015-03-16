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

#ifndef SSTBUILDER_H
#define SSTBUILDER_H

#include <QtPrintSupport>

#include "setupgeneraldialog.h"
#include "tabpageentry.h"
#include "configfileparser.h"
#include "aboutdialog.h"

namespace Ui {
class SSTBuilder;
}

class SSTBuilder : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SSTBuilder(QWidget *parent = 0);
    ~SSTBuilder();

private slots:
    // Handlers for the output of the external programs (autogen.sh and configure)
    void ShowRunOutputEditContextMenu(const QPoint& pos);

    // Handlers for Messages from the UI Controls
    void on_exitButton_clicked();
    void on_sstRootPathEdit_textChanged(const QString &arg1);
    void on_selectSSTRootPathButton_clicked();

    void on_setupButton_clicked();
    void on_SaveProfile_clicked();
    void on_LoadProfile_clicked();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_actionSave_Profile_triggered();
    void on_actionLoad_Profile_triggered();
    void on_LeftBtn_clicked();
    void on_RightBtn_clicked();
    void on_TabPageTitlesCombo_currentIndexChanged(int index);

    void on_runAutoGenScriptButton_clicked();
    void on_runConfigureButton_clicked();
    void on_runMakeButton_clicked();
    void on_runMakeInstallButton_clicked();

    void processFinishedAutogen(int exitCode, QProcess::ExitStatus exitStatus);
    void processFinishedConfiguration(int exitCode, QProcess::ExitStatus exitStatus);
    void processFinishedMakefile(int exitCode, QProcess::ExitStatus exitStatus);
    void processOutput();

private:
    // Code to handle Running a Process
    void RunShellProcess(QString ProcessName, QString ShellProcessCall, QString ProcessingRunMsg,
                         const char* ProcessFinshedSlotMethod, bool DisplayCallFirst = false,
                         QString DisplayCallText = "");
    void FinishShellProcess(int exitCode, QProcess::ExitStatus exitStatus);

    // Support Functions
    void resizeEvent(QResizeEvent* event);
    void outputGenericText(QString text, QString color = "black");
    void BuildProcessEnvWithCustomVariables(QProcessEnvironment& env);
    QString BuildConfigureExecutionString();
    void outputCurrentProcessEnvironment(QProcessEnvironment& env);
    bool testForAutoGenFileExists();
    bool testForConfigureFileExists(bool forceRepopulate = false);
    bool testForMakeFileExists();
    void checkLeftRightButtonEnables();
    void ForceResize();
    int  addSingleTab(QString Name, QScrollArea* pNewScrollArea);
    void deleteAllTabs();
    void addNoConfigTab(QString Msg);
    void populateConfigureTabInfo();
    void CreateLogFile(QString FileName);
    void WriteLogFile(QString OutputText);
    void CloseLogFile();

    // Internal variables
    Ui::SSTBuilder*     ui;
    QString             m_sstRootFileDir;
    QString             m_sstAutogenFilePath;
    QString             m_sstConfigFilePath;
    bool                m_sstConfigFileExists;
    bool                m_sstConfigTabsLoaded;
    QProcess*           m_pshellProcess;
    SetupGeneralDialog* m_pSetupGeneralDialog;
    PersistantData      m_PersistData;
    QFile*              m_pLoggingFile;
};

#endif // SSTBUILDER_H
