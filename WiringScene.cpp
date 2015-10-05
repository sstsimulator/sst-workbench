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

#include "WiringScene.h"

class MainWindow;

////////////////////////////////////////////////////////////

WiringScene::WiringScene(QMenu* ItemMenu, QUndoStack* UndoStack, QObject* parent /*=0*/) :
    QGraphicsScene(parent)
{
    // Get a copy of the ItemMenu and the UndoStack
    m_ItemMenu = ItemMenu;
    m_UndoStack = UndoStack;

    // Set the Default Operation Mode
    SetOperationMode(SCENEMODE_SELECTMOVEITEM);

    // Set the Default Selected Item Component Type
    m_UserChosenSSTInfoDataComponent = NULL;

    // Set Item Colors
    SetGraphicItemComponentFillColor(Qt::white);
    SetGraphicItemTextColor(Qt::black);
    SetGraphicItemTextFont(QFont());

    // Init some variables
    m_ptrNewItemWire = NULL;
    m_ptrMovingItemWire = NULL;
    m_ptrMovingItemWireHandle = NULL;
    m_PtrRubberBandSelect = NULL;
    m_PrefAutoDeleteTooShortWires = true;
}

WiringScene::~WiringScene()
{
}

void WiringScene::SetOperationMode(SceneOperationMode_enum NewMode)
{
    m_CurrentOperationMode = NewMode;
}

void WiringScene::SetUserChosenSSTInfoDataComponent(SSTInfoDataComponent* ComponentInfo)
{
    m_UserChosenSSTInfoDataComponent = ComponentInfo;
}

void WiringScene::SetGraphicItemComponentFillColor(const QColor& color)
{
    m_ItemComponentFillColor = color;

    // Change all Selected items of the GraphicItemComponent type
    foreach (QGraphicsItem* SelectedItem, selectedItems()) {
        if (SelectedItem->type() == GraphicItemComponent::Type) {
            GraphicItemComponent* ItemComponent = qgraphicsitem_cast<GraphicItemComponent*>(SelectedItem);
            if (ItemComponent != NULL) {
                ItemComponent->SetComponentFillColor(m_ItemComponentFillColor);
                emit SceneEventSetProjectDirty();
            }
        }
    }
}

void WiringScene::SetGraphicItemTextColor(const QColor& color)
{
    // Set the default color for any future text items created
    m_ItemTextColor = color;

    // Change all Selected items of the GraphicItemText type
    foreach (QGraphicsItem* SelectedItem, selectedItems()) {
        if (SelectedItem->type() == GraphicItemText::Type) {
            GraphicItemText* ItemText = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
            if (ItemText != NULL) {
                ItemText->setDefaultTextColor(m_ItemTextColor);
                emit SceneEventSetProjectDirty();
            }
        }
    }
}

void WiringScene::SetGraphicItemTextFont(const QFont& font)
{
    // Set the default font for any future text items created
    m_ItemTextFont = font;

    // Change all Selected items of the GraphicItemText type
    foreach (QGraphicsItem* SelectedItem, selectedItems()) {
        if (SelectedItem->type() == GraphicItemText::Type) {
            GraphicItemText* TextItem = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
            if (TextItem != NULL) {
                TextItem->setFont(m_ItemTextFont);
                emit SceneEventSetProjectDirty();
            }
        }
    }
}

void WiringScene::SetNothingSelected()
{
    // Get the List of all graphic Items, and set none of them selected
    QList<QGraphicsItem*> GraphicItems = items();
    foreach (QGraphicsItem* item, GraphicItems) {
        item->setSelected(false);
    }

    // Turn off the properties
    emit SceneEventGraphicItemSelected(NULL);
    emit SceneEventGraphicItemSelectedProperties(NULL);
}

void WiringScene::SelectAllGraphicalItems()
{
    // Set all the Graphic items to selected
    foreach (QGraphicsItem* item, items(Qt::DescendingOrder)) {
        item->setSelected(true);
    }
}

bool WiringScene::IsSceneEmpty()
{
    // Return if the list of all Graphical Items is empty
    return items().isEmpty();
}

int WiringScene::GetNumSelectedGraphicalItems()
{
    // Return the number of selected Graphical Items
    return selectedItems().count();
}

QGraphicsItem*  WiringScene::GetFirstSelectedGraphicalItem()
{
    // Return the first item in the list of selected graphical items
    return selectedItems().first();
}

QList<QGraphicsItem*> WiringScene::GetAllSelectedGraphicalItems()
{
    // Return a list of all Graphical items that are selected
    return selectedItems();
}

QRectF WiringScene::GetAllGraphicalItemsBoundingRect()
{
    // Return the bounding rect for all items in the scene
    return itemsBoundingRect();
}

void WiringScene::RefreshAllCurrentWirePositions()
{
    GraphicItemWire* Wire;

    // Get a list of all graphic Items
    QList<QGraphicsItem*> GraphicItems = items();

    // Iterate through all items and Refresh the positions of all wires
    foreach (QGraphicsItem* Item, GraphicItems) {
        // Cast to a pointer, if the cast is invalid, the pointer will be NULL
        Wire = qgraphicsitem_cast<GraphicItemWire*>(Item);

        if (Wire != NULL) {
            // We have a wire, now update each of its points
            Wire->UpdateStartPointPosition(Wire->GetStartPoint());
            Wire->UpdateEndPointPosition(Wire->GetEndPoint());
        }
    }
}

