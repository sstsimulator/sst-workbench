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

#include "GraphicItemWireHandle.h"

////////////////////////////////////////////////////////////

GraphicItemWireHandle::GraphicItemWireHandle(const QPointF& InitPosition, WirePointPosition WirePointPos, QGraphicsItem* parent /*=0*/)
    : QGraphicsRectItem(parent), GraphicItemBase(ITEMTYPE_WIREHANDLE)
{
    m_HandleConnectedPort = NULL;
    m_WirePointPosition = WirePointPos;

    // Setup the size and color of the handle
    setRect(WIRE_HANDLE_RECT);

    // Initially Set the handle as not connected
    SetWireHandleConnectedPort(NULL);

    // Update the position and make visible
    UpdatePosition(InitPosition);
    MakeVisible(false);

    // Set Graphic parameters for the Line Handle Items
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

GraphicItemWireHandle::~GraphicItemWireHandle()
{
}

void GraphicItemWireHandle::SaveData(QDataStream& DataStreamOut)
{
    Q_UNUSED(DataStreamOut)
    // All Wire Data is saved in GraphicItemWire
}

void GraphicItemWireHandle::UpdatePosition(const QPointF& NewPosition)
{
    m_CurrentPosition = NewPosition;
    setPos(NewPosition);
}

void GraphicItemWireHandle::MakeVisible(bool VisibleFlag)
{
    setVisible(VisibleFlag);
}

void GraphicItemWireHandle::SetWireHandleConnectedPort(GraphicItemPort* ptrPort)
{
    // If the Port is NULL, then this handle is not connected to a port
    // Set the color of the handl as appropriate
    if (ptrPort != NULL)
    {
        // Color of handle when connected to a port
        if (m_WirePointPosition == STARTPOINT) {
            setBrush(WIRE_HANDLE_START_CONNECTED);
        } else {
            setBrush(WIRE_HANDLE_END_CONNECTED);
        }
    } else {
        // Color of handle when NOT connected to a port
        if (m_WirePointPosition == STARTPOINT) {
            setBrush(WIRE_HANDLE_START_DISCONNECTED);
        } else {
            setBrush(WIRE_HANDLE_END_DISCONNECTED);
        }
    }

    m_HandleConnectedPort = ptrPort;

    // Now Tie this handle to the port
    if (ptrPort != NULL) {
        ptrPort->SetPortConnectedWireHandle(this);
    }
}

QVariant GraphicItemWireHandle::itemChange(GraphicsItemChange change, const QVariant& value)
{
    bool    SelectedState;
    QPointF NewPos;
    QPointF NewPosAdjusted;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    if (change == QGraphicsItem::ItemPositionChange && scene()) {
        // Check to see if we need to snap to grid
        NewPos = value.toPointF();
        NewPosAdjusted = SnapToGrid::CheckSnapToGrid(NewPos);
        return NewPosAdjusted;
    }

    return QGraphicsItem::itemChange(change, value);
}


GraphicItemWire* GraphicItemWireHandle::GetConnectedWire()
{
    GraphicItemWire* ptrWire;
    QGraphicsItem*    ParentItem;

    ParentItem = parentItem();

    // Cast the parent of this WireHandle to a GraphicItemWire (or NULL if this item is not a wire)
    ptrWire = qgraphicsitem_cast<GraphicItemWire*>(parentItem());
    return ptrWire;
}

