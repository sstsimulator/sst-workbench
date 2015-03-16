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

#ifndef GRAPHICITEMWIRE_H
#define GRAPHICITEMWIRE_H

#include "GlobalIncludes.h"

#include "GraphicItemWireLineSegment.h"
#include "GraphicItemWireHandle.h"
#include "GraphicItemPort.h"

// Forward declarations to allow compile
class GraphicItemWireLineSegment;

////////////////////////////////////////////////////////////

class GraphicItemWire : public QGraphicsObject, public GraphicItemBase
{
    Q_OBJECT

public:
    // Enumerations for Identifying the Wire Point Position
    enum ConnectedState { NO_CONNECTIONS, STARTPOINT_CONNECTED, ENDPOINT_CONNECTED, FULL_CONNECTION };

    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_WIRE };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemWire(int WireIndex, const QPointF& StartPoint, const QPointF& EndPoint, QGraphicsItem* parent = 0);
    GraphicItemWire(QDataStream& DataStreamIn, int NewWireIndex = -1, QGraphicsItem* parent = 0);   // Only used for serialization
    virtual ~GraphicItemWire();

    // General info on the wire
    int GetWireIndex() {return m_WireIndex;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

    // Update the Start / End Point Positions (Main way to move a wire)
    void UpdateStartPointPosition(const QPointF& NewPointLocation);
    void UpdateEndPointPosition(const QPointF& NewPointLocation);
    void SetPastePosition(int PasteOffset);

    // Disconnect Ports
    void DisconnectAllAttachedPorts();
    void DisconnectPort(GraphicItemPort* Port, int MoveXOffset = 0);

    // Get Information on the Connected Ports
    GraphicItemPort* GetStartPointConnectedPort() {return m_StartPointCurrentPortConnected;}
    GraphicItemPort* GetEndPointConnectedPort() {return m_EndPointCurrentPortConnected;}

    // Set the Wire Selected State (Will call the sub objects to select them as necessary)
    void SetWireSelected(bool SelectedState);

    // Get Start / End Point Positions
    QPointF GetStartPoint() {return m_StartPoint;}
    QPointF GetEndPoint() {return m_EndPoint;}

    // Callback Handlers from the Lower Level WireLiineSegments
    void HandleWireLineSegmentItemChange(const QPointF& NewPos, GraphicItemWireLineSegment* CallingWireLineSegment, GraphicsItemChange change, const QVariant& value);
    void HandleWireLineSegmentMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    ConnectedState GetWireConnectedState() {return m_WireConnectedState;}

signals:
    void ItemWireSetProjectDirty();

private:
    // Update routines for the Wire Positions
    void UpdatePointPositions(GraphicItemWireLineSegment* CallingWireLineSegment);
    void UpdatePortAndWireHandleSettings();
    void UpdateWireLineSegmentPositions(const QPointF& NewPos, GraphicItemWireLineSegment* CallingWireLineSegment);
    void UpdateWireSelectedState(bool SelectedState);
    void UpdateWireLineZOrder(qreal z);
    void UpdateWireLinePens();

private:
    // Overridden virtual functions
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    // Handle Changes to the Ports Properties
    void PropertyChanged(QString& PropName, QString& NewPropValue);

private:
    int                         m_WireIndex;

    QLineF                      m_PointToPointLine;
    QPointF                     m_StartPoint;
    QPointF                     m_EndPoint;

    QPointF                     m_MiddleStartVertical1Point;
    QPointF                     m_MiddleEndVertical1Point;

    qreal                       m_Current_x_VerticalDrawPoint;

    GraphicItemPort*            m_StartPointCurrentPortConnected;
    GraphicItemPort*            m_EndPointCurrentPortConnected;

    GraphicItemWireHandle*      m_StartPointHandle;
    GraphicItemWireHandle*      m_EndPointHandle;

    GraphicItemWireLineSegment* m_StartPointLine;
    GraphicItemWireLineSegment* m_MiddleVLine1;
    GraphicItemWireLineSegment* m_EndPointLine;

    bool                        m_AutomaticWireLinePositioning;

    QColor                      m_CurrentWireColor;
    QPen                        m_CurrentPen;
    Qt::PenStyle                m_CurrentPenStyle;

    bool                        m_WireSelectedState;
    ConnectedState              m_WireConnectedState;
};

#endif // GRAPHICITEMWIRE_H