void WiringScene::CreateNewComponentItem(QPointF ScenePos)
{
    QString            Key;
    int                IndexValue;
    int                AllowedInstances;
    ComponentType_enum CompType;
    QString            ParentElementName;
    QString            ComponentName;
    QPointF            FinalScenePos;

    AllowedInstances = m_UserChosenSSTInfoDataComponent->GetAllowedNumberOfInstances();
    CompType = m_UserChosenSSTInfoDataComponent->GetComponentType();
    ParentElementName = m_UserChosenSSTInfoDataComponent->GetParentElementName();
    ComponentName = m_UserChosenSSTInfoDataComponent->GetComponentName();

    // Check to see if we need to snap to grid
    FinalScenePos = SnapToGrid::CheckSnapToGrid(ScenePos);

    Key = CheckComponentReqsAndBuildKey(AllowedInstances, CompType, ParentElementName, ComponentName);
    if (Key.isEmpty() == false) {
        // Get the Index Value based on key <element>.<component> and increment it.
        IndexValue = GraphicItemData::GetNextComponentIndexByKey(Key);

        // Create a new Component at the mouse's location
        GraphicItemComponent* NewComponent;
        NewComponent = new GraphicItemComponent(IndexValue, m_UserChosenSSTInfoDataComponent, m_ItemMenu, m_ItemComponentFillColor, FinalScenePos);

        // Add this ADD Command to the Undo/Redo Queue
        // NOTE: QUndoCommand::redo() is called when pushing the command
        //       onto the stack, which will call AddNewComponentItemToScene();
        QUndoCommand* AddComponentCommand = new ComandAddGraphicItemComponent(NewComponent, this);
        m_UndoStack->push(AddComponentCommand);
    }
}

void WiringScene::CreateNewComponentItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    // Create a new Component Item from the Saved Data
    GraphicItemComponent* NewComponent;
    NewComponent = new GraphicItemComponent(DataStreamIn, ProjectFileVersion, m_ItemMenu);

    // Add the item to the scene
    AddNewComponentItemToScene(NewComponent);
}

void WiringScene::PasteNewComponentItem(QDataStream& DataStreamIn, int PasteOffset)
{
    QString            Key;
    int                IndexValue;
    int                AllowedInstances;
    QString            ParentElementName;
    QString            ComponentName;
    ComponentType_enum CompType;

    // Create a new Component Item from the Paste Action (always use latest project file version)
    GraphicItemComponent* NewComponent;
    NewComponent = new GraphicItemComponent(DataStreamIn, SSTWORKBENCHPROJECTFILECURRFORMATVER, m_ItemMenu);

    AllowedInstances = NewComponent->GetNumAllowedInstances();
    CompType = NewComponent->GetComponentType();
    ParentElementName = NewComponent->GetParentElementName();
    ComponentName = NewComponent->GetComponentName();

    Key = CheckComponentReqsAndBuildKey(AllowedInstances, CompType, ParentElementName, ComponentName);
    if (Key.isEmpty() == false) {
        // Get the Index Value based on key <element>.<component> and increment it.
        IndexValue = GraphicItemData::GetNextComponentIndexByKey(Key);

        // Set the Index for the New Component
        NewComponent->SetComponentIndex(IndexValue);

        // Add this PASTE Command to the Undo/Redo Queue
        // NOTE: QUndoCommand::redo() is called when pushing the command
        //       onto the stack, which will call AddNewComponentItemToScene();
        QUndoCommand* AddComponentCommand = new ComandAddGraphicItemComponent(NewComponent, this, true);
        m_UndoStack->push(AddComponentCommand);

        // Offset the new Component's Position so it shows up near its original
        QPointF pos = NewComponent->pos();
        pos += QPointF(PasteOffset, PasteOffset);
        NewComponent->setPos(pos);

        // Set the Pasted object to selected so that its ready to move
        NewComponent->setSelected(true);
    }
}

void WiringScene::AddNewComponentItemToScene(GraphicItemComponent* NewComponentItem, bool SelectSingle /*=true*/)
{
    ComponentType_enum CompType = NewComponentItem->GetComponentType();

    // Add the item to the scene
    addItem(NewComponentItem);

    // Increment the count of all components and count of components by type
    GraphicItemData::AddComponentToGraphicItemComponentList(NewComponentItem);
    GraphicItemData::AddComponentToGraphicItemComponentByTypeList(NewComponentItem, CompType);

    if (SelectSingle == true) {
        // Set the Component to as the only selected item
        SetSingleGraphicItemAsSelected(NewComponentItem);
    }

    // Display the properties for this Wire
    emit SceneEventGraphicItemSelected(NewComponentItem);
    emit SceneEventGraphicItemSelectedProperties(NewComponentItem->GetItemProperties());

    connect(NewComponentItem, SIGNAL(ItemComponentSetProjectDirty()), this, SIGNAL(SceneEventSetProjectDirty()));
    connect(NewComponentItem, SIGNAL(ItemComponentRefreshPropertiesWindowProperty(QString, QString)), this, SIGNAL(SceneEventRefreshPropertiesWindowProperty(QString, QString)));
    connect(NewComponentItem, SIGNAL(ItemComponentRefreshPropertiesWindow(ItemProperties*)), this, SIGNAL(SceneEventRefreshPropertiesWindow(ItemProperties*)));

    // Tell the Main Window we added this Component
    emit SceneEventComponentAdded(NewComponentItem);
}

void WiringScene::CreateNewTextItem(QPointF ScenePos)
{
    // Create a new Text Object at the mouse's location
    GraphicItemText* NewTextItem;
    NewTextItem = new GraphicItemText(m_ItemTextFont, m_ItemTextColor, ScenePos);

    // Add this ADD Command to the Undo/Redo Queue
    // NOTE: QUndoCommand::redo() is called when pushing the command
    //       onto the stack, which will call AddNewTextItemToScene();
    QUndoCommand* AddTextCommand = new ComandAddGraphicItemText(NewTextItem, this);
    m_UndoStack->push(AddTextCommand);
}

