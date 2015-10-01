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

#include "UndoRedoCommands.h"

///////////////////////////////////////////////////////////////////////////////////////

ComandAddGraphicItemComponent::ComandAddGraphicItemComponent(GraphicItemComponent* Component, WiringScene* Scene, bool PasteMode /*=false*/, QUndoCommand* parent /*=0*/)
    : QUndoCommand(parent)
{
    m_Component = Component;
    m_WiringScene = Scene;
    m_PasteMode = PasteMode;
}

ComandAddGraphicItemComponent::~ComandAddGraphicItemComponent()
{
    // Delete the Component
    delete m_Component;
}

void ComandAddGraphicItemComponent::undo()
{
    // Delete the Object
    m_WiringScene->DeleteComponentFromScene(m_Component);

    UpdateCommandText();
}

void ComandAddGraphicItemComponent::redo()
{
    // Add the Object
    m_WiringScene->AddNewComponentItemToScene(m_Component, !m_PasteMode);

    UpdateCommandText();

//  m_WiringScene->RefreshAllCurrentWirePositions();  // Re-enable when Move Command is handled
    // NOTE: Refreshing all wire positions will cause the wire->UpdateWireLineSegmentPositions
    //       to emit ItemWireSetProjectDirty() which will set the  project dirty bit that will
    //       override the undo stack clean flag.
    //       Until this call above is re-enabled, a bug can exist that when redo'ing an add
    //       command, its possible that existing wires might not connect back up.
}

void ComandAddGraphicItemComponent::UpdateCommandText()
{
    QString ModeText;

    m_PasteMode ? ModeText = "Paste" : ModeText = "Add";
    QString CommandText = ModeText + QString(" Component (%1)").arg(m_Component->GetComponentDisplayName());
    setText(CommandText);
}

///////////////////////////////////////////////////////////////////////////////////////

ComandAddGraphicItemText::ComandAddGraphicItemText(GraphicItemText* Text, WiringScene* Scene, bool PasteMode /*=false*/, QUndoCommand* parent /*=0*/)
    : QUndoCommand(parent)
{
    m_Text = Text;
    m_WiringScene = Scene;
    m_PasteMode = PasteMode;
}

ComandAddGraphicItemText::~ComandAddGraphicItemText()
{
    // Delete the Text
    delete m_Text;
}

void ComandAddGraphicItemText::undo()
{
    // Delete the Object
    m_WiringScene->DeleteTextFromScene(m_Text);

    UpdateCommandText();
}

void ComandAddGraphicItemText::redo()
{
    // Add the Object
    m_WiringScene->AddNewTextItemToScene(m_Text, !m_PasteMode);

    UpdateCommandText();
}

void ComandAddGraphicItemText::UpdateCommandText()
{
    QString CommandText;
    QString ModeText;

    m_PasteMode ? ModeText = "Paste" : ModeText = "Add";
    if (m_Text->toPlainText().isEmpty() == true) {
        CommandText = ModeText + QString(" Text");
    } else {
        CommandText = ModeText + QString(" Text \"%1\"").arg(m_Text->toPlainText());
    }
    setText(CommandText);
}

///////////////////////////////////////////////////////////////////////////////////////

ComandAddGraphicItemWire::ComandAddGraphicItemWire(GraphicItemWire* Wire, WiringScene* Scene, bool PasteMode /*=false*/, QUndoCommand* parent /*=0*/)
    : QUndoCommand(parent)
{
    m_Wire = Wire;
    m_WiringScene = Scene;
    m_PasteMode = PasteMode;
}

ComandAddGraphicItemWire::~ComandAddGraphicItemWire()
{
    // Delete the Component
    delete m_Wire;
}

void ComandAddGraphicItemWire::undo()
{
    // Delete the Object
    m_WiringScene->DeleteWireFromScene(m_Wire);

    UpdateCommandText();
}

void ComandAddGraphicItemWire::redo()
{
    // Add the Object
    m_WiringScene->AddNewWireItemToScene(m_Wire, true);

    UpdateCommandText();
}

void ComandAddGraphicItemWire::UpdateCommandText()
{
    QString ModeText;

    m_PasteMode ? ModeText = "Paste" : ModeText = "Add";
    QString CommandText = ModeText + QString(" Wire #%1").arg(m_Wire->GetWireIndex());
    setText(CommandText);
}


