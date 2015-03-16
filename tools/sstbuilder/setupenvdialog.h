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

#ifndef SETUPENVDIALOG_H
#define SETUPENVDIALOG_H

#include <QDialog>
#include <QKeyEvent>

#include "persistantdata.h"

namespace Ui {
class SetupEnvDialog;
}

class SetupEnvDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetupEnvDialog(QString GroupName, QWidget *parent = 0);
    ~SetupEnvDialog();
    
private:
    void setupEnvTable();
    void PopulateEnvVars();
    void AddEnvEntry(QString EnvName, QString EnvValue);
    void SaveEnvData();

private slots:
    // Handlers for Messages from the UI Controls
    void on_CloseBtn_clicked();
    void keyPressEvent( QKeyEvent *e );
    void on_AddBtn_clicked();
    void on_RemoveBtn_clicked();

private:
    QString             m_GroupName;
    PersistantData      m_PersistantData;
    Ui::SetupEnvDialog* ui;
};

#endif // SETUPENVDIALOG_H