void WiringScene::CreateNewTextItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    // Create a new Text item from the saved data
    GraphicItemText* NewTextItem;
    NewTextItem = new GraphicItemText(DataStreamIn, ProjectFileVersion);

    // Add the item to the scene
    AddNewTextItemToScene(NewTextItem);
}

void WiringScene::PasteNewTextItem(QDataStream& DataStreamIn, int PasteOffset)
{
    // Create a new Text item from the Paste Action (always use latest project file version)
    GraphicItemText* NewTextItem;
    NewTextItem = new GraphicItemText(DataStreamIn, SSTWORKBENCHPROJECTFILECURRFORMATVER);

    // Add this Paste Command to the Undo/Redo Queue
    // NOTE: QUndoCommand::redo() is called when pushing the command
    //       onto the stack, which will call AddNewTextItemToScene();
    QUndoCommand* AddTextCommand = new ComandAddGraphicItemText(NewTextItem, this, true);
    m_UndoStack->push(AddTextCommand);

    // Offset the new Text's Position so it shows up near its original
    QPointF pos = NewTextItem->pos();
    pos += QPointF(PasteOffset, PasteOffset);
    NewTextItem->setPos(pos);

    // Set the Pasted object to selected so that its ready to move
    NewTextItem->setSelected(true);
}

void WiringScene::AddNewTextItemToScene(GraphicItemText* NewTextItem, bool SelectSingle /*=true*/)
{
    // Add the item to the scene
    addItem(NewTextItem);

    if (SelectSingle == true) {
        // Set the Text to as the only selected item
        SetSingleGraphicItemAsSelected(NewTextItem);
    }

    // Turn off the properties display (for text)
    emit SceneEventGraphicItemSelected(NULL);
    emit SceneEventGraphicItemSelectedProperties(NULL);

    // Signal a handler when Text Editor Looses Focus
    connect(NewTextItem, SIGNAL(ItemTextLostFocus(GraphicItemText*)), this, SLOT(HandleTextEditorLostFocus(GraphicItemText*)));
    connect(NewTextItem, SIGNAL(ItemTextSetProjectDirty()), this, SIGNAL(SceneEventSetProjectDirty()));

    // Tell the Main Window we added this Text
    emit SceneEventTextAdded(NewTextItem);
}

void WiringScene::CreateNewWireItem(QPointF ScenePos)
{
    QPointF FinalScenePos;

    // Check to see if we need to snap to grid
    FinalScenePos = SnapToGrid::CheckSnapToGrid(ScenePos);

    // Create a New ItemWire graphic object to show the user how they are wiring,
    // both points initially start at the mouse position.
    m_ptrNewItemWire = new GraphicItemWire(GraphicItemData::GetNextWireIndex(), FinalScenePos, FinalScenePos);

    // Set the Wire to as the only selected item
    // Also set the Wire to selected so it shows its colors correctly
    SetSingleGraphicItemAsSelected(m_ptrNewItemWire);
    m_ptrNewItemWire->SetWireSelected(true);

    // Display the properties for this Wire
    emit SceneEventGraphicItemSelected(m_ptrNewItemWire);
    emit SceneEventGraphicItemSelectedProperties(m_ptrNewItemWire->GetItemProperties());

    // Add this ADD Command to the Undo/Redo Queue
    // NOTE: QUndoCommand::redo() is called when pushing the command
    //       onto the stack, which will call AddNewWireItemToScene();
    QUndoCommand* AddWireCommand = new ComandAddGraphicItemWire(m_ptrNewItemWire, this);
    m_UndoStack->push(AddWireCommand);
}

void WiringScene::CreateNewWireItem(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    // Create a New ItemWire graphic object to show the user how they are wiring,
    // both points initially start at the mouse position.
    m_ptrNewItemWire = new GraphicItemWire(DataStreamIn, ProjectFileVersion);

    // Add the item to the scene
    AddNewWireItemToScene(m_ptrNewItemWire, true);

    // Mark the New Item Wire as NULL so it is not seen as being moved
    m_ptrNewItemWire = NULL;
}

void WiringScene::PasteNewWireItem(QDataStream& DataStreamIn, int PasteOffset)
{
    // Create a New ItemWire graphic object to show the user how they are wiring,
    // both points initially start at the mouse position.
    m_ptrNewItemWire = new GraphicItemWire(DataStreamIn, SSTWORKBENCHPROJECTFILECURRFORMATVER, GraphicItemData::GetNextWireIndex());

    // Add this ADD Command to the Undo/Redo Queue
    // NOTE: QUndoCommand::redo() is called when pushing the command
    //       onto the stack, which will call AddNewWireItemToScene();
    QUndoCommand* AddWireCommand = new ComandAddGraphicItemWire(m_ptrNewItemWire, this, true);
    m_UndoStack->push(AddWireCommand);

    // Move the Objects Position
    m_ptrNewItemWire->SetPastePosition(PasteOffset);

    // Set the Pasted object to selected so that its ready to move
    m_ptrNewItemWire->setSelected(true);

    // Mark the New Item Wire as NULL so it is not seen as being moved
    m_ptrNewItemWire = NULL;
}

void WiringScene::AddNewWireItemToScene(GraphicItemWire* NewWireItem, bool UpdateBothPoints)
{
    // Add the item to the scene, and to the list
    addItem(NewWireItem);
    GraphicItemData::AddWireToGraphicItemWireList(NewWireItem);

    // Tell the Main Window we Initially added this Wire (The wire has been created and start point set)
    emit SceneEventWireAddedInitialPlacement(NewWireItem);

    connect(NewWireItem, SIGNAL(ItemWireSetProjectDirty()), this, SIGNAL(SceneEventSetProjectDirty()));

    // Now Update the positions of the wires (always the Start Point and sometimes the End Point)
    NewWireItem->UpdateStartPointPosition(NewWireItem->GetStartPoint());
    if (UpdateBothPoints == true) {
        NewWireItem->UpdateEndPointPosition(NewWireItem->GetEndPoint());
    }
}

