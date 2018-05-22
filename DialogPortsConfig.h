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

#ifndef DIALOGPORTSCONFIG_H
#define DIALOGPORTSCONFIG_H

#include "GlobalIncludes.h"

#include "GraphicItemComponent.h"

//////////////////////////////////////////////////////////////////////////////

// The User Interface Form
namespace Ui {
    class DialogPortsConfig;
}

//////////////////////////////////////////////////////////////////////////////

class DialogPortsConfig : public QDialog
{
    Q_OBJECT
public:
    // Constructor / Destructor
    explicit DialogPortsConfig(GraphicItemComponent* SelectedComponent, QWidget* parent = 0);
    ~DialogPortsConfig();

signals:
    
private:
    // Table Management
    void SetupTable();
    void PopulatePortData();
    void AddPortDataEntry(int Index, QString& EnvName, QString& EnvValue);
    // Save the Changes back to the Port
    void SavePortData();

private slots:
    // Handlers for Messages from the UI Controls
    void on_CloseBtn_clicked();
    void keyPressEvent(QKeyEvent* evt);

private:
    Ui::DialogPortsConfig* ui;
    SpinBoxEditor*         m_SpinBoxEditor;
    GraphicItemComponent*  m_SelectedComponent;
};

#endif // DIALOGPORTSCONFIG_H