///////////////////////////////////////////////////////////////////////////////////////

ComandDeleteGraphicItems::ComandDeleteGraphicItems(WiringScene* Scene, QUndoCommand* parent /*=0*/)
    : QUndoCommand(parent)
{
    m_WiringScene = Scene;
    m_Initialized = false;
}

ComandDeleteGraphicItems::~ComandDeleteGraphicItems()
{
}

void ComandDeleteGraphicItems::undo()
{
    // Re-Add all Deleted Wires
    foreach (GraphicItemWire* item, m_DeletedWireList) {
        // Add the Object
        m_WiringScene->AddNewWireItemToScene(item, true);
        item->SetWireSelected(true);  // Have to toggle selection on off to sync all wire segments
        item->SetWireSelected(false);
    }

    // Re-Add all Deleted Components
    foreach (GraphicItemComponent* item, m_DeletedComponentList) {
        // Add the Object
        m_WiringScene->AddNewComponentItemToScene(item);
        item->setSelected(false);
    }

    // Re-Add all Deleted Text
    foreach (GraphicItemText* item, m_DeletedTextList) {
        // Add the Object
        m_WiringScene->AddNewTextItemToScene(item);
        item->setSelected(false);
    }
    m_WiringScene->RefreshAllCurrentWirePositions();
}

void ComandDeleteGraphicItems::redo()
{
    GraphicItemWire*      ptrParentWire;
    GraphicItemComponent* ptrComponent;
    GraphicItemText*      ptrText;
    int                   NumItemsDeleted = 0;

    // Figure out if this is the first time running
    if (m_Initialized == false)
    {
        // We need to get the lists of objects from the selections
        foreach (QGraphicsItem* item, m_WiringScene->selectedItems()) {
            if (item->type() == GraphicItemWireLineSegment::Type) {
                // Get the items Parent (Graphic Wire item)
                ptrParentWire = (GraphicItemWire*)item->parentItem();

                // Ensure that only one copy of the Parent Wire is in the list
                if (m_DeletedWireList.indexOf(ptrParentWire) == -1) {
                    // Add this Item to the list of deleted Wires
                    m_DeletedWireList.append(ptrParentWire);
                }
            }
        }

        foreach (QGraphicsItem* item, m_WiringScene->selectedItems()) {
            if (item->type() == GraphicItemComponent::Type) {
                // Get the item as a Graphic Ccomponent item
                ptrComponent = (GraphicItemComponent*)item;

                // Add this Item to the list of deleted Components
                m_DeletedComponentList.append(ptrComponent);
            }
        }

        foreach (QGraphicsItem* item, m_WiringScene->selectedItems()) {
            if (item->type() == GraphicItemText::Type) {
                // Get the item as a Graphic Text item
                ptrText = (GraphicItemText*)item;

                // Add this Item to the list of deleted Text
                m_DeletedTextList.append(ptrText);
            }
        }
    }

    // Mark this object as Initialized
    m_Initialized = true;

    // Delete All Wires in the DeleteWireList
    foreach (GraphicItemWire* item, m_DeletedWireList) {
        // Delete the Item from the Scene
        m_WiringScene->DeleteWireFromScene(item);
    }

    // Delete All Components in the DeleteComponentList
    foreach (GraphicItemComponent* item, m_DeletedComponentList) {
        // Delete the Item from the Scene
        m_WiringScene->DeleteComponentFromScene(item);
    }

    // Delete All Text in the DeleteTextList
    foreach (GraphicItemText* item, m_DeletedTextList) {
        // Delete the Item from the Scene
        m_WiringScene->DeleteTextFromScene(item);
    }

    // Figure out how many items were deleted
    NumItemsDeleted += m_DeletedWireList.count();
    NumItemsDeleted += m_DeletedComponentList.count();
    NumItemsDeleted += m_DeletedTextList.count();

    if (NumItemsDeleted > 1) {
        setText(QString("Delete %1 Items").arg(NumItemsDeleted));
    } else {
        setText(QString("Delete %1 Item").arg(NumItemsDeleted));
    }
}

