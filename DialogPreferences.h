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

#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include "GlobalIncludes.h"

//////////////////////////////////////////////////////////////////////////////

// The User Interface Form
namespace Ui {
    class DialogPreferences;
}

//////////////////////////////////////////////////////////////////////////////

class DialogPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPreferences(QWidget* parent = 0);
    ~DialogPreferences();

    void SetFlagReturnToSelectAfterWirePlaced(bool flag);
    void SetFlagReturnToSelectAfterTextPlaced(bool flag);
    void SetFlagAutomaticallyDeleteShortWires(bool flag);
    void SetFlagDisplayGridEnabled(bool flag);
    void SetFlagSnapToGridEnabled(bool flag);
    void SetGridSize(int GridSize);

    ////

    bool GetFlagReturnToSelectAfterWirePlaced();
    bool GetFlagReturnToSelectAfterTextPlaced();
    bool GetFlagAutomaticallyDeleteShortWires();
    bool GetFlagDisplayGridEnabled();
    bool GetFlagSnapToGridEnabled();
    int GetGridSize();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_BackgroundImageFileSelect_clicked();

private:
    Ui::DialogPreferences* ui;
};

#endif // DIALOGPREFERENCES_H
