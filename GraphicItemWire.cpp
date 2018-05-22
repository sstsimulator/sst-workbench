////////////////////////////////////////////////////////////////////////
// Copyright 2009-2018 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2018, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#include "GraphicItemWire.h"

////////////////////////////////////////////////////////////

GraphicItemWire::GraphicItemWire(int WireIndex, const QPointF& StartPoint, const QPointF& EndPoint, QGraphicsItem* parent /*=0*/)
    : QGraphicsObject(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_WIRE)
{
    m_WireIndex = WireIndex;

    // Build the Wire Line Segments
    m_StartPointLine = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_STARTLINE, GetItemProperties(), this);
    m_MiddleVLine1   = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_MIDDLEVLINE1, GetItemProperties(), this);
    m_EndPointLine   = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_ENDLINE, GetItemProperties(), this);

    // Flag to control if we automatically move the middle line segments
    // Once cleared, we used the saved positions with any updates
    m_AutomaticWireLinePositioning = true;

    // Identify that the wire is selected
    // (the user created it, so they must want a wire to move around)
    m_WireSelectedState = true;
    m_WireConnectedState = NO_CONNECTIONS;

    // Set the wire color and style for the pen  selected mode
    m_CurrentWireColor = WIRE_COLOR_DISCONNECTED;
    m_CurrentPenStyle  = WIRE_STYLE_SELECTED;

    // Initialize the Graphical Pen for the wires
    UpdateWireLinePens();

    // Save off the line points
    m_StartPoint = StartPoint;
    m_EndPoint = EndPoint;
    m_PointToPointLine = QLineF(m_StartPoint, m_EndPoint);

    // Init variables that track when ports are touched
    m_StartPointCurrentPortConnected = NULL;
    m_EndPointCurrentPortConnected = NULL;

    // Now draw a initial line.  User will later move it via endpoint
    QLineF InitialLine(m_StartPoint, m_EndPoint);

    // And draw the initial 3 wires at the start/end points
    // User will later move it via endpoint
    m_StartPointLine->setLine(InitialLine);
    m_MiddleVLine1->setLine(InitialLine);
    m_EndPointLine->setLine(InitialLine);

    // Set the Z height (Wires are on top of Components, but lower than Text)
    setZValue(WIRE_SELECTED_ZVALUE);

    // Build the handles at the end of the lines, start them off as invisible
    m_StartPointHandle = new GraphicItemWireHandle(m_StartPoint, GraphicItemWireHandle::STARTPOINT, this);
    m_StartPointHandle->MakeVisible(false);
    m_EndPointHandle = new GraphicItemWireHandle(m_EndPoint, GraphicItemWireHandle::ENDPOINT, this);
    m_EndPointHandle->MakeVisible(false);

    // Now set the Properties for this Wire
    GetItemProperties()->AddProperty(WIRE_PROPERTY_NUMBER, QString("%1").arg(m_WireIndex), "Wire Index", ItemProperty::READONLY, false);
    GetItemProperties()->AddProperty(WIRE_PROPERTY_COMMENT, "", "Comment on this Wire", ItemProperty::READWRITE, false);
}

