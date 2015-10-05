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

#ifndef GRAPHICITEMWIRELINESEGMENT_H
#define GRAPHICITEMWIRELINESEGMENT_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"
#include "GraphicItemWire.h"

////////////////////////////////////////////////////////////

class GraphicItemWireLineSegment : public QGraphicsLineItem, public GraphicItemBase
{
public:
    // Enumerations for Identifying the item type
    enum Position { SEGPOS_STARTHLINE, SEGPOS_MIDDLEVLINE1, SEGPOS_MIDDLEHLINE, SEGPOS_MIDDLEVLINE2, SEGPOS_ENDHLINE };

    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_WIRELINESEGMENT };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemWireLineSegment(Position LineSegPosition, ItemProperties* ParentProperties, QGraphicsItem* parent = 0);
    virtual ~GraphicItemWireLineSegment();

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

    bool GetAutoMoveFlag() {return m_AutoMoveFlag;}
    void ClearAutoMoveFlag() {m_AutoMoveFlag = false;}

    // Is this a Middle Wire Segment
    bool IsWireLineAMiddleSegment();

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    Position m_LineSegPosition;  // Identifies what type of Line Segment it is
    bool     m_AutoMoveFlag;     // Identifies if the application can automatically move this wire segment,
                                 // or is it under user control
};


#endif // GRAPHICITEMWIRELINESEGMENT_H
