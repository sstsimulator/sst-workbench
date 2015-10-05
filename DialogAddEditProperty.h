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

#ifndef DIALOGADDEDITPROPERTY_H
#define DIALOGADDEDITPROPERTY_H

#include "GlobalIncludes.h"

//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class DialogAddEditProperty;
}

//////////////////////////////////////////////////////////////////////////////

class DialogAddEditProperty : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddEditProperty(ItemProperties* Properties, QString Name = "", QString Value = "", QString Desc = "", QWidget* parent = 0);
    ~DialogAddEditProperty();

private slots:
    // Handlers for Messages from the UI Controls
    void on_buttonBox_accepted();
    void on_BtnUnlockName_clicked();
    void on_BtnUnlockDesc_clicked();

private:
    Ui::DialogAddEditProperty*         ui;
    ItemProperties*                    m_Properties;
    QString                            m_StartingProperyName;
    bool                               m_EditMode; // true = edit; false = Add
    QIcon                              m_Icon_Locked;
    QIcon                              m_Icon_Unlocked;
    bool                               m_NameFieldLocked;
    bool                               m_DescFieldLocked;
};

#endif // DIALOGADDEDITPROPERTY_H
