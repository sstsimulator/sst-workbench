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

#ifndef WINDOWCOMPONENTTOOLBOX_H
#define WINDOWCOMPONENTTOOLBOX_H

#include "GlobalIncludes.h"

#include "GraphicItemComponent.h"

//////////////////////////////////////////////////////////////

class ComponentTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    // Constructor / Destructor
    explicit ComponentTreeWidget(QWidget* parent = 0);

private:
    // Drag & Drop Handling
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QPointF               m_DragStartPosition;
    SSTInfoDataComponent* m_DraggingComponent;
    ComponentType_enum    m_DraggingComponentType;
    QTreeWidgetItem*      m_DraggingItem;
};

////////////////////////////////////////////////////////////

class WindowComponentToolBox : public QFrame
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit WindowComponentToolBox(QWidget* parent = 0);
    ~WindowComponentToolBox();

    // Uncheck All ToolBox buttons in the current group
    void UnselectAllToolboxComponentItems();

    // Load SSTInfo data into Toolbox
    void LoadSSTInfo(SSTInfoData* SSTInfoData, bool AddSSTConfigComponent);

    // Info on the SSTInfo
    SSTInfoData* GetSSTInfoData() {return m_SSTInfoData;}
    bool IsSSTInfoDataLoaded() {return (m_SSTInfoData != NULL);}

signals:
    void ComponentToolboxItemPressed(SSTInfoDataComponent* ptrComponent);

private:
    // Clear the ToolBox
    void ClearToolBox();

    // Add the SST Configuration Component to the SSTInfo Structures
    void Add_SSTConfigComponentToSSTInfo();

    void PopulateToolBox();

private slots:
    void HandleSortByButtonClicked(bool checked);
    void HandleComponentTreeItemPressed(QTreeWidgetItem* item, int column);
    void HandleComponentTreeItemCollapsed(QTreeWidgetItem* item);

private:
    SSTInfoData*         m_SSTInfoData;
    QGroupBox*           m_SortByGroupBox;

    ComponentTreeWidget* m_ToolboxTreeWidget;

    QRadioButton*        m_RadioSortByType;
    QRadioButton*        m_RadioSortByElement;

    SortType_enum        m_SortByFlag;
};

#endif // WINDOWCOMPONENTTOOLBOX_H