GraphicItemWire::GraphicItemWire(QDataStream& DataStreamIn, int NewWireIndex /*=-1*/, QGraphicsItem* parent /*=0*/)
    : QGraphicsObject(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_WIRE)
{
    QPointF         NewPos;
    qreal           NewZValue;
    qint32          nCurrentPenStyle;
    qint32          nWireConnectedState;

    // Serialization: Load General GraphicItem Data
    DataStreamIn >> NewPos;
    DataStreamIn >> NewZValue;

    // Load In the Easy Stuff
    DataStreamIn >> m_WireIndex;
    DataStreamIn >> m_StartPoint;
    DataStreamIn >> m_EndPoint;
    DataStreamIn >> m_MiddleStartVertical1Point;
    DataStreamIn >> m_MiddleEndVertical1Point;
    DataStreamIn >> m_Current_x_VerticalDrawPoint;
    DataStreamIn >> m_AutomaticWireLinePositioning;
    DataStreamIn >> m_CurrentWireColor;
    DataStreamIn >> nCurrentPenStyle;
    DataStreamIn >> nWireConnectedState;
    m_CurrentPenStyle = (Qt::PenStyle)nCurrentPenStyle;
    m_WireConnectedState = (ConnectedState)nWireConnectedState;

    // Check to see if we use the Index from the datastream
    // (NewWireIndex == -1) or give this a wire a new index
    if (NewWireIndex >= 0) {
        m_WireIndex = NewWireIndex;
    }

    // Default this ware to not selected
    m_WireSelectedState = false;

    // Build the Wire Line Segments
    m_StartPointLine = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_STARTLINE, GetItemProperties(), this);
    m_MiddleVLine1   = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_MIDDLEVLINE1, GetItemProperties(), this);
    m_EndPointLine   = new GraphicItemWireLineSegment(GraphicItemWireLineSegment::SEGPOS_ENDLINE, GetItemProperties(), this);

    // Initialize the Graphical Pen for the wires
    UpdateWireLinePens();

    // Save off the line points
    m_PointToPointLine = QLineF(m_StartPoint, m_EndPoint);

    // Init variables that track when ports are touched
    m_StartPointCurrentPortConnected = NULL;
    m_EndPointCurrentPortConnected = NULL;

    // Now draw a initial line.  User will later move it via endpoint
    QLineF InitialLine(m_StartPoint, m_EndPoint);

    // And draw the initial 3 wires at the start/end points
    // User will later move it via endpoint
    m_StartPointLine->setLine(InitialLine);
    m_MiddleVLine1->setLine(InitialLine);
    m_EndPointLine->setLine(InitialLine);

    // Set the Z height (Wires are on top of Components, but lower than Text)
    setZValue(NewZValue);

    // Build the handles at the end of the lines, start them off as invisible
    m_StartPointHandle = new GraphicItemWireHandle(m_StartPoint, GraphicItemWireHandle::STARTPOINT, this);
    m_StartPointHandle->MakeVisible(false);
    m_EndPointHandle = new GraphicItemWireHandle(m_EndPoint, GraphicItemWireHandle::ENDPOINT, this);
    m_EndPointHandle->MakeVisible(false);

    // Turn off the selected state
    UpdateWireSelectedState(false);

    // Load the Component Properties
    GetItemProperties()->LoadData(DataStreamIn);

    // Set the Wire Index incase it changed via the parameter
    GetItemProperties()->SetPropertyValue(WIRE_PROPERTY_NUMBER, QString("%1").arg(m_WireIndex));
}

GraphicItemWire::~GraphicItemWire()
{
    DisconnectAllAttachedPorts();
}

void GraphicItemWire::SaveData(QDataStream& DataStreamOut)
{
    // Serialization: Save General GraphicItem Data
    DataStreamOut << pos();
    DataStreamOut << zValue();

    // Save off the Easy Stuff
    DataStreamOut << (qint32)m_WireIndex;
    DataStreamOut << m_StartPoint;
    DataStreamOut << m_EndPoint;
    DataStreamOut << m_MiddleStartVertical1Point;
    DataStreamOut << m_MiddleEndVertical1Point;
    DataStreamOut << m_Current_x_VerticalDrawPoint;
    DataStreamOut << m_AutomaticWireLinePositioning;
    DataStreamOut << m_CurrentWireColor;
    DataStreamOut << (qint32)m_CurrentPenStyle;
    DataStreamOut << (qint32)m_WireConnectedState;

    // Save the Wire Properties
    GetItemProperties()->SaveData(DataStreamOut);
}

void GraphicItemWire::UpdateStartPointPosition(const QPointF& NewPointLocation)
{
    // Save off the updated line point
    m_StartPoint = NewPointLocation;

    UpdatePointPositions(m_StartPointLine);
}

