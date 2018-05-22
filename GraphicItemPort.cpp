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

#include "GraphicItemPort.h"

////////////////////////////////////////////////////////////

GraphicItemPort::GraphicItemPort(PortInfoData* ParentPortInfo, QGraphicsItem* parent /*=0*/)
    : QObject(), QGraphicsEllipseItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_PORT)
{
    // Save off the SSTInfoPort Information
    m_ParentPortInfo = ParentPortInfo;
    m_PortName = ParentPortInfo->GetSSTInfoPortName();
    m_PortDesc = ParentPortInfo->GetSSTInfoPortDesc();
    m_SSTInfoPortOriginalName = ParentPortInfo->GetSSTInfoPortOriginalName();
    m_DynamicPortControllingParameterName = ParentPortInfo->GetDynamicPortContollingParameterName();

    // Initial Port Name information
    m_SSTInfoPortName = m_PortName;
    m_ConfiguredPortName = m_PortName;

    // Init the Dynamic Port data
    m_NextPeerDynamicPort = NULL;

    // Initial Port Color
    m_PortColor = PORT_COLOR_DISCONNECTED;

    // Setup initial default start and end points for the port until defined by component
    m_StartPoint = QPointF(0, 0);
    m_EndPoint = QPointF(10, 10);

    // Identify the point where we connect wires to
    m_ConnectionPoint = m_EndPoint;
    m_PortConnectedHandle = NULL;  // No wires connected

    // Identify the starting location of the point
    m_InitialPoint = m_EndPoint;

    // Set the initial size of the Ellipse for the port circle
    m_PortSizeRect = QRectF(PORT_ELLIPSE_SIZE);

    // Create the pen for the port line
    m_PortLinePen = QPen(PORT_LINE_COLOR, PORT_LINE_WIDTH);

    // Now create the port Line
    m_PortLine = new QGraphicsLineItem(this);
    m_PortLine->setPen(m_PortLinePen);
    m_PortLine->setFlag(QGraphicsItem::ItemStacksBehindParent);  // Draw Line behind Ellipse

    // Build the Unconfigured Marker display
    m_UnconfiguredPortMarker = new QGraphicsRectItem(this);
    m_UnconfiguredPortMarker->setRect(PORT_UNCONFIG_MARKER_SIZE);
    m_UnconfiguredPortMarker->setBrush(PORT_UNCONFIG_MARKER_COLOR);
    m_UnconfiguredPortMarker->setRotation(PORT_UNCONFIG_MARKER_ROTATION);

    // Set the Font for Port Name Text
    m_PortNameFont = QApplication::font();            // Get the default application font
    m_PortNameFont.setPointSize(PORT_NAME_FONT_SIZE);

    // Build the Text for the port
    m_PortNameText = new QGraphicsSimpleTextItem(this);
    m_PortNameText->setText(m_ConfiguredPortName);
    m_PortNameText->setFont(m_PortNameFont);

    // Set Graphic parameters
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    // Now set the Properties for this Port
    GetItemProperties()->AddProperty(PORT_PROPERTY_CONFIGURED_NAME, m_ConfiguredPortName, "Port Name", ItemProperty::READONLY, false);
    if (ParentPortInfo->IsPortDynamic() == true) {
        // Show the Generic Name only if the port is Dynamic
        GetItemProperties()->AddProperty(PORT_PROPERTY_GENERIC_NAME, m_SSTInfoPortName, "Generic Port Name", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(PORT_PROPERTY_ORIG_NAME, m_SSTInfoPortOriginalName, "Original Port Name", ItemProperty::READONLY, false);
    }
    GetItemProperties()->AddProperty(PORT_PROPERTY_DESCRIPTION, m_PortDesc, "Port Description", ItemProperty::READONLY, false);
    if (ParentPortInfo->IsPortDynamic() == true) {
        GetItemProperties()->AddProperty(PORT_PROPERTY_PORTTYPE, PORT_TYPE_DYNAMIC, "Port Type", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(PORT_PROPERTY_CONTROLPARAM, m_DynamicPortControllingParameterName, "Component Parameter Controlling this Dynamic Port", ItemProperty::READONLY, false);
    } else {
        GetItemProperties()->AddProperty(PORT_PROPERTY_PORTTYPE, PORT_TYPE_STATIC, "Port Type", ItemProperty::READONLY, false);
    }
    GetItemProperties()->AddProperty(PORT_PROPERTY_COMMENT, "0", "Comment on this Port", ItemProperty::READWRITE, false);
    GetItemProperties()->AddProperty(PORT_PROPERTY_LATENCY, "0", "The Latency for this Port", ItemProperty::READWRITE, false);

    m_PortDynamicSequenceID = 0;

    // Search The Port Name for a "%d" that will indicate that it is a Dynamic Port
    if (m_SSTInfoPortName.contains("%d") == true) {
        // This is a Dynamic Port; We Need to identify that the port is unconfigured
        m_PortIsDynamic = true;
        SetPortConfigured(false, 0);
    } else {
        // This is a Normal Port; The configured name is the same as the SSTInfoPortName
        m_PortIsDynamic = false;
        SetPortConfigured(true, 0);
    }

    UpdatePortPosition();
}

GraphicItemPort::~GraphicItemPort()
{
    // Disconnect any wires
    DisconnectPortFromWire();
}

void GraphicItemPort::SaveData(QDataStream& DataStreamOut)
{
    Q_UNUSED(DataStreamOut)
    // PORT DATA IS SAVED BY PARENT PORT INFO (m_ParentPortInfo)
}

void GraphicItemPort::SetPortPosition(qreal CompEdge_x, qreal CompEdge_y, bool UpdateInitialPoint/*=true*/)
{
    // Save off the start point in the parents coordinate system
    m_StartPoint = QPointF(CompEdge_x, CompEdge_y);

    // Figure out the End Point based on the StartPoint and what side we are drawing on
    if (GetAssignedComponentSide() == PortInfoData::SIDE_LEFT)
    {
        m_EndPoint = QPointF(CompEdge_x - PORT_LINE_LENGTH, CompEdge_y);
    } else {
        m_EndPoint = QPointF(CompEdge_x + PORT_LINE_LENGTH, CompEdge_y);
    }

    // The Connection point is the same as the end point
    // This is were we draw the Connection Ellipse
    m_ConnectionPoint = m_EndPoint;

    // See if we are to update the Initial Point position
    if (UpdateInitialPoint == true) {
        m_InitialPoint = m_EndPoint;
    }

    UpdatePortPosition();
}

PortInfoData::ComponentSide GraphicItemPort::GetAssignedComponentSide()
{
    return m_ParentPortInfo->GetAssignedComponentSide();
}

int GraphicItemPort::GetAssignedComponentSideSequence()
{
    return m_ParentPortInfo->GetAssignedComponentSideSequence();
}

void GraphicItemPort::SetPortConfigured(bool ConfiguredFlag, int DynamicSequenceID)
{
    QString ParentUserName;
    QString ParentLatencyValue;
    QString ParentComment;

    // Change the Configured name and users name as necessary
    if (ConfiguredFlag == false) {
        // Set the Port as NOT configured (ONLY DYNAMIC PORTS ARE SET THIS WAY)
        m_PortIsConfigured = false;
        m_PortDynamicSequenceID = 0;

        m_ConfiguredPortName = PORT_UNCONFIGURED_STRING + m_PortName;

        // Set the Visual Text that Displays with the Port to always display the Configured Port Name
        m_PortNameText->setText(m_ConfiguredPortName);
    } else {
        // Set the Port as configured (DYNAMIC AND STATIC PORTS)
        m_PortIsConfigured = true;
        m_PortDynamicSequenceID = DynamicSequenceID;

        // Now build the Configured Port Name by replacing the %d (if found) with the DynamicSequenceID
        m_ConfiguredPortName = m_PortName;
        m_ConfiguredPortName.replace("%d", QString("%1").arg(DynamicSequenceID));

        // Set the visual text
        m_PortNameText->setText(m_ConfiguredPortName);
    }

    // Change the Configured Port Name Property
    GetItemProperties()->GetProperty(PORT_PROPERTY_CONFIGURED_NAME)->SetValue(m_ConfiguredPortName);

    // Set the Comment from the Parent
    ParentComment = m_ParentPortInfo->GetPortComment(DynamicSequenceID);
    GetItemProperties()->GetProperty(PORT_PROPERTY_COMMENT)->SetValue(ParentComment, false);

    // Set the Latency Value from the Parent
    ParentLatencyValue = m_ParentPortInfo->GetPortLatencyValue(DynamicSequenceID);
    GetItemProperties()->GetProperty(PORT_PROPERTY_LATENCY)->SetValue(ParentLatencyValue, false);
}

void GraphicItemPort::SetPortConnectedWireHandle(GraphicItemWireHandle* ptrHandle)
{
    // Set the color of the port based upon if it is connected to a Wire Handle
    if (ptrHandle != NULL){
        m_PortColor = PORT_COLOR_CONNECTED;
    } else {
        m_PortColor = PORT_COLOR_DISCONNECTED;
    }
    setBrush(m_PortColor);

    m_PortConnectedHandle = ptrHandle;
}

GraphicItemWire* GraphicItemPort::GetPortConnectedWire()
{
    // Return the wire that is connected to this port
    if (m_PortConnectedHandle != NULL) {
        return m_PortConnectedHandle->GetConnectedWire();
    } else {
        return NULL;
    }
}

void GraphicItemPort::DisconnectPortFromWire(int MoveXOffset /*=0*/)
{
    // Tell the wire to disconnect this port
    if (GetPortConnectedWire() != NULL) {
        GetPortConnectedWire()->DisconnectPort(this, MoveXOffset);
    }
}

void GraphicItemPort::UpdatePortPosition()
{
    qreal LineStartEdgeOffset;
    qreal LineEndEdgeOffset;

    // Set the Ellipse to indicate the port size and color, and its position
    setRect(m_PortSizeRect);
    setBrush(m_PortColor);
    setPos(m_ConnectionPoint);

    // Draw the Line from the edge of the component to the edge of the Port Ellipse
    if (GetAssignedComponentSide() == PortInfoData::SIDE_LEFT) {
        LineEndEdgeOffset = PORT_LINE_END_EDGE_OFFSET;
        LineStartEdgeOffset = -PORT_LINE_START_EDGE_OFFSET;
    } else {
        LineEndEdgeOffset = -PORT_LINE_END_EDGE_OFFSET;
        LineStartEdgeOffset = PORT_LINE_START_EDGE_OFFSET;
    }

    // Note: Since the Line is a child of the Ellipse, its coordinate system is relative to
    //       the Ellipse, we actually want it relative to the Ellipse Parent (component)
    m_PortLine->setLine(mapFromParent(m_StartPoint).x() + LineStartEdgeOffset, mapFromParent(m_StartPoint).y(),
                         mapFromParent(m_EndPoint).x() + LineEndEdgeOffset, mapFromParent(m_EndPoint).y());

    // Set Port Name Text Position
    if (GetAssignedComponentSide() == PortInfoData::SIDE_LEFT)
    {
        m_PortNameText->setPos(mapFromParent(m_EndPoint).x() - 5, mapFromParent(m_EndPoint).y() + PORT_NAME_Y_OFFSET);
    } else {
        m_PortNameText->setPos(mapFromParent(m_StartPoint).x() + 2, mapFromParent(m_StartPoint).y() + PORT_NAME_Y_OFFSET);
    }

    // Setup the unconfigured Marker; This will display on top of the
    // normal port ellipse if the port is not configured
    m_UnconfiguredPortMarker->setPos(mapFromParent(m_ConnectionPoint));
    m_UnconfiguredPortMarker->setVisible(!m_PortIsConfigured);
}

QVariant GraphicItemPort::itemChange(GraphicsItemChange change, const QVariant& value)
{
    bool SelectedState;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    // See if the port has moved position on the scene
    if (change == ItemScenePositionHasChanged && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // Now see if there is a Wire Handle Attached to this port
        GraphicItemWireHandle* ptrWireHandle = GetPortConnectedWireHandle();
        if (ptrWireHandle != NULL) {
            GraphicItemWire*  ptrWire = GetPortConnectedWire();

            // Depending upon what end of the wire this handle is attached to, update that point
            if (ptrWireHandle->GetWireHandlePointPosition() == GraphicItemWireHandle::STARTPOINT) {
                ptrWire->UpdateStartPointPosition(newPos);
            } else {
                ptrWire->UpdateEndPointPosition(newPos);
            }
        }
    }

    // If the position has moved, then mark the Project as dirty
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        emit ItemPortSetProjectDirty();
        emit ItemPortMovedPosition(this);
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphicItemPort::PropertyChanged(QString& PropName, QString& NewPropValue)
{
    if (PropName == PORT_PROPERTY_LATENCY) {
        // Tell the Parent what the new Latency Value is
        m_ParentPortInfo->SetPortLatencyValue(m_PortDynamicSequenceID, NewPropValue);
    }

    if (PropName == PORT_PROPERTY_COMMENT) {
        // Tell the Parent what the new Comment is
        m_ParentPortInfo->SetPortComment(m_PortDynamicSequenceID, NewPropValue);
    }

    // Set the project dirty
    emit ItemPortSetProjectDirty();
}