void WiringScene::DeleteAllSelectedItems()
{
    // Add this DELETE Command to the Undo/Redo Queue
    // NOTE: QUndoCommand::redo() is called when pushing the command
    //       onto the stack Which will call DeleteWireFromScene(),
    //       DeleteComponentFromScene() & DeleteTextFromScene() as many
    //       times as necessary.
    QUndoCommand* DeleteGraphicItemsCommand = new ComandDeleteGraphicItems(this);
    m_UndoStack->push(DeleteGraphicItemsCommand);
}

void WiringScene::DeleteWireFromScene(GraphicItemWire* ptrParentWire)
{
    // Disconnect any Connected ports
    ptrParentWire->DisconnectAllAttachedPorts();

    // Delete the Parent Wire
    removeItem(ptrParentWire);

    // Remove the Wire from the WireList
    GraphicItemData::RemoveWireFromGraphicItemWireList(ptrParentWire);

    // NOTE: Even if no wires exist, we do NOT reset the m_CurrentWireIndex as this could
    //       cause a mismatch when a undo command is executed (it was discovered bug)

    // NOTE: OBJECT IS CONTAINED IN THE ADD COMMAND and will be deleted there upon destruction

    // De-Select all Items & Turn off the properties display
    emit SceneEventGraphicItemSelected(NULL);
    emit SceneEventGraphicItemSelectedProperties(NULL);
}

void WiringScene::DeleteComponentFromScene(GraphicItemComponent* ptrComponent)
{
    QString Key;
    QString SearchKey;
    int     MatchingComponentKeys = 0;

    // Get the Component Key
    Key = BuildComponentKey(ptrComponent->GetParentElementName(), ptrComponent->GetComponentName());

    // Disconnect any Wires from the Component
    ptrComponent->DisconnectPortsFromAllWires();

    // Delete Component
    removeItem(ptrComponent);

    // Remove the Component from the ComponentList & ComponentTypeList
    GraphicItemData::RemoveComponentFromGraphicItemComponentByTypeList(ptrComponent, ptrComponent->GetComponentType());
    GraphicItemData::RemoveComponentFromGraphicItemComponentList(ptrComponent);

    // Check to see if there are any Components left That matches the Key
    foreach (GraphicItemComponent* Item, GraphicItemData::GetGraphicItemComponentList()) {
        SearchKey = BuildComponentKey(Item->GetParentElementName(), Item->GetComponentName());
        if (Key == SearchKey) {
            MatchingComponentKeys++;
        }
    }

    // NOTE: Even if no wires exist, we do NOT reset the m_CurrentComponentByKeyIndex as this could
    //       cause a mismatch when a undo command is executed (it was discovered bug)
    // NOTE: OBJECT IS CONTAINED IN THE ADD COMMAND and will be deleted there upon destruction

    // De-Select all Items & Turn off the properties display
    emit SceneEventGraphicItemSelected(NULL);
    emit SceneEventGraphicItemSelectedProperties(NULL);
}

void WiringScene::DeleteTextFromScene(GraphicItemText* ptrText)
{
    // Delete Text Item
    removeItem(ptrText);

    // NOTE: OBJECT IS CONTAINED IN THE ADD COMMAND and will be deleted there upon destruction

    // De-Select all Items & Turn off the properties display
    emit SceneEventGraphicItemSelected(NULL);
    emit SceneEventGraphicItemSelectedProperties(NULL);
}

void WiringScene::SaveData(QDataStream& DataStreamOut)
{
    GraphicItemText*       Text;
    GraphicItemComponent*  Component;
    GraphicItemWire*       Wire;
    qint32                 TextCount = 0;
    qint32                 ComponentCount = 0;
    qint32                 WireCount = 0;

    // Get a list of all graphic Items
    QList<QGraphicsItem*> GraphicItems = items();

    // Iterate through all items get the counts of each top level object
    foreach (QGraphicsItem* Item, GraphicItems) {
        // Cast to a pointer, if the cast is invalid, the pointer will be NULL
        Component = qgraphicsitem_cast<GraphicItemComponent*>(Item);
        Text = qgraphicsitem_cast<GraphicItemText*>(Item);
        Wire = qgraphicsitem_cast<GraphicItemWire*>(Item);

        if (Text != NULL) {
            TextCount++;
        }
        if (Component != NULL) {
            ComponentCount++;
        }
        if (Wire != NULL) {
            WireCount++;
        }
    }

    // Save the Counts
    DataStreamOut << TextCount;
    DataStreamOut << ComponentCount;
    DataStreamOut << WireCount;

    // Save all the Text Items
    foreach (QGraphicsItem* Item, GraphicItems) {
        // Cast to a pointer, if the cast is invalid, the pointer will be NULL
        Text = qgraphicsitem_cast<GraphicItemText*>(Item);
        if (Text != NULL) {
            Text->SaveData(DataStreamOut);
        }
    }

    // Save all the Component Items (and sub items)
    foreach (QGraphicsItem* Item, GraphicItems) {
        // Cast to a pointer, if the cast is invalid, the pointer will be NULL
        Component = qgraphicsitem_cast<GraphicItemComponent*>(Item);
        if (Component != NULL) {
            Component->SaveData(DataStreamOut);
        }
    }

    // Save all the Wire Items (and sub items)
    foreach (QGraphicsItem* Item, GraphicItems) {
        // Cast to a pointer, if the cast is invalid, the pointer will be NULL
        Wire = qgraphicsitem_cast<GraphicItemWire*>(Item);
        if (Wire != NULL) {
            Wire->SaveData(DataStreamOut);
        }
    }
}