void GraphicItemWire::UpdateEndPointPosition(const QPointF& NewPointLocation)
{
    // Save off the updated line point
    m_EndPoint = NewPointLocation;

    UpdatePointPositions(m_EndPointLine);
}

void GraphicItemWire::SetPastePosition(int PasteOffset)
{
    // Set the offset of the vertical draw if it is enabled
    m_Current_x_VerticalDrawPoint += PasteOffset;

    // Offset the Start and End Point positions
    UpdateStartPointPosition(GetStartPoint() + QPointF(PasteOffset, PasteOffset));
    UpdateEndPointPosition(GetEndPoint() + QPointF(PasteOffset, PasteOffset));
}

void GraphicItemWire::DisconnectAllAttachedPorts()
{
    // Tell the ports that they are not connected to the wire anymore
    if (m_StartPointCurrentPortConnected != NULL) {
        m_StartPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
        m_StartPointCurrentPortConnected = NULL;
    }

    if (m_EndPointCurrentPortConnected != NULL) {
        m_EndPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
        m_EndPointCurrentPortConnected = NULL;
    }

    UpdatePortAndWireHandleSettings();
}

void GraphicItemWire::DisconnectPort(GraphicItemPort* Port, int MoveXOffset /*=0*/)
{
    // Figure out what port this is
    if (Port == m_StartPointCurrentPortConnected) {
        // Start Port
        // Tell the port that it is not connected to the wire
        m_StartPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
        m_StartPointCurrentPortConnected = NULL;

        UpdatePortAndWireHandleSettings();

        // See if we need to move the end point
        if (MoveXOffset != 0) {
            QPointF Offset(MoveXOffset, 0);
            UpdateStartPointPosition(m_StartPoint + Offset);
        }
    }

    if (Port == m_EndPointCurrentPortConnected) {
        // End Port

        // Tell the port that it is not connected to the wire
        m_EndPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
        m_EndPointCurrentPortConnected = NULL;

        UpdatePortAndWireHandleSettings();

        // See if we need to move the end point
        if (MoveXOffset != 0) {
            QPointF Offset(MoveXOffset, 0);
            UpdateEndPointPosition(m_EndPoint + Offset);
        }
    }
}

void GraphicItemWire::SetWireSelected(bool SelectedState)
{
    // Just select one of the child WireLineSegments and set its
    // select state, this will call back into this parent object
    // to setup change the colors, pens, line style, etc
    m_EndPointLine->setSelected(SelectedState);
}

void GraphicItemWire::HandleWireLineSegmentItemChange(const QPointF& NewPos, GraphicItemWireLineSegment* CallingWireLineSegment, GraphicsItemChange change, const QVariant& value)
{
    bool SelectedState;

    ///////////////////////////////////////////////////////////////////////
    // If any of Wire Line Segments change, they will call this routine

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
        UpdateWireSelectedState(SelectedState);
    }

    // Did the items position change (note: we don't allow the position to actually change),
    // we just want the offset the user provided to tell us how far to move the wire
    if (change == ItemPositionChange && scene()) {

        // Set this wire to user controlled
        CallingWireLineSegment->ClearAutoMoveFlag();

        // Turn off automatic wire positioning, now we use what was last set
        m_AutomaticWireLinePositioning = false;

        // Now redraw the wire line segments in the new position
        UpdateWireLineSegmentPositions(NewPos, CallingWireLineSegment);
    }
}

void GraphicItemWire::HandleWireLineSegmentMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)

    // When the mouse is released on a WireLineSegment (When updating its position)
    m_Current_x_VerticalDrawPoint = m_MiddleStartVertical1Point.x();
}

