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

#ifndef WINDOWITEMPROPERTIES_H
#define WINDOWITEMPROPERTIES_H

#include "GlobalIncludes.h"

#include "ItemProperties.h"
#include "DialogParametersConfig.h"

////////////////////////////////////////////////////////////

class WindowItemProperties : public QFrame
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit WindowItemProperties(QWidget* parent = 0);
    ~WindowItemProperties();

    void SetGraphicItemProperties(ItemProperties* Properties);
    void ClearProperiesWindow();
    void RefreshProperiesWindowProperty(QString PropertyName, QString NewPropertyValue);

private:
    void AddPropertyData(QString Property, QString Value, QString Desc, bool ReadOnly = false);

public slots:
    void HandleCellChanged(int, int);
    void HandleItemDoubleClicked(QTableWidgetItem* Item);

signals:

private:
    QGroupBox*              m_PropertiesGroupBox;
    QTableWidget*           m_PropertiesTable;

    ItemProperties*         m_CurrentProperties;
    bool                    m_PopulatingWindow;
    DialogParametersConfig* m_ConfigureDynamicParameter;
};

#endif // WINDOWITEMPROPERTIES_H

