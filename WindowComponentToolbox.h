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

#ifndef WINDOWCOMPONENTTOOLBOX_H
#define WINDOWCOMPONENTTOOLBOX_H

#include "GlobalIncludes.h"

#include "GraphicItemComponent.h"

//////////////////////////////////////////////////////////////

class ComponentToolButton : public QToolButton
{
    Q_OBJECT
public:
    // Constructor / Destructor
    explicit ComponentToolButton(SSTInfoDataComponent::ComponentType ComponentType, QWidget* parent = 0);

private:
    // Drag & Drop Handling
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    QPointF                             m_DragStartPosition;
    SSTInfoDataComponent::ComponentType m_ComponentType;
};

////////////////////////////////////////////////////////////

class ComponentToolBox : public QToolBox
{
    Q_OBJECT
public:
    // Constructor / Destructor
    explicit ComponentToolBox(QWidget* parent = 0);

signals:
    void ComponentToolboxResized();

private:
    // Resize Handling
    void resizeEvent(QResizeEvent* event);
};


////////////////////////////////////////////////////////////

class WindowComponentToolBox : public QFrame
{
    Q_OBJECT

private:
    enum SortType {SORTBY_ELEMENT, SORTBY_TYPE};

public:
    // Constructor / Destructor
    explicit WindowComponentToolBox(QWidget* parent = 0);
    ~WindowComponentToolBox();

    // Uncheck All ToolBox buttons in the current group
    void UncheckAllCurrentGroupButtons();

    // Load SSTInfo data into Toolbox
    SSTInfoData* GetSSTInfoData() {return m_SSTInfoData;}
    void LoadSSTInfo(SSTInfoData* SSTInfoData, bool AddSSTConfigComponent);

signals:
    void ComponentToolboxButtonPressed(SSTInfoDataComponent* ptrComponent);

private:
    // Clear the ToolBox
    void ClearToolBox();

    // Add the SST Configuration Component to the SSTInfo Structures
    void Add_SSTConfigComponentToSSTInfo();

    QButtonGroup* GetCurrentButtonGroup(){return m_CurrentButtonGroup;}

    QWidget* CreateItemComponentButtonWidget(QButtonGroup* ButtonGroup, const QString& text, SSTInfoDataComponent* Component);
    QWidget* CreateButtonGroupWidgetByElement(SSTInfoDataElement* ElementData);
    QWidget* CreateButtonGroupWidgetByComponentType(SSTInfoDataComponent::ComponentType Type);

    void PopulateToolBox();

private slots:
    void HandleComponentToolboxButtonGroupPressed(int id);
    void HandleComponentToolboxButtonGroupToggled(int id, bool checked);
    void HandleChangedButtonGroup(int index);
    void HandleSortByButtonClicked(bool checked);
    void HandleComponentToolboxResized();

private:
    SSTInfoData*         m_SSTInfoData;
    QButtonGroup*        m_CurrentButtonGroup;
    QList<QButtonGroup*> m_ButtonGroupList;

    QGroupBox*           m_SortByGroupBox;
    ComponentToolBox*    m_ToolboxWidget;

    QRadioButton*        m_RadioSortByType;
    QRadioButton*        m_RadioSortByElement;

    SortType             m_SortByFlag;
};

#endif // WINDOWCOMPONENTTOOLBOX_H