void GraphicItemWire::UpdatePointPositions(GraphicItemWireLineSegment* CallingWireLineSegment)
{
    int                   x;
    GraphicItemPort*      StartPointFoundPort = NULL;
    GraphicItemPort*      EndPointFoundPort = NULL;
    QList<QGraphicsItem*> StartPointItems;
    QList<QGraphicsItem*> EndPointItems;
    GraphicItemPort*      ptrPort;

    // Get a list of items at each of the points
    StartPointItems = scene()->items(m_StartPoint);
    EndPointItems = scene()->items(m_EndPoint);

    // Remove this GraphicItemWire Graphical Object from the list of Items
    if (StartPointItems.count() && StartPointItems.first() == this) {
       StartPointItems.removeFirst();
    }
    if (EndPointItems.count() && EndPointItems.first() == this) {
       EndPointItems.removeFirst();
    }

    // Search all the Items at the START POINT to see if it is a GraphicItemPort Object
    // and find any unconnected PORTS at the point that are not connected to this wire
    for (x = 0; x < StartPointItems.count(); x++) {
        // Cast the item to a GraphicItemPort (or NULL if this item is not a port)
        ptrPort = qgraphicsitem_cast<GraphicItemPort*>(StartPointItems.at(x));
        if (ptrPort != NULL) {
            // See if this port does not have a connection, or is connected to this wire's start handle
            if ((ptrPort->GetPortConnectedWireHandle() == NULL) ||
                (ptrPort->GetPortConnectedWireHandle() == m_StartPointHandle)) {
                // This point is touching the connection point of the port
                // Check to see if the port is configured, if yes, when we can use it
                if (ptrPort->IsPortConfigured() == true) {
                    // Save off the found port for later analysis
                    StartPointFoundPort = ptrPort;

                    // Snap the point to the center of the port
                    m_StartPoint = mapFromItem(ptrPort->parentItem(), ptrPort->GetConnectionPoint());
                }
            }
        }
    }

    // Search all the Items at the END POINT to see if it is a GraphicItemPort Object
    // and find any unconnected PORTS at the point that are not connected to this wire
    for (x = 0; x < EndPointItems.count(); x++) {
        // Cast the item to a GraphicItemPort (or NULL if this item is not a port)
        ptrPort = qgraphicsitem_cast<GraphicItemPort*>(EndPointItems.at(x));
        if (ptrPort != NULL) {
            // See if this port does not have a connection, or is connected to this wire's end handle,
            if ((ptrPort->GetPortConnectedWireHandle() == NULL) ||
                (ptrPort->GetPortConnectedWireHandle() == m_EndPointHandle)) {
                // This point is touching the connection point of the port
                // Check to see if the port is configured, if yes, when we can use it
                if (ptrPort->IsPortConfigured() == true) {
                    EndPointFoundPort = ptrPort;

                    // Snap the point to the center of the port
                    m_EndPoint = mapFromItem(ptrPort->parentItem(), ptrPort->GetConnectionPoint());
                }
            }
        }
    }

    // Now we check if the start point was previously connected to a port
    // We are doing this to see if we are being disconnected from a
    // previous port via a wire handle (selected state)
    if ((m_StartPointCurrentPortConnected != NULL) && (m_WireSelectedState == true)) {
        // A port was previously connected, check to see if the start point
        // is now pointing to a blank spot (ignore different ports)
        if ((m_StartPointCurrentPortConnected != StartPointFoundPort) && (StartPointFoundPort == NULL)) {
            // The last port connected does not match the found port,  so mark the
            // last port as not connected and flag the start point as not connected
            m_StartPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
            m_StartPointCurrentPortConnected = NULL;
        }
    }

    // Now we check if the end point was previously connected to a port
    // We are doing this to see if we are being disconnected from a
    // previous port via a wire handle (selected state)
    if ((m_EndPointCurrentPortConnected != NULL) && (m_WireSelectedState == true)) {
        // A port was previously connected, check to see if the end point
        // is now pointing to a blank spot (ignore different ports)
        if ((m_EndPointCurrentPortConnected != EndPointFoundPort) && (EndPointFoundPort == NULL)) {
            // The last port connected does not match the found port,  so mark the
            // last port as not connected and flag the end point as not connected
            m_EndPointCurrentPortConnected->SetPortConnectedWireHandle(NULL);
            m_EndPointCurrentPortConnected = NULL;
        }
    }

    // If the start/end point is not connected, then connect it to the found port
    // the found port may be NULL (meaning empty space)
    if (m_StartPointCurrentPortConnected == NULL) {
        // Save the Connected Port touched by each point (it might be NULL)
        m_StartPointCurrentPortConnected = StartPointFoundPort;
    }
    if (m_EndPointCurrentPortConnected == NULL) {
        // Save the Connected Port touched by each point (it might be NULL)
        m_EndPointCurrentPortConnected = EndPointFoundPort;
    }

    UpdatePortAndWireHandleSettings();

    // Finally Move the Line's position
    m_PointToPointLine = QLineF(m_StartPoint, m_EndPoint);

    UpdateWireLineSegmentPositions(QPointF(0, 0), CallingWireLineSegment);
}