void WiringScene::LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int                    x;
    qint32                 TextCount = 0;
    qint32                 ComponentCount = 0;
    qint32                 WireCount = 0;
    int                    JunkWireIndex;
    QMap<QString, int>     JunkComponentByKeyIndex;

    // Reset the Scene
    SetOperationMode(SCENEMODE_SELECTMOVEITEM);
    m_UserChosenSSTInfoDataComponent = NULL;

    // Remove all items from the scene
    QList<QGraphicsItem*> GraphicItems = items();
    foreach (QGraphicsItem* Item, GraphicItems) {
        removeItem(Item);
    }

    // Get the number of each type of Items
    DataStreamIn >> TextCount;
    DataStreamIn >> ComponentCount;
    DataStreamIn >> WireCount;

    // Handle the Version 1.0 data
    if (ProjectFileVersion == SSTWORKBENCHPROJECTFILEFORMATVER_1_0) {
        // Load the Current Item Index's (from 1.0 save data)
        DataStreamIn >> JunkWireIndex;
        GraphicItemData::SetCurrentWireIndex(JunkWireIndex);
        DataStreamIn >> JunkComponentByKeyIndex;
        GraphicItemData::SetComponentIndexByKey(JunkComponentByKeyIndex);

        // NOTE: Version 2 index data is Loaded from the MainWindow
    }

    // Build all the Text Items
    for (x = 0; x < TextCount; x++) {
        CreateNewTextItem(DataStreamIn, ProjectFileVersion);
    }

    // Build all the Component Items
    for (x = 0; x < ComponentCount; x++) {
        CreateNewComponentItem(DataStreamIn, ProjectFileVersion);
    }

    // Build all the Wire Items
    for (x = 0; x < WireCount; x++) {
        CreateNewWireItem(DataStreamIn, ProjectFileVersion);
    }
}

void WiringScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QList<QGraphicsItem*>   PointItems;
    SceneOperationMode_enum CurrentOperationMode;

    // Check to ensure its a right or left buttons
    if ((mouseEvent->button() != Qt::RightButton) && (mouseEvent->button() != Qt::LeftButton))
    {
        // Don't do any processing
        return;
    }

    // Get the Operation Mode
    CurrentOperationMode = GetOperationMode();

    // Special Case for Right Button:
    // If we are in an ADD operation Mode, then
    // change it to a DO NOTHING Mode
    // NOTE: Right clicks are NOT allowed to add objects
    if ((mouseEvent->button() == Qt::RightButton) &&
        ((CurrentOperationMode == SCENEMODE_ADDCOMPONENT) ||
         (CurrentOperationMode == SCENEMODE_ADDTEXT) ||
         (CurrentOperationMode == SCENEMODE_ADDWIRE)))
    {
        CurrentOperationMode = SCENEMODE_DONOTHING;
    }

    // Do what we need to do depending upon the Operating Mode of the Scene
    switch (CurrentOperationMode) {
        case SCENEMODE_ADDCOMPONENT: // LEFT BUTTON ONLY
            CreateNewComponentItem(mouseEvent->scenePos());
        break;

        case SCENEMODE_ADDTEXT: // LEFT BUTTON ONLY
            CreateNewTextItem(mouseEvent->scenePos());
        break;

        case SCENEMODE_ADDWIRE: // LEFT BUTTON ONLY
            CreateNewWireItem(mouseEvent->scenePos());
        break;

        case SCENEMODE_SELECTMOVEITEM:
            /////////////////////////////////

            // GraphicItemComponent's can be selected using left or
            // right buttons.  This allows the user to right click on
            // a component and get the item menu.
            //
            // All other objects (Text, wires, ports) can only be
            // selected using the left button. only.

            ////////////////////////////////////////////////////////
            // FIRST: De-select all items & turn off the properties
            //        display
            ////////////////////////////////////////////////////////
            // De-Select all Items & Turn off the properties display
            emit SceneEventGraphicItemSelected(NULL);
            emit SceneEventGraphicItemSelectedProperties(NULL);

            ////////////////////////////////////////////////////////
            // SECOND: See if the user has selected an item,
            //         If yes, then update the properties for the object
            ////////////////////////////////////////////////////////
            CheckGraphicItemsSelectedAtPoint(mouseEvent);

            /////////////////////////////////////////////
            // THIRD: See if the user clicked on a wire
            //        Hhandle (assuming intent to move it)
            /////////////////////////////////////////////
            // MOVE IS ALLOWED WITH LEFT BUTTON ONLY
            if (mouseEvent->button() == Qt::LeftButton) {
                // Check to see if a wire handle was selected, if yes
                // Do some special processing
                CheckForWireHandlesSelectedAtPoint(mouseEvent);
            }

            /////////////////////////////////////////////
            // FORTH: See if the user clicked on on NOTHING
            //        If so, then perform a rubber band select
            /////////////////////////////////////////////
            // RUBBERBAND SELECT IS WITH LEFT BUTTON ONLY
            if (mouseEvent->button() == Qt::LeftButton) {
                CheckForStartOfRubberBandSelect(mouseEvent);
            }
        break;

        case SCENEMODE_MOVEWIREHANDLE:
        case SCENEMODE_RUBBERBANDSELECT:
            // Do Nothing Here, but moving the Wire Handle or Rubberband select
            // is done by the mouseMoveEvent and mouseReleaseEvent
        break;

        case SCENEMODE_DONOTHING:
            // Do Nothing Here
        break;

        default:
            QMessageBox::critical(NULL, "CODING ERROR", "SCENE MOUSE PRESS - ILLEGAL MODE SELECTED");
        break;
    }

    // Call the default handler (We must use the GetOperationMode() call because the op mode may have changed above)
    if ((GetOperationMode() != SCENEMODE_MOVEWIREHANDLE) && (GetOperationMode() != SCENEMODE_ADDWIRE)) {
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}

void WiringScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    // Are we Currently Adding a Wire (correct mode and the m_ptrNewItemWire exists)
    if ((GetOperationMode() == SCENEMODE_ADDWIRE) && (m_ptrNewItemWire != NULL)) {

        // Move the TempWireLine Graphical Object to the new point position
        m_ptrNewItemWire->UpdateEndPointPosition(mouseEvent->scenePos());

    } else if ((GetOperationMode() == SCENEMODE_MOVEWIREHANDLE) && (m_ptrMovingItemWireHandle != NULL)) {

        // Move the appropriate end of the wire
        if (m_ptrMovingItemWire != NULL) {
            // Figure out which end of the wire to move
            if (m_ptrMovingItemWireHandle->GetWireHandlePointPosition() == GraphicItemWireHandle::STARTPOINT) {
                m_ptrMovingItemWire->UpdateStartPointPosition(mouseEvent->scenePos());
            } else {
                m_ptrMovingItemWire->UpdateEndPointPosition(mouseEvent->scenePos());
            }
        }

    } else if ((GetOperationMode() == SCENEMODE_RUBBERBANDSELECT) && (m_PtrRubberBandSelect != NULL)) {
        // Move the rubber band select
        ProcessMoveOfRubberBandSelect(mouseEvent);
    }

    // Call the default handler
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void WiringScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    // Are we Currently Adding a Wire (correct mode and the m_ptrNewItemWire exists)
    if (GetOperationMode() == SCENEMODE_ADDWIRE && m_ptrNewItemWire != NULL) {

        // Set the Wire to Not Selected so it shows its colors correctly
        m_ptrNewItemWire->SetWireSelected(false);

        // Tell the Main Window we Finally added this Wire (the End point is where the user wants it)
        emit SceneEventWireAddedFinalPlacement(m_ptrNewItemWire);

        // Check to see if the line is too short
        if ((m_ptrNewItemWire->GetLineLength() <= SHORTWIRELENGTHLIMIT) && (m_PrefAutoDeleteTooShortWires == true)) {
            // Undo the add of the wire
            m_UndoStack->undo();
        }

        // Reset the m_ptrNewItemWire to NULL so we can add another wire
        m_ptrNewItemWire = NULL;

    } else if ((GetOperationMode() == SCENEMODE_MOVEWIREHANDLE) && (m_ptrMovingItemWireHandle != NULL)) {

        // The Wire Handle has been released, Go to the MODE SELECTMOVEITEM to stop moving the handle

        // Set the Wire to Not Selected so it shows its colors correctly
        m_ptrMovingItemWire->SetWireSelected(false);
        m_ptrMovingItemWire = NULL;
        m_ptrMovingItemWireHandle = NULL;

        // Go back to the Select Mode
        SetOperationMode(SCENEMODE_SELECTMOVEITEM);

    } else if ((GetOperationMode() == SCENEMODE_RUBBERBANDSELECT) && (m_PtrRubberBandSelect != NULL)) {
        // De-Select all Items & Turn off the properties display
        emit SceneEventGraphicItemSelected(NULL);
        emit SceneEventGraphicItemSelectedProperties(NULL);

        // Finalize the Rubberband select
        FinishRubberBandSelect(mouseEvent);

        // Go back to the Select Mode
        SetOperationMode(SCENEMODE_SELECTMOVEITEM);
    }

    // Call the default handler
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void WiringScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    // Accept the drag & drop event
    event->setAccepted(true);

    // Make sure that we are dropping a Workbench component and not
    // something from another application
    if (event->mimeData()->text() == DRAGDROP_COMPONENTNAME) {
        event->acceptProposedAction();
    } else {
        event->setAccepted(false);
    }
}

void WiringScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    // This routine must exist, because the default handler will
    // prevent Drag & Drop if the target is not a graphical item

    Q_UNUSED(event);
}

void WiringScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    // Accept the drag & drop event
    event->setAccepted(true);

    // Do the drop
    CreateNewComponentItem(event->scenePos());

    // Tell the Main Window we finished the Drag & Drop
    emit SceneEventDragAndDropFinished();
}

void WiringScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    int x;
    int y;
    int GridSize =  ApplicationPreferences::GetAppPref_SnapToGridSize();
    int StartLeft = RoundTo((int)rect.left(), (int) GridSize);
    int StartTop = RoundTo((int)rect.top(), (int) GridSize);

    // Enable Drawing the grid (only draw in the rect area)
    if (ApplicationPreferences::GetAppPref_DisplayGridEnabled() == true) {
        painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
        // Draw the Grid
        for (x = StartLeft; x <= rect.right(); x += GridSize) {
            for (y = StartTop; y <= rect.bottom(); y += GridSize) {
                painter->drawPoint(x, y);
            }
        }
    }

    QGraphicsScene::drawBackground(painter, rect);
}

void WiringScene::HandleTextEditorLostFocus(GraphicItemText* item)
{
    // If the Text Item is empty, populate it with something.
    // We cannot just delete it as this would mess with the undo/redo
    // command engine; Because it adds a command for every text object added
    if (item->toPlainText().isEmpty()) {
        item->setPlainText(TEXT_EMPTYTEXTSTRING);
    }
}

void WiringScene::HandlePreferenceChangeAutoDeleteTooShortWire(bool NewValue)
{
   m_PrefAutoDeleteTooShortWires = NewValue;
}

