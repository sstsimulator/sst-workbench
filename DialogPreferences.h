////////////////////////////////////////////////////////////////////////
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

    bool GetFlagReturnToSelectAfterWirePlaced();
    bool GetFlagReturnToSelectAfterTextPlaced();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::DialogPreferences* ui;
    bool m_FlagReturnToSelectAfterWirePlaced;
    bool m_FlagReturnToSelectAfterTextPlaced;
};

#endif // DIALOGPREFERENCES_H
