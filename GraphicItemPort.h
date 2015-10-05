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

#ifndef GRAPHICITEMPORT_H
#define GRAPHICITEMPORT_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"
#include "GraphicItemWire.h"
#include "GraphicItemWireHandle.h"
#include "PortInfoData.h"

// Forward declarations to allow compile
class GraphicItemWire;
class GraphicItemWireHandle;

////////////////////////////////////////////////////////////

class GraphicItemPort : public QObject, public QGraphicsEllipseItem, public GraphicItemBase
{
    Q_OBJECT

public:
    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_PORT };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemPort(PortInfoData* ParentPortInfo, QGraphicsItem* parent = 0);
    virtual ~GraphicItemPort();

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

    // Port Names
    QString GetSSTInfoPortName() {return m_SSTInfoPortName;}
    QString GetConfiguredPortName() {return m_ConfiguredPortName;}

    // Port Physical Position Settings
    void SetPortPosition(qreal CompEdge_x, qreal CompEdge_y, bool UpdateInitialPoint = true);

    // Port Side Settings
    ComponentSide_enum  GetAssignedComponentSide();
    int                 GetAssignedComponentSideSequence();

    // Controlling Data from Parents
    PortInfoData* GetParentPortInfoData() {return m_ParentPortInfo;}

    // Dynamic Port Information
    bool IsPortConfigured() {return m_PortIsConfigured;}
    bool IsPortDynamic() {return m_PortIsDynamic;}
    void SetPortConfigured(bool ConfiguredFlag, int DynamicSequenceID);
    bool IsPortConnectedToWire() {return (GetPortConnectedWire() != NULL);}
    int  GetPortDynamicSequenceID() {return m_PortDynamicSequenceID;}

    // Graphic Port Linked List Control
    GraphicItemPort* GetNextPeerDynamicPort() {return m_NextPeerDynamicPort;}
    void SetNextPeerDynamicPort(GraphicItemPort* NextPort) {m_NextPeerDynamicPort = NextPort;}

    // Control for Connection to wires
    QPointF GetConnectionPoint() const {return m_ConnectionPoint;}
    void SetPortConnectedWireHandle(GraphicItemWireHandle* ptrHandle);
    GraphicItemWireHandle* GetPortConnectedWireHandle() {return m_PortConnectedHandle;}
    GraphicItemWire* GetPortConnectedWire();
    void DisconnectPortFromWire(int MoveXOffset = 0);

    // Control for Moving Port Location on Component
    QPointF GetInitialPoint() const {return m_InitialPoint;}
    void    SetInitialPoint(QPointF NewPoint) {m_InitialPoint = NewPoint;}

signals:
    // Message to the Parent Scene
    void ItemPortSetProjectDirty();
    void ItemPortMovedPosition(GraphicItemPort*);

private:
    void UpdatePortPosition();

private:
    // Something changed on the Port
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // Handle Changes to the Ports Properties
    void PropertyChanged(QString& PropName, QString& NewPropValue);

private:
    PortInfoData*            m_ParentPortInfo;
    QString                  m_PortName;
    QString                  m_PortDesc;
    QString                  m_ConfiguredPortName;    // Name of port after it is Configured
    QString                  m_SSTInfoPortName;       // Name of port from SSTInfo
    QString                  m_SSTInfoPortOriginalName;

    QString                  m_DynamicPortControllingPropertyName;

    bool                     m_PortIsConfigured;      // Flag that identifies if port is fully Configured
    bool                     m_PortIsDynamic;         // Flag that identifies if port is a Dynamic Port (has a %d)
    int                      m_PortDynamicSequenceID; // Port Sequence in the Dynamic Group
    GraphicItemPort*         m_NextPeerDynamicPort;

    QColor                   m_PortColor;
    QPen                     m_PortLinePen;
    QPointF                  m_StartPoint;
    QPointF                  m_EndPoint;
    QPointF                  m_ConnectionPoint;
    QPointF                  m_InitialPoint;
    QRectF                   m_PortSizeRect;
    QFont                    m_PortNameFont;

    QGraphicsLineItem*       m_PortLine;
    QGraphicsSimpleTextItem* m_PortNameText;
    QGraphicsRectItem*       m_UnconfiguredPortMarker;

    GraphicItemWireHandle*   m_PortConnectedHandle;
};

#endif // GRAPHICITEMPORT_H