void WiringScene::CheckGraphicItemsSelectedAtPoint(QGraphicsSceneMouseEvent* mouseEvent)
{
    QList<QGraphicsItem*>  PointItems;
    QGraphicsItem*         SelectedItem = NULL;
    QGraphicsItem*         GraphicItem = NULL;
    GraphicItemBase*       BaseItem = NULL;
    GraphicItemWireHandle* WireHandle = NULL;

    // Get a list of items at this point (in Decending Order to select the item on top)
    PointItems = items(mouseEvent->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);

    // See if anything was in the list of items
    if (PointItems.count() > 0) {
        // Get the first SelectedItem
        SelectedItem = PointItems.first();

        // Look through the list of items, and pick the first one that is a GraphicItemXXX type
        for (int x = 0; x < PointItems.count(); x++) {
            SelectedItem = PointItems.at(x);
            if (SelectedItem->type() >= QGraphicsItem::UserType) {
                break;
            }
        }

        // See if the object is one of our GraphicItemXXX or is it a generic QGrapicsItem?
        if (SelectedItem->type() >= QGraphicsItem::UserType) {

            // See if we can cast the item to a wire handle, Note: the cast will return
            // NULL if SelectedItem is not of the correct type that we are casting to
            if (mouseEvent->button() == Qt::LeftButton) { // Select WireHandles with LEFT Button only
                WireHandle = qgraphicsitem_cast<GraphicItemWireHandle*>(SelectedItem);
                if (WireHandle != NULL) {
                    // We got a wire handle, but we actually need the wire that the wire handle is connected to.
                    // Get the actual Wire that is connected to this wire handle and return it as the base item
                    BaseItem = WireHandle->GetConnectedWire();
                    GraphicItem = WireHandle;
                }
            }
            // If Item is not a WireHandle, is it one of the other items
            if (BaseItem == NULL) {  // Select Components with LEFT or RIGHT Button
                BaseItem = qgraphicsitem_cast<GraphicItemComponent*>(SelectedItem);
                GraphicItem = qgraphicsitem_cast<GraphicItemComponent*>(SelectedItem);
            }
            if (mouseEvent->button() == Qt::LeftButton) {  // Select WireLineSegments, Wires, Ports and Text with LEFT Button only
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemWireLineSegment*>(SelectedItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemWireLineSegment*>(SelectedItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemWire*>(SelectedItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemWire*>(SelectedItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemPort*>(SelectedItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemPort*>(SelectedItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
                }
            }

            // If we got something, then display its properties
            if (BaseItem != NULL) {
                // Notify the Main window that the item has been selected and this is its properties
                emit SceneEventGraphicItemSelected(GraphicItem);
                emit SceneEventGraphicItemSelectedProperties(BaseItem->GetItemProperties());
            }
        }
    }
}

void WiringScene::CheckForWireHandlesSelectedAtPoint(QGraphicsSceneMouseEvent* mouseEvent)
{
    QList<QGraphicsItem*>   PointItems;

    // Get a list of ALL items at mouse point
    PointItems = items(mouseEvent->scenePos());

    // See if the selected Items contain anything
    if (PointItems.count() > 0) {
        // Check to see if the top Item is a Wire Handle
        // Cast the first item in the items list to a GraphicItemWireHandle (or NULL if this item is not a Wire Handle)
        m_ptrMovingItemWireHandle = qgraphicsitem_cast<GraphicItemWireHandle*>(PointItems.first());

        // Now figure out the Actual Wire that the WireHandle is attached to
        if (m_ptrMovingItemWireHandle != NULL) {
            m_ptrMovingItemWire = qgraphicsitem_cast<GraphicItemWire*>(m_ptrMovingItemWireHandle->parentItem());
            // Final Check
            if (m_ptrMovingItemWire != NULL) {
                // Deselect any other items that might be selected
                SetSingleGraphicItemAsSelected(NULL);
                // Force select of moving wire wire
                m_ptrMovingItemWire->SetWireSelected(true);
            } else {
                // Opps, we have a handle, but not a wire... Something is screwed up
                m_ptrMovingItemWireHandle = NULL;
                QMessageBox::critical(NULL, "CODING ERROR", "SCENE MOUSE PRESS - WIRE HANDLE CANNOT FIND WIRE");
            }

            // Now change the operation move to Moving a Wire Handle
            SetOperationMode(SCENEMODE_MOVEWIREHANDLE);
        }
    }
}

void WiringScene::CheckGroupGraphicItemsSelected(QList<QGraphicsItem*>& SelectedItems)
{
    QGraphicsItem*         CheckItem = NULL;
    QGraphicsItem*         GraphicItem = NULL;
    GraphicItemBase*       BaseItem = NULL;
    GraphicItemWireHandle* WireHandle = NULL;
    int                    x;
    bool                   ItemPropertiesDisplayed = false;

    // See if anything was in the list of items
    if (SelectedItems.count() > 0) {
        // Check all items in the list of Selected Items
        for (x = 0; x < SelectedItems.count(); x++) {
            CheckItem = SelectedItems.at(x);
            // Reset the check variables
            GraphicItem = NULL;
            BaseItem = NULL;
            WireHandle = NULL;

            // See if the object is one of our GraphicItemXXX or is it a generic QGrapicsItem?
            if (CheckItem->type() >= QGraphicsItem::UserType) {

                // See if we can cast the item to a wire handle, Note: the cast will return
                // NULL if SelectedItem is not of the correct type that we are casting to
                WireHandle = qgraphicsitem_cast<GraphicItemWireHandle*>(CheckItem);
                if (WireHandle != NULL) {
                    // We got a wire handle, but we actually need the wire that the wire handle is connected to.
                    // Get the actual Wire that is connected to this wire handle and return it as the base item
                    BaseItem = WireHandle->GetConnectedWire();
                    GraphicItem = WireHandle;
                }
                // If Item is not a WireHandle, is it one of the other items
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemComponent*>(CheckItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemComponent*>(CheckItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemWireLineSegment*>(CheckItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemWireLineSegment*>(CheckItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemWire*>(CheckItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemWire*>(CheckItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemPort*>(CheckItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemPort*>(CheckItem);
                }
                if (BaseItem == NULL) {
                    BaseItem = qgraphicsitem_cast<GraphicItemText*>(CheckItem);
                    GraphicItem = qgraphicsitem_cast<GraphicItemText*>(CheckItem);
                }

                // If we got something, then display its properties
                if (BaseItem != NULL) {
                    // Set the Graphic Item Selected
                    GraphicItem->setSelected(true);

                    // Present the Properties of the first one that we can use
                    if (ItemPropertiesDisplayed == false) {
                        if ((GraphicItem->type() == QGraphicsItem::UserType + ITEMTYPE_COMPONENT) ||
                            (GraphicItem->type() == QGraphicsItem::UserType + ITEMTYPE_PORT) ||
                            (GraphicItem->type() == QGraphicsItem::UserType + ITEMTYPE_WIRELINESEGMENT) ||
                            (GraphicItem->type() == QGraphicsItem::UserType + ITEMTYPE_WIRE)) {

                            // Notify the Main window that the item has been selected and this is its properties
                            emit SceneEventGraphicItemSelected(GraphicItem);
                            emit SceneEventGraphicItemSelectedProperties(BaseItem->GetItemProperties());
                            ItemPropertiesDisplayed = true;
                        }
                    }
                }
            }
        }
    }
}

void WiringScene::CheckForStartOfRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent)
{
    QList<QGraphicsItem*>   PointItems;

    // Get a list of ALL items at mouse point
    PointItems = items(mouseEvent->scenePos());

    // See if we clicked on NOTHING
    if (PointItems.count() == 0) {
        // Now change the operation move to Moving a Wire Handle
        SetOperationMode(SCENEMODE_RUBBERBANDSELECT);

        // Create the Select Rubber Band and add it to the scene
        m_RubberBandStartX = mouseEvent->scenePos().x();
        m_RubberBandStartY = mouseEvent->scenePos().y();
        m_PtrRubberBandSelect = new QGraphicsRectItem(m_RubberBandStartX, m_RubberBandStartY, 0, 0, NULL);
        QPen SelectPen(Qt::DashDotDotLine);
        SelectPen.setWidth(2);
        m_PtrRubberBandSelect->setPen(SelectPen);
        addItem(m_PtrRubberBandSelect);
    }
}

void WiringScene::ProcessMoveOfRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent)
{
    // Figure out the x and y Points
    QRectF NewRect;
    int    Mouse_x = mouseEvent->scenePos().x();
    int    Mouse_y = mouseEvent->scenePos().y();

    // Figure out which point is top, bottom, left and right
    if (Mouse_x >= m_RubberBandStartX) {
        NewRect.setLeft(m_RubberBandStartX);
        NewRect.setRight(Mouse_x);
    } else {
        NewRect.setLeft(Mouse_x);
        NewRect.setRight(m_RubberBandStartX);
    }
    if (Mouse_y >= m_RubberBandStartY) {
        NewRect.setTop(m_RubberBandStartY);
        NewRect.setBottom(Mouse_y);
    } else {
        NewRect.setTop(Mouse_y);
        NewRect.setBottom(m_RubberBandStartY);
    }

    // Move the Rubber Band Rect
    m_PtrRubberBandSelect->setRect(NewRect);
}

void WiringScene::FinishRubberBandSelect(QGraphicsSceneMouseEvent* mouseEvent)
{
    QRectF                SelectRect;
    QList<QGraphicsItem*> RectItems;

    // Set the final position of the rect.
    ProcessMoveOfRubberBandSelect (mouseEvent);

    // Select the object contained withing the Rubber band rect
    SelectRect = m_PtrRubberBandSelect->rect();

    // Get a list of items inside this rect (in Decending Order to select the item on top)
    // Note: the entire object must be in contained in the rect
    RectItems = items(SelectRect, Qt::ContainsItemShape, Qt::DescendingOrder);

    // Chec the group of items selected
    CheckGroupGraphicItemsSelected(RectItems);

    // Delete the Rubber Band Select Rectangle Object
    delete m_PtrRubberBandSelect;
    m_PtrRubberBandSelect = NULL;
}

void WiringScene::SetSingleGraphicItemAsSelected(QGraphicsItem* NewItem)
{
    // Clear all selected item and then select only the NewItem

    // Clear any selected items
    clearSelection();

    // Set the Component to selected
    if (NewItem != NULL) {
        NewItem->setSelected(true);
    }
}

QString WiringScene::CheckComponentReqsAndBuildKey(int AllowedInstances, ComponentType_enum CompType, QString ParentElementName, QString ComponentName)
{
    QString Key;

    int NumItemsCreatedByType = GraphicItemData::GetGraphicItemComponentByTypeList(CompType).count();

    // Check to see if we can create this component based on its component type
    // If the allowed number of instances is -1, then it is unlimited
    if (AllowedInstances >= 0) {
        if (NumItemsCreatedByType >= AllowedInstances) {
            QString MsgText = QString("Cannot Create New Component Of Type:\n\n%1\n\nExceeded Creation Limit Of %2")
                              .arg(SSTInfoDataComponent::GetComponentTypeName(CompType)).arg(AllowedInstances);
            QMessageBox::warning(NULL, "Cannot Create New Component", MsgText);
            return "";  // Failed, return an empty key
        }
    }

    // Get the Component Key
    Key = BuildComponentKey(ParentElementName, ComponentName);

    return Key;
}

QString WiringScene::BuildComponentKey(QString ElementName, QString ComponentName)
{
    QString IndexKey;
    IndexKey = ElementName + "." + ComponentName;
    return IndexKey;
}

