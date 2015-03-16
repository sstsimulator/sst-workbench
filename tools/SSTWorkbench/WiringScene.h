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

#ifndef WIRINGSCENE_H
#define WIRINGSCENE_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"
#include "GraphicItemComponent.h"
#include "GraphicItemWire.h"
#include "GraphicItemText.h"
#include "SSTInfoData.h"
#include "UndoRedoCommands.h"

/////////////////////////////////////////////////////////////////////
// In the WiringScene, An Item is either a Component, Wire or Text
/////////////////////////////////////////////////////////////////////

class WiringScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum OperationMode {MODE_DONOTHING, MODE_SELECTMOVEITEM, MODE_MOVEWIREHANDLE, MODE_ADDCOMPONENT, MODE_ADDWIRE, MODE_ADDTEXT};

    // Constructor / Destructor
    explicit WiringScene(QMenu* ItemMenu, QUndoStack* UndoStack, QObject* parent = 0);
    ~WiringScene();

    // Set / Get the Operation Mode and What Component user has currently choosen
    void SetOperationMode(OperationMode NewMode);
    void SetUserChosenSSTInfoDataComponent(SSTInfoDataComponent* ComponentInfo);
    OperationMode GetOperationMode() {return m_CurrentOperationMode;}
    SSTInfoDataComponent* GetUserChosenSSTInfoDataComponent() {return m_UserChosenSSTInfoDataComponent;}

    // Set / Get of Color and Font Info
    void SetGraphicItemComponentFillColor(const QColor& color);
    void SetGraphicItemTextColor(const QColor& color);
    void SetGraphicItemTextFont(const QFont& font);
    QColor GetGraphicItemComponentFillColor() const { return m_ItemComponentFillColor; }
    QColor GetItemTextColor() const { return m_ItemTextColor; }
    QFont  GetItemTextFont() const { return m_ItemTextFont; }

    // Called by MainWindow when a component is being deleted from the scene
    void RemoveComponentFromComponentTypeList(GraphicItemComponent* Component);
    QString BuildComponentKey(QString ElementName, QString ComponentName);

    // Selections
    void SetNothingSelected();

    // Refresh Wires
    void RefreshAllCurrentWirePositions();

    // Create New Graphic Item Component
    QString CheckComponentReqsAndBuildKey(int AllowedInstances, SSTInfoDataComponent::ComponentType CompType, QString ParentElementName, QString ComponentName);
    void CreateNewComponentItem(QPointF ScenePos);
    void CreateNewComponentItem(QDataStream& DataStreamIn);  // From Serialization
    void PasteNewComponentItem(QDataStream& DataStreamIn, int PasteOffset);   // From Copy/Paste
    void AddNewComponentItemToScene(GraphicItemComponent* NewComponentItem, bool SelectSingle = true);

    // Create New Graphic Item Text
    void CreateNewTextItem(QPointF ScenePos);
    void CreateNewTextItem(QDataStream& DataStreamIn);  // From Serialization
    void PasteNewTextItem(QDataStream& DataStreamIn, int PasteOffset);   // From Copy/Paste
    void AddNewTextItemToScene(GraphicItemText* NewTextItem, bool SelectSingle = true);

    // Create New Graphic Item Wire
    void CreateNewWireItem(QPointF ScenePos);
    void CreateNewWireItem(QDataStream& DataStreamIn);  // From Serialization
    void PasteNewWireItem(QDataStream& DataStreamIn, int PasteOffset);   // From Copy/Paste
    void AddNewWireItemToScene(GraphicItemWire* NewWireItem, bool UpdateBothPoints);

    // Delete Handling
    void DeleteAllSelectedItems();
    void DeleteWireFromScene(GraphicItemWire* ptrParentWire);
    void DeleteComponentFromScene(GraphicItemComponent* ptrComponent);
    void DeleteTextFromScene(GraphicItemText* ptrText);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);
    void LoadData(QDataStream& DataStreamIn);

signals:
    // Signals to notify Main Window of Events that happen
    void SceneEventComponentAdded(GraphicItemComponent* Item);
    void SceneEventTextAdded(GraphicItemText* Item);
    void SceneEventWireAddedInitialPlacement(GraphicItemWire* Item);
    void SceneEventWireAddedFinalPlacement(GraphicItemWire* Item);
    void SceneEventGraphicItemSelected(QGraphicsItem* Item);
    void SceneEventGraphicItemSelectedProperties(ItemProperties* Properties);
    void SceneEventGraphicItemMoved(GraphicItemBase* Item);
    void SceneEventSetProjectDirty();
    void SceneEventRefreshPropertiesWindowProperty(QString, QString);
    void SceneEventRefreshPropertiesWindow(ItemProperties*);
    void SceneEventDragAndDropFinished();

private:
    // Mouse Handling Events
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

    // Drag & Drop Events
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);

private:
    // Set a Graphic Item Selected
    void SetSingleGraphicItemAsSelected(QGraphicsItem* NewItem);

private slots:
    void HandleTextEditorLostFocus(GraphicItemText* item);

private:
    // General vars
    QMenu*                                m_ItemMenu;
    QUndoStack*                           m_UndoStack;
    OperationMode                         m_CurrentOperationMode;            // Mode that the Scene is current in
    SSTInfoDataComponent*                 m_UserChosenSSTInfoDataComponent;  // What component the user is going to add

    // Colors and fonts
    QColor                                m_ItemComponentFillColor;
    QColor                                m_ItemTextColor;
    QFont                                 m_ItemTextFont;

    // Handling of wires
    GraphicItemWire*                      m_ptrNewItemWire;
    GraphicItemWire*                      m_ptrMovingItemWire;
    GraphicItemWireHandle*                m_ptrMovingItemWireHandle;

    // Lists to track Graphic items
    QList<GraphicItemWire*>               m_GraphicItemWireList;          // List of all GraphicItemWires
    QList<GraphicItemComponent*>          m_GraphicItemComponentList;     // List of all GraphicItemComponents
    QList<GraphicItemComponent*>          m_GraphicItemComponentByTypeList[NUMCOMPONENTTYPES]; // List of GraphicItemComponents by ComponentType

    // Index of Wires and Components
    int                                   m_CurrentWireIndex;           // Current Index of Wires
    QMap<QString, int>                    m_CurrentComponentByKeyIndex; // Map of Current Indexs of Components by KeyValue <element>.<componentname>

};

#endif // WIRINGSCENE_H

