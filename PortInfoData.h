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

#ifndef PORTINFODATA_H
#define PORTINFODATA_H

#include "GlobalIncludes.h"

// Forward declarations to allow compile
class GraphicItemPort;
//#include "GraphicItemPort.h"

///////////////////////////////////////////////////////////////////////////////

class PortInfoData
{
public:
    // Constructor / Destructor
    PortInfoData(SSTInfoDataPort* SSTInfoPort, ComponentSide_enum ComponentSide);
    PortInfoData(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization

public:
    // Port Names & Description
    QString GetConfiguredPortName()      {return m_ConfiguredPortName;}
    QString GetSSTInfoPortName()         {return m_SSTInfoPortName;}
    QString GetSSTInfoPortOriginalName() {return m_SSTInfoPortOriginalName;}
    QString GetSSTInfoPortDesc()         {return m_SSTInfoPortDesc;}

    // Get the Controlling Property Name that controls Dynamic Ports (can be empty)
    QString GetDynamicPortContollingPropertyName() {return m_DynamicPortControllingPropertyName;}

    // Port Valid Events
    int GetNumSSTInfoPortValidEvents() {return m_SSTInfoPortValidEvents.count();}
    QString GetSSTInfoPortValidEvent(int Index) {return m_SSTInfoPortValidEvents.at(Index);}
    QStringList& GetSSTInfoPortValidEvents() {return m_SSTInfoPortValidEvents;}

    // Dynamic Port Information
    bool IsPortDynamic() {return m_PortIsDynamic;}
    bool IsPortConfigured() {return m_PortIsConfigured;}
    void SetPortConfigured(bool ConfigFlag) {m_PortIsConfigured = ConfigFlag;}

    // NOTE: The Difference between the TOTAL Number of Dynamic Port Instances and CREATED Number
    //       of Dynamic Port Instances identifies how many Ports need to be created or destroyed
    //       on the component.  Normally these two values will be equal.  They will be different
    //       when the user changes the TOTAL value to change the number of dynamic ports.

    // Control of the TOTAL Number of Instances that this Port will have
    int  GetNumTotalInstances() {return m_DynamicPortTotalInstances;}
    void SetNumTotalInstances(int NumInstances) {m_DynamicPortTotalInstances = NumInstances;}

    // Control of the CREATED Number of Instances that this Port currently has
    int  GetNumCreatedInstances() {return m_DynamicPortCreatedInstances;}
    void SetNumCreatedInstances(int NumInstances) {m_DynamicPortCreatedInstances = NumInstances;}

    // Control of what side of the component this port is assigned to
    ComponentSide_enum GetAssignedComponentSide() {return m_AssignedComponentSide;}
    void SetAssignedComponentSide(ComponentSide_enum CompSide) {m_AssignedComponentSide = CompSide;}

    // Get the side sequence that the part is assigned to
    int GetAssignedComponentSideSequence() {return m_AssignedComponentSideSequence;}
    void SetAssignedComponentSideSequence(int Seq) {m_AssignedComponentSideSequence = Seq;}

    // Dynamic Port Control
    // The Dynamic port is represented by 1 or more graphical ports (each have have a different name)
    // that are all tied to this PortInfoData.  Static Ports always have 1 graphical port.
    // The graphical ports are tied together via a linked list.
    void SetStartingGraphicalPort(GraphicItemPort* port) {m_StartingGraphicalPort = port;}
    GraphicItemPort* GetStartingGraphicalPort() {return m_StartingGraphicalPort;}

    // Latency and Comments
    void SetPortLatencyValue(int Index, QString LatencyValue);
    QString GetPortLatencyValue(int Index);

    void SetPortComment(int Index, QString Comment);
    QString GetPortComment(int Index);

    // Serialization of data
    void SaveData(QDataStream& DataStreamOut);

private:
    void DetirminePortStaticOrDynamic();
    void CommonSetup();

private:
    // Data from the SSTInfoPort
    QString            m_SSTInfoPortName;
    QString            m_SSTInfoPortOriginalName;
    QString            m_SSTInfoPortDesc;
    QStringList        m_SSTInfoPortValidEvents;

    QString            m_ConfiguredPortName;                   // Name of port after it is configured
    ComponentSide_enum m_AssignedComponentSide;                // Assigned Component Side
    int                m_AssignedComponentSideSequence;        // The order that this port should be drawn on its assigned side
    GraphicItemPort*   m_StartingGraphicalPort;                // Ptr to the first in the series of static or dynamic GraphicalItemPort objects

    QString            m_DynamicPortControllingPropertyName;  // The Property on the Component that controls the size of the dynamic port
    bool               m_PortIsConfigured;                     // Flag that identifies if port is fully Configured
    bool               m_PortIsDynamic;                        // Flag that identifies if port is a Dynamic Port (has a %d)
    int                m_DynamicPortTotalInstances;            // Total number of Instances of Dynamic Ports that the user as defined
    int                m_DynamicPortCreatedInstances;          // Total number of Instances of Dynamic Ports Currently Created; The difference
                                                               // from m_DynamicPortTotalInstances is the number of GraphicItemPorts that need
                                                               // to be created or destroyed.

    QStringList        m_PortLatencyValuesList;                // List of Latency Values for all ports (Static or Dynamic)
    QStringList        m_PortCommentsList;                     // List of Coments for all ports (Static or Dynamic)
};

#endif // PORTINFODATA_H
