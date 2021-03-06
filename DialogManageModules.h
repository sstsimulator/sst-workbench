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

#ifndef DIALOGMANAGEMODULES_H
#define DIALOGMANAGEMODULES_H

#include "GlobalIncludes.h"

#include "GraphicItemComponent.h"

//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class DialogManageModules;
}

//////////////////////////////////////////////////////////////////////////////

class DialogManageModules : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManageModules(GraphicItemComponent* SelectedComponent, SSTInfoData* InfoData, QWidget* parent = 0);
    ~DialogManageModules();

private:
    // Table Management
    void SetupTables();
    void PopulateComponentModulesTable();
    void PopulateAvailableModulesTable();
    void AddModuleNameToComponentSide(QString ModuleName);
    void RemoveModuleNameFromComponentSide(QString ModuleName);

    // Save the Changes back to the Port
    void SaveData();

private slots:
    void on_BtnMoveLeft_clicked();
    void on_BtnMoveRight_clicked();
    void on_closeButton_clicked();
    void keyPressEvent(QKeyEvent* evt);
    void on_listWidgetAvailableModules_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidgetComponentModules_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::DialogManageModules* ui;
    GraphicItemComponent*    m_SelectedComponent;
    SSTInfoData*             m_SSTInfoData;
};

#endif // DIALOGMANAGEMODULES_H
