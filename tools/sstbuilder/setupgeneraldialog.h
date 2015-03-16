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

#ifndef SETUPGENERALDIALOG_H
#define SETUPGENERALDIALOG_H

#include <QProcessEnvironment>
#include <QMessageBox>
#include <QTableWidget>

#include "persistantdata.h"
#include "setupenvdialog.h"

namespace Ui {
class SetupGeneralDialog;
}

class SetupGeneralDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetupGeneralDialog(QWidget *parent = 0);
    ~SetupGeneralDialog();

private:
    void setupTables();
    void PopulateEnvVars();
    void AddEnvEntry(QString EnvName, QString EnvValue);

    void PopulateEnvGroupData();
    void AddEnvGroupEntry(QString GroupName, bool Enabled);
    void CopyEnvGroupEntry(QString ExistingGroupName, QString NewGroupName, bool Enabled);
    void RemoveEnvGroupEntry(QString GroupName);
    void SaveEnvGroupData();

private slots:
    // Handlers for Messages from the UI Controls
    void on_CloseBtn_clicked();
    void keyPressEvent( QKeyEvent *e );
    void on_AddBtn_clicked();
    void on_CopyBtn_clicked();
    void on_RemoveBtn_clicked();
    void handle_setup_button_clicked(bool checked = false);
    void on_tableWidget_EnvGroups_itemSelectionChanged();
    void on_tableWidget_EnvGroups_cellChanged(int row, int column);
    void on_tableWidget_EnvGroups_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::SetupGeneralDialog* ui;
    PersistantData          m_PersistantData;
    SetupEnvDialog*         m_pSetupEnvDialog;
    bool                    m_PopulatingEnvGroupData;
    QString                 m_EnvGroupNameBeingEdited;
};

#endif // SETUPGENERALDIALOG_H
