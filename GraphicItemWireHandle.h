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

#ifndef GRAPHICITEMWIREHANDLE_H
#define GRAPHICITEMWIREHANDLE_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"
#include "GraphicItemWire.h"
#include "GraphicItemPort.h"

// Forward declarations to allow compile
class GraphicItemPort;
class GraphicItemWire;

////////////////////////////////////////////////////////////

class GraphicItemWireHandle : public QGraphicsRectItem, public GraphicItemBase
{
public:
    // Enumerations for Identifying the Wire Point Position
    enum WirePointPosition { STARTPOINT, ENDPOINT };

    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_WIREHANDLE };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemWireHandle(const QPointF& InitPosition, WirePointPosition WirePointPos, QGraphicsItem* parent=0);
    virtual ~GraphicItemWireHandle();

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

public:
    // Update Positition and Control Visibility
    void UpdatePosition(const QPointF& NewPosition);
    void MakeVisible(bool VisibleFlag);

    // Set / Get Connected Port
    void SetWireHandleConnectedPort(GraphicItemPort* ptrPort);
    GraphicItemPort* GetWireHandleConnectedPort() {return m_HandleConnectedPort;}

    // Get the parent GraphicItemWire
    GraphicItemWire* GetConnectedWire();

    // Get the current Point Position of this Wire Handle
    WirePointPosition GetWireHandlePointPosition() {return m_WirePointPosition;}

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    QPointF           m_CurrentPosition;
    WirePointPosition m_WirePointPosition;
    GraphicItemPort*  m_HandleConnectedPort;
};

#endif // GRAPHICITEMWIREHANDLE_H

