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

#ifndef WINDOWITEMPROPERTIES_H
#define WINDOWITEMPROPERTIES_H

#include "GlobalIncludes.h"

#include "ItemProperties.h"

////////////////////////////////////////////////////////////

class WindowItemProperties : public QFrame
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit WindowItemProperties(QWidget* parent = 0);
    ~WindowItemProperties();

    void AddPropertyData(QString Property, QString Value, QString Desc, bool ReadOnly = false);

    void SetGraphicItemProperties(ItemProperties* Properties);
    void ClearProperiesWindow();
    void RefreshProperiesWindow(QString PropertyName, QString NewPropertyValue);

public slots:
    void HandleCellChanged(int, int);

signals:

private:
    QGroupBox*      m_PropertiesGroupBox;
    QTableWidget*   m_PropertiesTable;

    ItemProperties* m_CurrentProperties;
    bool            m_PopulatingWindow;
};

#endif // WINDOWITEMPROPERTIES_H

