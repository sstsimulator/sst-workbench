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

#include "PortInfoData.h"

///////////////////////////////////////////////////////////////////////////////

PortInfoData::PortInfoData(SSTInfoDataPort* SSTInfoPort, PortInfoData::ComponentSide ComponentSide)
{
    // NOTE: We do not save the pointer to the SSTInfoDataPort as
    //       The SSTInfo can be changed with a new import and could cause
    //       This object to be orphaned or point to incorrect data.
    //       Therefore when then object is created, it must grab everything
    //       it needs from the SSTInfoDataPort in one shot.

    CommonSetup();

    // Port Name information
    m_SSTInfoPortName = SSTInfoPort->GetPortName();
    m_SSTInfoPortOriginalName = SSTInfoPort->GetPortName();
    m_SSTInfoPortDesc = SSTInfoPort->GetPortDesc();
    m_SSTInfoPortValidEvents = SSTInfoPort->GetPortValidEvents();

    // Set What side of the component is this port assigned to
    m_AssignedComponentSide = ComponentSide;
    m_AssignedComponentSideSequence = 0;
    m_DynamicPortControllingParameterName = "";

    // Detirmine if Port is Static or Dynamic
    DetirminePortStaticOrDynamic();
}

void PortInfoData::DetirminePortStaticOrDynamic()
{
    int     Index1;
    int     Index2;
    QString ParamName;
    QString NewPortName;

    // Search The Port Name for NOT containing a "%d" or a "%("
    // that will indicate that it is a Static Port
    if ((m_SSTInfoPortName.contains("%d") == false) && (m_SSTInfoPortName.contains("%(") == false) ) {
        // This is a Static Port; The configured name is the same as the SSTInfoPortName
        m_PortIsDynamic = false;
        m_PortIsConfigured = true;
        m_ConfiguredPortName = m_SSTInfoPortName;
        return;
    }

    // If we get here, then this is a Dynamic Port, but we still have to figure a few things out.
    // Search The Port Name for a "%d" that will indicate that it is a Dynamic Port with no
    // Associated parameter
    if (m_SSTInfoPortName.contains("%d") == true) {
        // This is a Dynamic Port; We Need to identify that the port is unconfigured
        m_PortIsDynamic = true;
        m_PortIsConfigured = false;
        m_ConfiguredPortName = PORTINFO_UNCONFIGURED + m_SSTInfoPortName;
        return;
    }

    // If we get here, then this is a Dynamic Port, but it has a assocated Parameter
    // (a Component Parameter). we need to crack the Parameter from the port name
    Index1 = m_SSTInfoPortName.indexOf("%(", 0);
    Index2 = m_SSTInfoPortName.indexOf(")d", Index1);
    // Make sure we found both items
    if ((Index1 >= 0) && (Index2 > Index1)) {
        // Extract the Parameter Name
        ParamName = m_SSTInfoPortName.mid(Index1 + 2, Index2 - Index1 - 2);
        m_DynamicPortControllingParameterName = ParamName;

        // Create the New Port Name With the Parameter stripped out
        NewPortName = m_SSTInfoPortName.left(Index1+1) + m_SSTInfoPortName.mid(Index2 + 1);

        m_PortIsDynamic = true;
        m_PortIsConfigured = false;
        // Change the Port Name
        m_SSTInfoPortName = NewPortName;
        m_ConfiguredPortName = PORTINFO_UNCONFIGURED + m_SSTInfoPortName;

        return;
    } else {
        // Fall back and say the port is static
        m_PortIsDynamic = false;
        m_PortIsConfigured = true;
        m_ConfiguredPortName = m_SSTInfoPortName;
        return;
    }
}

void PortInfoData::CommonSetup()
{
    // Init the Dynamic Port data
    m_DynamicPortTotalInstances = 0;
    m_DynamicPortCreatedInstances = 0;
    m_StartingGraphicalPort = NULL;

    m_PortLatencyValuesList.clear();
    m_PortCommentsList.clear();
}

void PortInfoData::SetPortLatencyValue(int Index, QString LatencyValue)
{
    // Get the Number of Items in the list
    int count = m_PortLatencyValuesList.count();

    // If Index is less than the count update the current value
    if (Index <  count) {
        m_PortLatencyValuesList[Index] = LatencyValue;
    } else {
        // Add Additional entries as necessary; then set the value of the new object
        for (int x = count-1; x < Index; x++) {
            m_PortLatencyValuesList.append(QString("0"));
        }
        m_PortLatencyValuesList[Index] = LatencyValue;
    }
}

QString PortInfoData::GetPortLatencyValue(int Index)
{
    if (Index <  m_PortLatencyValuesList.count()) {
        return m_PortLatencyValuesList[Index];
    } else {
        return "0";
    }
}

void PortInfoData::SetPortComment(int Index, QString Comment)
{
    // Get the Number of Items in the list
    int count = m_PortCommentsList.count();

    // If Index is less than the count update the current value
    if (Index <  count) {
        m_PortCommentsList[Index] = Comment;
    } else {
        // Add Additional entries as necessary; then set the value of the new object
        for (int x = count-1; x < Index; x++) {
            m_PortCommentsList.append(QString(""));
        }
        m_PortCommentsList[Index] = Comment;
    }
}

QString PortInfoData::GetPortComment(int Index)
{
    if (Index <  m_PortCommentsList.count()) {
        return m_PortCommentsList[Index];
    } else {
        return "";
    }
}

void PortInfoData::SaveData(QDataStream& DataStreamOut)
{
    // Serialization - Save the Data
    // Save the PortInfoData
    DataStreamOut << m_ConfiguredPortName;
    DataStreamOut << m_SSTInfoPortName;
    DataStreamOut << m_SSTInfoPortOriginalName;
    DataStreamOut << m_SSTInfoPortDesc;
    DataStreamOut << m_SSTInfoPortValidEvents;

    DataStreamOut << m_DynamicPortControllingParameterName;
    DataStreamOut << m_PortIsConfigured;
    DataStreamOut << m_PortIsDynamic;
    DataStreamOut << (qint32)m_DynamicPortTotalInstances;
    DataStreamOut << (qint32)m_AssignedComponentSide;
    DataStreamOut << (qint32)m_AssignedComponentSideSequence;

    DataStreamOut << m_PortLatencyValuesList;
    DataStreamOut << m_PortCommentsList;
}

PortInfoData::PortInfoData(QDataStream& DataStreamIn)
{
    qint32 n_AssignedComponentSide;

    // Serialization - Load the Data
    CommonSetup();

    // Load the PortInfoData
    DataStreamIn >> m_ConfiguredPortName;
    DataStreamIn >> m_SSTInfoPortName;
    DataStreamIn >> m_SSTInfoPortOriginalName;
    DataStreamIn >> m_SSTInfoPortDesc;
    DataStreamIn >> m_SSTInfoPortValidEvents;

    DataStreamIn >> m_DynamicPortControllingParameterName;
    DataStreamIn >> m_PortIsConfigured;
    DataStreamIn >> m_PortIsDynamic;
    DataStreamIn >> m_DynamicPortTotalInstances;
    DataStreamIn >> n_AssignedComponentSide;
    DataStreamIn >> m_AssignedComponentSideSequence;

    DataStreamIn >> m_PortLatencyValuesList;
    DataStreamIn >> m_PortCommentsList;

    m_AssignedComponentSide = (PortInfoData::ComponentSide)n_AssignedComponentSide;
}
