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
#include "DialogDynamicPropertiesConfig.h"
#include "DialogAddEditProperty.h"

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
    void RefreshAllProperties();
    void RefreshProperiesWindowProperty(QString PropertyName, QString NewPropertyValue);

private:
    void AddPropertyData(ItemProperty* Property);
    QString GetPropertyName(int row);
    QString GetPropertyValue(int row);
    QString GetPropertyDesc(int row);
    bool IsPropertyReadOnly(int row);
    bool IsPropertyProtected(int row);

public slots:
    void HandleCellChanged(int, int);
    void HandleSelectionChanged();
    void HandleItemDoubleClicked(QTableWidgetItem* Item);
    void HandleBtnAddPropertyClicked();
    void HandleBtnRemovePropertyClicked();
    void HandleBtnEditPropertyClicked();

signals:

private:
    QGroupBox*                     m_PropertiesGroupBox;
    QTableWidget*                  m_PropertiesTable;
    QToolButton*                   m_BtnAddProperty;
    QToolButton*                   m_BtnRemoveProperty;
    QToolButton*                   m_BtnEditProperty;

    ItemProperties*                m_CurrentProperties;
    bool                           m_PopulatingWindow;
    DialogDynamicPropertiesConfig* m_ConfigureDynamicProperties;
    DialogAddEditProperty*         m_AddEditProperty;
};

#endif // WINDOWITEMPROPERTIES_H

