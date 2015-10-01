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

#include "GraphicItemWireLineSegment.h"

////////////////////////////////////////////////////////////

GraphicItemWireLineSegment::GraphicItemWireLineSegment(Position LineSegPosition, ItemProperties* ParentProperties, QGraphicsItem* parent/*=0*/)
    : QGraphicsLineItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_WIRELINESEGMENT)

{
    // Point this objects properties to the parent's properties
    SetExistingPropertiesStructure(ParentProperties);

    // Setup the internal member variables
    m_LineSegPosition = LineSegPosition;  // Identifies what type of Line Segment it is
    m_AutoMoveFlag = true;                // Identifies if the application can automatically move this wire segment,
                                          // or is it under user control

    // Set Graphic parameters for the Line Segment Items
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    // Handle cases for the middle lines; make them moveable
    if ( IsWireLineAMiddleSegment()) {
        setFlag(QGraphicsItem::ItemIsMovable);
    }
}

GraphicItemWireLineSegment::~GraphicItemWireLineSegment()
{
    // Our properties pointer actually points to the parent
    // properties, so we just null it out and let the parent delete it.
    SetExistingPropertiesStructure(NULL);
}

void GraphicItemWireLineSegment::SaveData(QDataStream& DataStreamOut)
{
    Q_UNUSED(DataStreamOut)
    // All Wire Data is saved in GraphicItemWire
}

bool GraphicItemWireLineSegment::IsWireLineAMiddleSegment()
{
    return ( (m_LineSegPosition != SEGPOS_STARTLINE) && (m_LineSegPosition != SEGPOS_ENDLINE));
}

void GraphicItemWireLineSegment::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*=0*/)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // We have to paint our own line because normally when the line is selected,
    // a large bounding rectable is shown... This bounding rect is drawn
    // by default by the base class paint function, therefore we override it

    painter->setPen(pen());
    painter->drawLine(line());

    // Uncomment the following to show the selection outline when item is selected
    //QGraphicsLineItem::paint(painter, option, widget);
}

QVariant GraphicItemWireLineSegment::itemChange(GraphicsItemChange change, const QVariant& value)
{
    QPointF OldPos;
    QPointF NewPos;
    QPointF FinalPos;
    bool    SelectedState;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    // Check to see if the X/Y position of the wire segement is potentially changing
    // NOTE: Position is not the same as the line.  Position is the single X/Y coordinate relative
    //       to the parent item, and is used as an offset for drawing the line.  Therefore,
    //       We really dont want the position (offset relative to the parent) to change, but we do want
    //       to know what the user has requested it to move by.  We send this positional change up
    //       to the parent so it can redraw the line in the correct position.  Also, this routine
    //       may be called multiple times when the user is trying to move a line segment, and it
    //       will call the parent handler each time.
    if (change == ItemPositionChange && scene()) {
        // Check the new position to allow change only in Horizontal or Vertical directions

        // Get the old position and the proposed new position
        NewPos = value.toPointF();
        OldPos = pos();

        // Depending upon what line segment, control what direction (Horizontal or Vertical) that it can move
        switch (m_LineSegPosition) {
            case SEGPOS_STARTLINE :     FinalPos = OldPos; break;                           // STARTLINE cannot change position
            case SEGPOS_MIDDLEVLINE1 :  FinalPos = QPointF(NewPos.x(), OldPos.y()); break;  // Can only move in the X direction (Horizontal)
            case SEGPOS_MIDDLEHLINE :   FinalPos = QPointF(OldPos.x(), NewPos.y()); break;  // Can only move in the Y direction (Vertical)
            case SEGPOS_MIDDLEVLINE2 :  FinalPos = QPointF(NewPos.x(), OldPos.y()); break;  // Can only move in the X direction (Horizontal)
            case SEGPOS_ENDLINE :       FinalPos = OldPos; break;                           // ENDLINE cannot change position
        }

        // Call the Parent's handler, to update the position of the peer Line Segments
        // NOTE: We dont move the position (offset relative to the parent), instead
        //       we let the parent move the lines by the offset amount.
        ((GraphicItemWire*)parentItem())->HandleWireLineSegmentItemChange(FinalPos, this, change, value);

        // Dont move the position (offset relative to the parent),
        return QVariant(OldPos);
    }

    // For all other conditions, Call the Parent's handler and then the base Handler
    // Note: FinalPos will = 0, 0 in this casse.
    ((GraphicItemWire*)parentItem())->HandleWireLineSegmentItemChange(FinalPos, this, change, value);

    // Then call the default handler
    return QGraphicsLineItem::itemChange(change, value);
}

void GraphicItemWireLineSegment::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Notify the parent that we have released the mouse (basiclly we are done moving this item)
    ((GraphicItemWire*)parentItem())->HandleWireLineSegmentMouseReleaseEvent(event);

    // Call the default handler
    QGraphicsLineItem::mouseReleaseEvent(event);
}

