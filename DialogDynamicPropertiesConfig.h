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

#ifndef DIALOGDYNAMICPROPERTIESCONFIG_H
#define DIALOGDYNAMICPROPERTIESCONFIG_H

#include "GlobalIncludes.h"

//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class DialogDynamicPropertiesConfig;
}

//////////////////////////////////////////////////////////////////////////////

class DialogDynamicPropertiesConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDynamicPropertiesConfig(ItemProperty* SelectedProperty, QWidget *parent = 0);
    ~DialogDynamicPropertiesConfig();

signals:

private:
    // Table Management
    void SetupTable();
    void PopulatePropertyData();
    void AddPropertyDataEntry(QString Name, QString Value);
    // Save the Changes back to the Property
    void SavePropertyData();

private slots:
    // Handlers for Messages from the UI Controls
    void on_CloseBtn_clicked();
    void keyPressEvent(QKeyEvent* evt);

private:
    Ui::DialogDynamicPropertiesConfig* ui;
    SpinBoxEditor*                     m_SpinBoxEditor;
    ItemProperty*                      m_SelectedProperty;
};

#endif // DIALOGDYNAMICPROPERTIESCONFIG_H