void GraphicItemWire::UpdatePortAndWireHandleSettings()
{
    // Figure out the connection state of the wire based upon what handles are touching a port
    m_WireConnectedState = NO_CONNECTIONS;  // Default
    if ((m_StartPointCurrentPortConnected != NULL) && (m_EndPointCurrentPortConnected != NULL)) {
        m_WireConnectedState = FULL_CONNECTION;
    } else if (m_StartPointCurrentPortConnected != NULL) {
        m_WireConnectedState = STARTPOINT_CONNECTED;
    } else if (m_EndPointCurrentPortConnected != NULL) {
        m_WireConnectedState = ENDPOINT_CONNECTED;
    }

    // Set the wire line color based upon a full connection (if both Handles are touching a port)
    if (m_WireConnectedState == FULL_CONNECTION) {
        m_CurrentWireColor = WIRE_COLOR_FULL_CONNECTED;
    } else {
        m_CurrentWireColor = WIRE_COLOR_DISCONNECTED;
    }
    UpdateWireLinePens();

    // Now decide if we need to show wire Handles for the endpoints of the line
    // and tie the wire handles to any connected ports
    m_StartPointHandle->UpdatePosition(m_StartPoint);
    m_EndPointHandle->UpdatePosition(m_EndPoint);
    m_StartPointHandle->SetWireHandleConnectedPort(m_StartPointCurrentPortConnected);
    m_EndPointHandle->SetWireHandleConnectedPort(m_EndPointCurrentPortConnected);

    UpdateWireSelectedState(m_WireSelectedState);
}

void GraphicItemWire::UpdateWireLineSegmentPositions(const QPointF& NewPos, GraphicItemWireLineSegment* CallingWireLineSegment)
{
    qreal Delta_x;
    qreal MidPoint_x;
    qreal VerticalDrawPoint_x;

    // If we are doing automatic line positioning figure out the mid line points
    // and save their positions
    if (m_AutomaticWireLinePositioning == true) {
        // Figure out the Horizontal Middle point between the two Absolute End Points
        Delta_x = (m_EndPoint.x() - m_StartPoint.x()) / 2;
        MidPoint_x = m_StartPoint.x() + Delta_x;

        // Save off the draw point
        m_Current_x_VerticalDrawPoint = MidPoint_x;
    }

    // Figure out if the the Vertical Line is Controlled by Program or by User
    // If this is a middle Wire Line Segment, and the its AutoMoveFlag is set, then figure out the
    // middle position, otherwise, the user set the position and we use that
    if ((CallingWireLineSegment->IsWireLineAMiddleSegment() == true) && (CallingWireLineSegment->GetAutoMoveFlag() == true)) {
        // Changed via Program Control
        VerticalDrawPoint_x = m_Current_x_VerticalDrawPoint;
    } else {
        // Changed User Control
        VerticalDrawPoint_x = m_Current_x_VerticalDrawPoint + NewPos.x();
    }

    // Compute where the lines go
    m_MiddleStartVertical1Point = QPointF(VerticalDrawPoint_x, m_StartPoint.y());
    m_MiddleEndVertical1Point = QPointF(VerticalDrawPoint_x, m_EndPoint.y());

    QLineF Line1 = QLineF(m_StartPoint, m_MiddleStartVertical1Point);
    QLineF Line2 = QLineF(m_MiddleStartVertical1Point, m_MiddleEndVertical1Point);
    QLineF Line3 = QLineF(m_MiddleEndVertical1Point, m_EndPoint);

    m_StartPointLine->setLine(Line1);
    m_MiddleVLine1->setLine(Line2);
    m_EndPointLine->setLine(Line3);

    // Set the project dirty
    emit ItemWireSetProjectDirty();
}

