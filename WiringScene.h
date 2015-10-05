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
    // Constructor / Destructor
    explicit WiringScene(QMenu* ItemMenu, QUndoStack* UndoStack, QObject* parent = 0);
    ~WiringScene();

    // Set / Get the Operation Mode and What Component user has currently choosen
    void SetOperationMode(SceneOperationMode_enum NewMode);
    void SetUserChosenSSTInfoDataComponent(SSTInfoDataComponent* ComponentInfo);
    SceneOperationMode_enum GetOperationMode() {return m_CurrentOperationMode;}
    SSTInfoDataComponent* GetUserChosenSSTInfoDataComponent() {return m_UserChosenSSTInfoDataComponent;}

    // Set / Get of Component and Text Items Font and Color Info
    void SetGraphicItemComponentFillColor(const QColor& color);
    void SetGraphicItemTextColor(const QColor& color);
    void SetGraphicItemTextFont(const QFont& font);
    QColor GetGraphicItemComponentFillColor() const { return m_ItemComponentFillColor; }
    QColor GetItemTextColor() const { return m_ItemTextColor; }
    QFont  GetItemTextFont() const { return m_ItemTextFont; }

    // Selection Handling and Information
    void                  SetNothingSelected();
    void                  SelectAllGraphicalItems();
    bool                  IsSceneEmpty();
    int                   GetNumSelectedGraphicalItems();
    QGraphicsItem*        GetFirstSelectedGraphicalItem();
    QList<QGraphicsItem*> GetAllSelectedGraphicalItems();
    QRectF                GetAllGraphicalItemsBoundingRect();

    // Refresh Wires
    void RefreshAllCurrentWirePositions();

    // Create New Graphic Items
    void CreateNewComponentItem(QPointF ScenePos);
    void CreateNewComponentItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // From Serialization
    void PasteNewComponentItem(QDataStream& DataStreamIn, int PasteOffset);             // From Copy/Paste
    void AddNewComponentItemToScene(GraphicItemComponent* NewComponentItem, bool SelectSingle = true);

    void CreateNewTextItem(QPointF ScenePos);
    void CreateNewTextItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion);       // From Serialization
    void PasteNewTextItem(QDataStream& DataStreamIn, int PasteOffset);                  // From Copy/Paste
    void AddNewTextItemToScene(GraphicItemText* NewTextItem, bool SelectSingle = true);

    void CreateNewWireItem(QPointF ScenePos);
    void CreateNewWireItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion);       // From Serialization
    void PasteNewWireItem(QDataStream& DataStreamIn, int PasteOffset);                  // From Copy/Paste
    void AddNewWireItemToScene(GraphicItemWire* NewWireItem, bool UpdateBothPoints);

    // Delete Handling
    void DeleteAllSelectedItems();
    void DeleteWireFromScene(GraphicItemWire* ptrParentWire);
    void DeleteComponentFromScene(GraphicItemComponent* ptrComponent);
    void DeleteTextFromScene(GraphicItemText* ptrText);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);
    void LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion);

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

    // Scene Virtual Functions
    void drawBackground(QPainter* painter, const QRectF& rect);

private slots:
    void HandleTextEditorLostFocus(GraphicItemText* item);
    void HandlePreferenceChangeAutoDeleteTooShortWire(bool NewValue);

private:
    // Routines for checking Selections
    void CheckGraphicItemsSelectedAtPoint(QGraphicsSceneMouseEvent* mouseEvent);
    void CheckForWireHandlesSelectedAtPoint(QGraphicsSceneMouseEvent* mouseEvent);
    void CheckGroupGraphicItemsSelected(QList<QGraphicsItem*>& SelectedItems);

    // RubberBand Selection
    void CheckForStartOfRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent);
    void ProcessMoveOfRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent);
    void FinishRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent);

    // Set Graphic Items Selected
    void SetSingleGraphicItemAsSelected(QGraphicsItem* NewItem);

    // Handling of Component Key Index
    QString CheckComponentReqsAndBuildKey(int AllowedInstances, ComponentType_enum CompType, QString ParentElementName, QString ComponentName);
    QString BuildComponentKey(QString ElementName, QString ComponentName);

private:
    // General vars
    QMenu*                                m_ItemMenu;
    QUndoStack*                           m_UndoStack;
    SceneOperationMode_enum               m_CurrentOperationMode;            // Mode that the Scene is current in
    SSTInfoDataComponent*                 m_UserChosenSSTInfoDataComponent;  // What component the user is going to add

    // Colors and fonts
    QColor                                m_ItemComponentFillColor;
    QColor                                m_ItemTextColor;
    QFont                                 m_ItemTextFont;

    // Handling of wires
    GraphicItemWire*                      m_ptrNewItemWire;
    GraphicItemWire*                      m_ptrMovingItemWire;
    GraphicItemWireHandle*                m_ptrMovingItemWireHandle;
    QGraphicsRectItem*                    m_PtrRubberBandSelect;
    qreal                                 m_RubberBandStartX;
    qreal                                 m_RubberBandStartY;

    // Misc
    bool                                   m_PrefAutoDeleteTooShortWires;
};

#endif // WIRINGSCENE_H