void GraphicItemWire::UpdateWireSelectedState(bool SelectedState)
{
    m_WireSelectedState = SelectedState;

    // Figure out what to do based on the selected state
    if (SelectedState == true) {
        // Change the Style of the pen
        m_CurrentPenStyle = WIRE_STYLE_SELECTED;

        // Set the Z height (Wires are on top of Components, but lower than Text)
        UpdateWireLineZOrder(WIRE_SELECTED_ZVALUE);

    } else {
        // Change the Style of the pen
        m_CurrentPenStyle = WIRE_STYLE_DESELECTED;

        // Set the Z height (Wires are below Components)
        UpdateWireLineZOrder(WIRE_DESELECTED_ZVALUE);
    }
    UpdateWireLinePens();

    // Enable the Wire Handles as appropriate
    m_StartPointHandle->MakeVisible((m_StartPointCurrentPortConnected == NULL) || m_WireSelectedState);
    m_EndPointHandle->MakeVisible((m_EndPointCurrentPortConnected == NULL) || m_WireSelectedState);
}

void GraphicItemWire::UpdateWireLineZOrder(qreal z)
{
    // Set z value on parent also sets the value for children wire line segments
    setZValue(z);
}

void GraphicItemWire::UpdateWireLinePens()
{
    m_CurrentPen = QPen(m_CurrentWireColor, WIRE_PEN_WIDTH, m_CurrentPenStyle, Qt::RoundCap, Qt::RoundJoin);

    m_StartPointLine->setPen(m_CurrentPen);
    m_MiddleVLine1->setPen(m_CurrentPen);
    m_EndPointLine->setPen(m_CurrentPen);
}

QPainterPath GraphicItemWire::shape() const
{
    // Provide a accurate outline of the object
    // Used for collision algorithms
    QPainterPath path;

    // Return the path of the WireLineSegments
    path.addPath(m_StartPointLine->shape());
    path.addPath(m_MiddleVLine1->shape());
    path.addPath(m_EndPointLine->shape());

    return path;
}

QRectF GraphicItemWire::boundingRect() const
{
    // boundingRect() is used for re-drawing the object, and
    // also for rough collision detection.  We have to implement this
    // routine since is QGraphicsItem::boundingRect is pure virtual.

    QRectF RtnRect;

    // Return the union of the WireLineSegments
    RtnRect = RtnRect.united(m_StartPointLine->boundingRect());
    RtnRect = RtnRect.united(m_MiddleVLine1->boundingRect());
    RtnRect = RtnRect.united(m_EndPointLine->boundingRect());

    return RtnRect;
}

void GraphicItemWire::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,  QWidget* widget /*=0*/)
{
    // Painting for this object is actually done by the WireSegments, but we have
    // to implement this routine since is QGraphicsItem::paint is pure virtual.

    Q_UNUSED(option)
    Q_UNUSED(widget)
    Q_UNUSED(painter)
}

QVariant GraphicItemWire::itemChange(GraphicsItemChange change, const QVariant& value)
{
    bool    SelectedState;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphicItemWire::PropertyChanged(QString& PropName, QString& NewPropValue)
{
    Q_UNUSED(PropName)
    Q_UNUSED(NewPropValue)

    // Set the project dirty
    emit ItemWireSetProjectDirty();
}

