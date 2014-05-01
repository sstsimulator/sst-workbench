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

#include "GraphicItemComponent.h"

///////////////////////////////////////////////////////////////////////////////

GraphicItemComponent::GraphicItemComponent(int ComponentIndex, SSTInfoDataComponent* SSTInfoComponent, QMenu* ItemMenu, QColor& ComponentFillColor, const QPointF& startPos, QGraphicsItem* parent /*=0*/)
    : QObject(), QGraphicsRectItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_COMPONENT)
{
    int                      x;
    int                      NumPortsFromSSTInfo = 0;
    PortInfoData*            NewPortInfoData = NULL;
    int                      LeftSideSequence = 1;
    int                      RightSideSequence = 1;


    // NOTE: We do not save the pointer to the SSTInfoDataComponent as
    //       The SSTInfo can be changed with a new import and could cause
    //       This object to be orphaned or point to incorrect data.
    //       Therefore when then object is created, it must grab everything
    //       it needs from the SSTInfoDataComponent in one shot.

    CommonInit();

    // Save Data Constructor
    m_ComponentType = SSTInfoComponent->GetComponentType();
    m_ComponentIndex = ComponentIndex;
    m_ItemMenu = ItemMenu;

    // Init names of the component
    m_ParentElementName = SSTInfoComponent->GetParentElementName();
    m_ComponentUserName = SSTInfoComponent->GetComponentName();
    m_ComponentUniqueName = SSTInfoComponent->GetComponentName();
    m_ComponentName = SSTInfoComponent->GetComponentName();
    m_ComponentDesc = SSTInfoComponent->GetComponentDesc();
    m_ComponentTypeName = SSTInfoDataComponent::GetComponentTypeName(m_ComponentType);

    m_NumAllowedInstances = SSTInfoComponent->GetAllowedNumberOfInstances();

    // Figure out what the display name of the component is
    CreateComponentDisplayName();

    // Set The Components fill color
    m_ComponentFillColor = ComponentFillColor;
    setBrush(m_ComponentFillColor);

    // Figure out how many ports we have to deal with
    NumPortsFromSSTInfo = SSTInfoComponent->GetNumOfPorts();

    // Since we get no information from SSTInfo on what side a port is on, we will
    // assume a fairly even distribution of ports on a side.
    // Due to this, the left side will always have 0 or port ports than the right
    // side. The user will be able to move the ports around as they desire.
    m_NumPortsLeftSide = ceil(double(NumPortsFromSSTInfo) / 2);
    m_NumPortsRightSide = floor(double(NumPortsFromSSTInfo) / 2);

    // Now Create a Port Info Data object for each of the Port and assign them to a side
    for (x = 0; x < NumPortsFromSSTInfo; x++) {
        if (x < m_NumPortsLeftSide) {
            NewPortInfoData = new PortInfoData(SSTInfoComponent->GetPort(x), PortInfoData::SIDE_LEFT);
            NewPortInfoData->SetAssignedComponentSideSequence(LeftSideSequence++);
        } else {
            NewPortInfoData = new PortInfoData(SSTInfoComponent->GetPort(x), PortInfoData::SIDE_RIGHT);
            NewPortInfoData->SetAssignedComponentSideSequence(RightSideSequence++);
        }
        m_PortInfoDataArray.append(NewPortInfoData);
    }

    // Create the Graphical Object for the Component Display Name
    m_ComponentGraphicDisplayName = new QGraphicsSimpleTextItem(this);
    m_ComponentGraphicDisplayTypeName = new QGraphicsSimpleTextItem(this);

    // Create the Initial Visual layout of the Component
    CreateInitiaLVisualLayoutOfComponent();

    // Set the Component's Starting Position
    setPos(startPos);

    // Set the Z height of the Component
    setZValue(COMPONENT_ZVALUE);

    // Now set the Properties for this Component
    if (m_ComponentType != SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION) {
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_USERNAME, m_ComponentUserName, "User Assigned Name", ItemProperty::READWRITE, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_UNIQUENAME, m_ComponentUniqueName, "Unique Name For Component", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_COMPPARENTELEM, m_ParentElementName, "Parent Element Name", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_COMPNAME, m_ComponentName, "Base Component Name", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_INDEX, QString("%1").arg(m_ComponentIndex), "Component Index", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_TYPE, m_ComponentTypeName, "Type of Component", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_DESCRIPTION, m_ComponentDesc, "Component Description", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_COMMENT, "", "Comment on this Component", ItemProperty::READWRITE, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_RANK, "", "Rank of Component (int)", ItemProperty::READWRITE, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_WEIGHT, "", "Weight of Component (float)", ItemProperty::READWRITE, false);
    } else {
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_COMPNAME, m_ComponentName, "Component Name", ItemProperty::READONLY, false);
//      GetItemProperties()->AddProperty(COMPONENT_PROPERTY_INDEX, QString("%1").arg(m_ComponentIndex), "Component Index", ItemProperty::READONLY, false);
        GetItemProperties()->AddProperty(COMPONENT_PROPERTY_COMMENT, "", "Comment on this Component", ItemProperty::READWRITE, false);
    }

    // Add the parameters
    for (int x = 0; x < SSTInfoComponent->GetNumOfParams(); x++) {
        GetItemProperties()->AddProperty(SSTInfoComponent->GetParam(x)->GetParamName(), SSTInfoComponent->GetParam(x)->GetDefaultValue(), SSTInfoComponent->GetParam(x)->GetParamDesc());
    }

    // Perform Common Setup
    CommonSetup();
}

GraphicItemComponent::GraphicItemComponent(QDataStream& DataStreamIn, QMenu* ItemMenu, QGraphicsItem* parent /*=0*/)
    : QObject(), QGraphicsRectItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_COMPONENT)
{
    int             x;
    qint32          nComponentType;
    QPointF         NewPos;
    qreal           NewZValue;
    qint32          NumPorts;
    PortInfoData*   NewPortInfoData = NULL;

    // NOTE: We do not save the pointer to the SSTInfoDataComponent as
    //       The SSTInfo can be changed with a new import and could cause
    //       This object to be orphaned or point to incorrect data.
    //       Therefore when then object is created, it must grab everything
    //       it needs from the SSTInfoDataComponent in one shot.

    CommonInit();

    // Serialization: Load General GraphicItem Data
    DataStreamIn >> NewPos;
    DataStreamIn >> NewZValue;

    // Load In the Easy Stuff
    DataStreamIn >> nComponentType;
    DataStreamIn >> m_ComponentIndex;
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_ComponentUserName;
    DataStreamIn >> m_ComponentUniqueName;
    DataStreamIn >> m_ComponentName;
    DataStreamIn >> m_ComponentDesc;
    DataStreamIn >> m_ComponentTypeName;
    DataStreamIn >> m_NumAllowedInstances;
    DataStreamIn >> m_ComponentFillColor;
    DataStreamIn >> NumPorts;

    // Set the Type and Menu
    m_ComponentType = (SSTInfoDataComponent::ComponentType)nComponentType;
    m_ItemMenu = ItemMenu;

    // Figure out what the display name of the component is
    CreateComponentDisplayName();

    // Set The Components fill color
    setBrush(m_ComponentFillColor);

    // Load the Port Information
    for (x = 0; x < NumPorts; x++) {
        NewPortInfoData = new PortInfoData(DataStreamIn);
        m_PortInfoDataArray.append(NewPortInfoData);
    }

    // Create the Graphical Object for the Component Display Name
    m_ComponentGraphicDisplayName = new QGraphicsSimpleTextItem(this);
    m_ComponentGraphicDisplayTypeName = new QGraphicsSimpleTextItem(this);

    // Create and Update the Visual Layout of the Component
    CreateInitiaLVisualLayoutOfComponent();
    UpdateVisualLayoutOfComponent();

    // Set all the text info
    setPos(NewPos);
    setZValue(NewZValue);

    // Load the Component Properties
    GetItemProperties()->LoadData(DataStreamIn);

    // Perform Common Setup
    CommonSetup();
}

GraphicItemComponent::~GraphicItemComponent()
{
    int            x;
    int            count;
    PortInfoData*  PortInfoData;

    count = m_PortInfoDataArray.count();
    // Clean up the allocated Port Info Array
    for (x = 0; x < count; x++) {
        PortInfoData = m_PortInfoDataArray.at(0);
        m_PortInfoDataArray.removeAt(0);
        delete PortInfoData;
    }
}

void GraphicItemComponent::SetComponentIndex(int NewIndex)
{
    m_ComponentIndex = NewIndex;

    // Reset the Display Name & Unique Name
    CreateComponentDisplayName();

    // Set the appropriate properties
    GetItemProperties()->SetPropertyValue(COMPONENT_PROPERTY_INDEX, QString("%1").arg(m_ComponentIndex));
    GetItemProperties()->SetPropertyValue(COMPONENT_PROPERTY_UNIQUENAME, m_ComponentUniqueName);
}

void GraphicItemComponent::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialization: Save General GraphicItem Data
    DataStreamOut << pos();
    DataStreamOut << zValue();

    // Save off the Easy Stuff
    DataStreamOut << (qint32)m_ComponentType;
    DataStreamOut << (qint32)m_ComponentIndex;
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_ComponentUserName;
    DataStreamOut << m_ComponentUniqueName;
    DataStreamOut << m_ComponentName;
    DataStreamOut << m_ComponentDesc;
    DataStreamOut << m_ComponentTypeName;
    DataStreamOut << (qint32)m_NumAllowedInstances;
    DataStreamOut << m_ComponentFillColor;

    // Save the number of ports and then save all the ports
    DataStreamOut << (qint32)m_PortInfoDataArray.count();
    for (x = 0; x < m_PortInfoDataArray.count(); x++) {
        m_PortInfoDataArray[x]->SaveData(DataStreamOut);
    }

    // Save the Component Properties
    GetItemProperties()->SaveData(DataStreamOut);
}

QString GraphicItemComponent::GetComponentButtonIconImageName(SSTInfoDataComponent::ComponentType componentType)
{
    // STATIC FUNCTION THAT RETURNS A ICON THAT REPRESENTS THE GENERAL COMPONENT TYPE

    QString rtnString;

    // Provide a resource string that represents the Icon Image of this component
    switch (componentType){
        case SSTInfoDataComponent::COMP_UNCATEGORIZED          : rtnString =  ":/images/ComponentUncategorized.png" ; break;
        case SSTInfoDataComponent::COMP_PROCESSOR              : rtnString =  ":/images/ComponentProcessor.png" ; break;
        case SSTInfoDataComponent::COMP_MEMORY                 : rtnString =  ":/images/ComponentMemory.png" ; break;
        case SSTInfoDataComponent::COMP_NETWORK                : rtnString =  ":/images/ComponentNetwork.png" ; break;
        case SSTInfoDataComponent::COMP_SYSTEM                 : rtnString =  ":/images/ComponentSystem.png" ; break;
        case SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION: rtnString =  ":/images/ComponentSSTConfig.png" ; break;
        default                                                : rtnString =  ":/images/ComponentUncategorized.png" ; break;
    }
    return rtnString;
}

void GraphicItemComponent::UpdateVisualLayoutOfComponent()
{
    int                            YLeftOffset = 0;
    int                            YRightOffset = 0;
    int                            AssignedYOffset = 0;
    qreal                          AssignedXPos = 0;
    int                            x;
    GraphicItemPort*               GraphicalPort = NULL;
    PortInfoData::ComponentSide    PortAssignedSide;
    int                            PortSideSequence;
    int                            LeftCurrentSequenceCount = 1;
    int                            RightCurrentSequenceCount = 1;
    int                            LeftGraphicalPortCount = 0;
    int                            RightGraphicalPortCount = 0;
    int                            LeftPhysicalSidePortSequence = 1;
    int                            RightPhysicalSidePortSequence = 1;
    int                            NumGraphicalDynPorts;
    QVector<int>                   PhysicalPortSideSequenceArray;
    QString                        FinalComponentDisplayName;

    // THIS ROUTINE UPDATES THE DISPLAY OF THE COMPONENT.  IT ASSUMES
    // THAT PORTS (STATIC OR DYNAMIC) HAVE ALREADY BEEN CREATED.  IF A PORT IS STATIC
    // IT WILL BASICALLY IGNORE IT (IT MAY CHANGE ITS GRAPHICAL POSITION AS NECESSARY).
    // IF THE PORT IS DYNAMIC, IT WILL FIGURE OUT IF IT NEEDS TO ADD/REMOVE ANY
    // GRAPHICAL PORTS AND SET THE GRAPHICAL PORT POSITION

    // Clear the Graphical Port Arrays and Num Ports on a side
    m_GraphicalPortArrayLeftSide.clear();
    m_GraphicalPortArrayRightSide.clear();
    m_NumPortsLeftSide = 0;
    m_NumPortsRightSide = 0;

    ///////////////////////////////////////////
    // FIRST - RE-CONFIGURE ALL GRAPHICAL PORTS AS NECESSARY
    // Update the Configuration of each Dynamic Port (add/remove any graphical ports).
    // This will update the m_GraphicalPortArray
    // NOTE: ALL DYNAMIC PORTS WILL BE IN ORDER FROM FIRST PORT_0 to PORT_N IN THE ARRAY
    //       THIS IS IMPORTANT WHEN WE ARE FIGURING OUT ITS PHYSICAL POSITION.
    ReconfigureAllGraphicalPorts();

    // SECOND - FIGURE OUT THE NUMBER OF PORTS TO A SIDE
    // Look at all the ports in the GraphicalPortArray.  Increment the
    // side assigned to it.
    for (x = 0; x < m_GraphicalPortArray.count(); x++) {
        GraphicalPort = m_GraphicalPortArray.at(x);
        PortAssignedSide = GraphicalPort->GetAssignedComponentSide();

        // Increment the number of visual ports for the appropriate side
        if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
            m_NumPortsLeftSide++;
        } else {
            m_NumPortsRightSide++;
        }
    }

    ///////////////////////////////////////////
    // SECOND - UGLY BRUTE FORCE METHOD TO DECIDE WHERE THE GRAPHICAL PORTS
    // LIVE IN THE PHYSICAL SEQUENCE ON LEFT AND RIGHT SIDES.
    // NOTE: IT IS ASSUMED THAT ALL DYNAMIC PORTS WILL BE IN SEQUENTIAL
    //       LOCATIONS IN THE m_GraphicalPortArray
    // xxxGraphicalPortCount       = Counts the number of Graphical ports on the left
    //                               and right sides that we have analyzed so far
    // xxxCurrentSequenceCount     = Counts What left and right side sequences we are
    //                               processing at the current time
    // xxxPhysicalSidePortSequence = Incrementing count of the right and left side physical
    //                               position that this port should go on.
    // PhysicalPortSideSequenceArray = Array that Identifies what is the actual physical
    //                                 sequence number for this port, the index is the
    //                                 same index as the GraphicalPortArray

    // Set the size of the Array
    PhysicalPortSideSequenceArray.resize(m_GraphicalPortArray.count());

    // Check all ports on the left and right side
    while ((LeftGraphicalPortCount <  m_NumPortsLeftSide) || (RightGraphicalPortCount <  m_NumPortsRightSide)) {
        // Walk through all the graphical ports in the array, over and over again
        // Until all ports on left and right side have been processed
        for (x = 0; x < m_GraphicalPortArray.count(); x++) {
            GraphicalPort = m_GraphicalPortArray.at(x);
            PortAssignedSide = GraphicalPort->GetAssignedComponentSide();
            PortSideSequence = GraphicalPort->GetAssignedComponentSideSequence();
            NumGraphicalDynPorts = GraphicalPort->GetParentPortInfoData()->GetNumCreatedInstances();

            if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
                if (PortSideSequence == LeftCurrentSequenceCount) {
                    // We found a graphical port that matches the current sequence.
                    // We only increment the CurrentSequenceCount (all dynamic ports hold the same sequence count)
                    // We increment the GraphicalPortCount by the number ot instances of the dynamic port
                    // If it is a static port, then we just add 1 to the PhysicalSidePortSequence
                    // If it is a Dynamic port, then we just add 1 to the PhysicalSidePortSequence for
                    //       each Dynamic Created instance.
                    if ((GraphicalPort->IsPortDynamic() == true) && (GraphicalPort->IsPortConfigured())) {
                        // Process as a Dynamic Port
                        LeftCurrentSequenceCount++;
                        LeftGraphicalPortCount += NumGraphicalDynPorts;
                        for (int y = 0; y < NumGraphicalDynPorts; y++) {
                            PhysicalPortSideSequenceArray[x + y] = LeftPhysicalSidePortSequence++;
                        }
                        break;
                    } else {
                        // Process as a Static Port (also dynamic ports that are not configured are like this)
                        LeftCurrentSequenceCount++;
                        LeftGraphicalPortCount++;
                        PhysicalPortSideSequenceArray[x] = LeftPhysicalSidePortSequence++;
                        break;  // Exit the for loop
                    }
                }
            }

            if (PortAssignedSide == PortInfoData::SIDE_RIGHT) {
                if (PortSideSequence == RightCurrentSequenceCount) {
                    // We found a graphical port that matches the current sequence.
                    // We only increment the CurrentSequenceCount (all dynamic ports hold the same sequence count)
                    // We increment the GraphicalPortCount by the number ot instances of the dynamic port
                    // If it is a static port, then we just add 1 to the PhysicalSidePortSequence
                    // If it is a Dynamic port, then we just add 1 to the PhysicalSidePortSequence for
                    //       each Dynamic Created instance.
                    if ((GraphicalPort->IsPortDynamic() == true) && (GraphicalPort->IsPortConfigured())) {
                        // Process as a Dynamic Port
                        RightGraphicalPortCount += NumGraphicalDynPorts;
                        RightCurrentSequenceCount++;
                        for (int y = 0; y < NumGraphicalDynPorts; y++) {
                            PhysicalPortSideSequenceArray[x + y] = RightPhysicalSidePortSequence++;
                        }
                        break;
                    } else {
                        // Process as a Static Port (also dynamic ports that are not configured are like this)
                        RightGraphicalPortCount++;
                        RightCurrentSequenceCount++;
                        PhysicalPortSideSequenceArray[x] = RightPhysicalSidePortSequence++;
                        break;  // Exit the for loop
                    }
                }
            }
        }
    }

    ///////////////////////////////////////////
    // THIRD - SETUP SIZE OF THE COMPONENT BOX AND SET IT'S TEXT
    // Setup the box dimensions that show the rectangle of the component
    SetComponentBoxDimensions(m_NumPortsLeftSide, m_NumPortsRightSide);

    // Check to see if We need to modify the Component name for Moving Ports
    if (m_MovingPortsMode == true) {
        m_ComponentGraphicDisplayName->setBrush(Qt::red);
        FinalComponentDisplayName = m_ComponentDisplayName + "\n" + COMPONENT_DISPLAY_MOVING_PORTS;
    } else {
        m_ComponentGraphicDisplayName->setBrush(Qt::black);
        FinalComponentDisplayName = m_ComponentDisplayName;
    }

    // Set the text and position of the component name
    m_ComponentGraphicDisplayName->setText(FinalComponentDisplayName);
    m_ComponentGraphicDisplayName->setPos(COMPONENT_LEFT_X, m_ComponentSideHeight);

    m_ComponentGraphicDisplayTypeName->setText(m_ComponentDisplayTypeName);
    m_ComponentGraphicDisplayTypeName->setPos(COMPONENT_LEFT_X + COMPONENT_TYPENAME_OFFSET, COMPONENT_TOP_Y);

    // FORTH - SET SIDE AND POSITION OF EACH GRAPHICAL PORT (STATIC AND DYNAMIC)
    // Look at all the ports in the GraphicalPortArray. Put the port
    // on the correct side in the correct position.
    for (x = 0; x < m_GraphicalPortArray.count(); x++) {
        GraphicalPort = m_GraphicalPortArray.at(x);
        PortAssignedSide = GraphicalPort->GetAssignedComponentSide();
        PortSideSequence = PhysicalPortSideSequenceArray[x];

        // Check to see if it is a left or right port we are going to draw, and set its offset
        if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
            // Drawing a Left Port, set the offset
            YLeftOffset = COMPONENT_PORTYOFFSET * PortSideSequence;
            AssignedYOffset = YLeftOffset;
            AssignedXPos = COMPONENT_LEFT_X;
        } else {
            // Drawing a Right Port, set the offset
            YRightOffset = COMPONENT_PORTYOFFSET * PortSideSequence;
            AssignedYOffset = YRightOffset;
            AssignedXPos = COMPONENT_RIGHT_X;
        }

        // Now set the Graphical Port Position
        GraphicalPort->SetPortPosition(AssignedXPos, AssignedYOffset);

        // Add the new Port to the left or right side array, and also set its index
        if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
            m_GraphicalPortArrayLeftSide.append(GraphicalPort);
        } else {
            m_GraphicalPortArrayRightSide.append(GraphicalPort);
        }
    }
}

void GraphicItemComponent::DisconnectPortsFromAllWires()
{
    // For each port, call its disconnect from wire
    for (int x = 0; x < m_GraphicalPortArray.count(); x++) {
        m_GraphicalPortArray.at(x)->DisconnectPortFromWire();
    }
}

int GraphicItemComponent::GetNumGraphicalPortsOnComponent()
{
    return m_GraphicalPortArray.count();
}

bool GraphicItemComponent::ComponentContainsDynamicPorts()
{
    // Look at all the PortInfoData on this component, if any of
    // the ports are dynamic, then this component has dynamic ports
    for (int x = 0; x < m_PortInfoDataArray.count(); x++) {
        if (m_PortInfoDataArray.at(x)->IsPortDynamic()) {
            return true;
        }
    }
    return false;
}

bool GraphicItemComponent::AreAnyComponentPortsConnectedToWires()
{
    // Look at all the Graphical Ports on this component, if any of
    // the ports has a wire connected, we return false
    for (int x = 0; x < m_GraphicalPortArray.count(); x++) {
        if (m_GraphicalPortArray.at(x)->GetPortConnectedWire() != NULL) {
            return true;
        }
    }
    return false;
}

void GraphicItemComponent::SetComponentFillColor(const QColor& color)
{
    // Set The Components fill color
    m_ComponentFillColor = color;
    setBrush(m_ComponentFillColor);
}


void GraphicItemComponent::SetMovingPortsMode(bool Flag)
{
    GraphicItemPort* Port;
    int              x;

    // Set the Port Moving Flag
    m_MovingPortsMode = Flag;

    // Go through all ports and set them moveable
    for (x = 0; x < m_GraphicalPortArray.count(); x++) {
        Port = m_GraphicalPortArray.at(x);
        Port->setFlag(QGraphicsItem::ItemIsMovable, Flag);
    }

    // Update the component
    UpdateVisualLayoutOfComponent();
}

void GraphicItemComponent::HandleItemPortModedPosition(GraphicItemPort* Port)
{
    QPointF                     NewPortPos;
    qreal                       UpdatedXPos;
    qreal                       UpdatedYPos;
    QPointF                     InitialPos;
    bool                        SwapSides = false;
    bool                        UpdateVisualLayout = false;
    PortInfoData::ComponentSide CurrentPortSide;
    int                         CurrentPortSideSequence;
    int                         NewPortSideSequence = 0;

    if (m_MovingPortsMode == true) {

        // Get the New Port Position, and initial updated locations
        NewPortPos = Port->pos();
        UpdatedXPos = NewPortPos.x();
        UpdatedYPos = NewPortPos.y();
        InitialPos = Port->GetInitialPoint();
        CurrentPortSide = Port->GetAssignedComponentSide();
        CurrentPortSideSequence = Port->GetAssignedComponentSideSequence();

        //qDebug() << "Port" << Port->GetConfiguredPortName() << "SIDE=" << Port->GetAssignedComponentSide() << "SEQ=" << Port->GetAssignedComponentSideSequence()<< "Pos=" << NewPortPos << "InitPT=" << InitialPos;

        // Check the Left side limits
        if (UpdatedXPos <= 0) {
            UpdatedXPos = COMPONENT_LEFT_X;
            if (CurrentPortSide == PortInfoData::SIDE_RIGHT) {
                SwapSides = true;
            }
        }
        // Check the Right side limits
        if (UpdatedXPos > 0) {
            UpdatedXPos = COMPONENT_RIGHT_X;
            if (CurrentPortSide == PortInfoData::SIDE_LEFT) {
                SwapSides = true;
            }
        }
        // Check the Top limits
        if (UpdatedYPos < COMPONENT_TOP_Y) {
            UpdatedYPos = COMPONENT_TOP_Y + 1; // Ensure that we dont violate Y offset and cause a sequence change
        }
        // Check the Bottom limits
        if (UpdatedYPos > m_ComponentSideHeight) {
            UpdatedYPos = m_ComponentSideHeight - 1; // Ensure that we dont violate Y offset and cause a sequence change
        }

        // X & Y SET WITHIN THEIR LIMITS, NOW FIGURE OUT THE NEW GRAPHICAL SEQUENCE

        if (SwapSides == true) {
            //qDebug() << "** SWAP SIDES";
            ReorderPortSequence(Port, CurrentPortSide, SwapSides, CurrentPortSideSequence, 0);
            UpdateVisualLayout = true;
        } else {
            // Check to see if the port has moved in Y direction enough to warrent an update of Sequence
            if (fabs(InitialPos.y() - UpdatedYPos) > COMPONENT_PORTYOFFSET) {

                // Check which way the user moved the port and update the sequence.
                // NOTE: We dont have to check for limits as we limit top/bottom limits above
                if (InitialPos.y() <= UpdatedYPos) {
                    // We increase the Sequence (User Moved the Port Down)
                    NewPortSideSequence = CurrentPortSideSequence + 1;
                }
                if (InitialPos.y() > UpdatedYPos) {
                    // We Descrease the Sequence (User Moved the Port Up)
                    NewPortSideSequence = CurrentPortSideSequence - 1;
                }

                //qDebug() << "** UPDATE SEQ: Old=" << CurrentPortSideSequence << "New=" << NewPortSideSequence;
                ReorderPortSequence(Port, CurrentPortSide, SwapSides, CurrentPortSideSequence, NewPortSideSequence);
                UpdateVisualLayout = true;
            }
        }

        if (UpdateVisualLayout == true) {
            // Update the Visual Layout of the component (which also updates the initial point)
            UpdateVisualLayoutOfComponent();
        } else {
            // Redraw the port at the new position, but dont update the inital point
            Port->SetPortPosition(UpdatedXPos, UpdatedYPos, false);
        }
    }
}

void GraphicItemComponent::ReorderPortSequence(GraphicItemPort* CurrentPort, PortInfoData::ComponentSide CurrentSide, bool SwapSides, int CurrentSeq, int NewSeq)
{
    int                      x;
    QList<GraphicItemPort*>* GraphicalPortArrayCurSide;
    QList<GraphicItemPort*>* GraphicalPortArrayNewSide;
    GraphicItemPort*         Port;
    int                      HighestSequenceProcessed;
    int                      PortSequence;

    // Figure out what side we are working on
    if (CurrentSide == PortInfoData::SIDE_LEFT) {
        GraphicalPortArrayCurSide = &m_GraphicalPortArrayLeftSide;
        GraphicalPortArrayNewSide = &m_GraphicalPortArrayRightSide;
    } else {
        GraphicalPortArrayCurSide = &m_GraphicalPortArrayRightSide;
        GraphicalPortArrayNewSide = &m_GraphicalPortArrayLeftSide;
    }

    if (SwapSides == false) {
        // Just a General Reorder of the sequence
        for (x = 0; x < GraphicalPortArrayCurSide->count(); x++) {
            Port = GraphicalPortArrayCurSide->at(x);
            // Check to see if the new port currently has the new Sequence number,
            if (Port->GetAssignedComponentSideSequence() == NewSeq) {
                // Swap the sequence numbers for this found Port and the CurrentPort
                Port->GetParentPortInfoData()->SetAssignedComponentSideSequence(CurrentSeq);
                CurrentPort->GetParentPortInfoData()->SetAssignedComponentSideSequence(NewSeq);
            }
        }
    } else {
        // We are going to move the Port's Component Side, and set the sequence
        // on the new side greater than the max seq on the new side.
        // Also we need to reduce all the sequences (fill the hole) of the old side
        // This works because the ports in the array are shown in increasing sequence values

        // First go though the Current side (before move), and fill the hole
        HighestSequenceProcessed = 0;
        for (x = 0; x < GraphicalPortArrayCurSide->count(); x++) {
            Port = GraphicalPortArrayCurSide->at(x);
            PortSequence = Port->GetAssignedComponentSideSequence();
            // Check to see if the Port currently has a Sequence number higher than
            // The Port we are moving, and is has not been processed.
            if ((PortSequence > CurrentSeq) && (PortSequence > HighestSequenceProcessed)) {
                // Mark this sequence number as processed
                HighestSequenceProcessed = PortSequence;
                // Reduce this Port's sequence number
                Port->GetParentPortInfoData()->SetAssignedComponentSideSequence(PortSequence - 1);
            }
        }

        // Second go though the New side and find the highest squence number.  Then move
        // the Current port to the new side and set the sequence number greater than the highest
        HighestSequenceProcessed = 0;
        for (x = 0; x < GraphicalPortArrayNewSide->count(); x++) {
            Port = GraphicalPortArrayNewSide->at(x);
            PortSequence = Port->GetAssignedComponentSideSequence();
            // Check to see if the HighestSequence is this port
            if (PortSequence > HighestSequenceProcessed) {
                // Mark this sequence number as the highest
                HighestSequenceProcessed = PortSequence;
            }
        }

        // Now move the Current Port and set its sequence number
        CurrentPort->GetParentPortInfoData()->SetAssignedComponentSideSequence(HighestSequenceProcessed + 1);
        if (CurrentPort->GetAssignedComponentSide() == PortInfoData::SIDE_LEFT) {
            CurrentPort->GetParentPortInfoData()->SetAssignedComponentSide(PortInfoData::SIDE_RIGHT);
        } else {
            CurrentPort->GetParentPortInfoData()->SetAssignedComponentSide(PortInfoData::SIDE_LEFT);
        }
    }
}

void GraphicItemComponent::CreateComponentDisplayName()
{
    QString CompDisplayName;
    QString CompTypeName;

    // Figure out The Component Type Name
    switch (m_ComponentType) {
        case SSTInfoDataComponent::COMP_UNCATEGORIZED:           CompTypeName = COMPONENT_DISPLAY_PREFIX_UNCAT; break;
        case SSTInfoDataComponent::COMP_PROCESSOR:               CompTypeName = COMPONENT_DISPLAY_PREFIX_PROCESSOR; break;
        case SSTInfoDataComponent::COMP_MEMORY:                  CompTypeName = COMPONENT_DISPLAY_PREFIX_MEMORY; break;
        case SSTInfoDataComponent::COMP_NETWORK:                 CompTypeName = COMPONENT_DISPLAY_PREFIX_NETWORK; break;
        case SSTInfoDataComponent::COMP_SYSTEM:                  CompTypeName = COMPONENT_DISPLAY_PREFIX_SYSTEM; break;
        case SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION: CompTypeName = COMPONENT_DISPLAY_PREFIX_SSTCONFIG; break;
        default:                                                 CompTypeName = COMPONENT_DISPLAY_PREFIX_ERROR; break;
    }

    // If the component name matches the user name, Full Path Name
    if (m_ComponentName == m_ComponentUserName) {
        CompDisplayName = m_ParentElementName;
        CompDisplayName += "." + m_ComponentUserName;
    } else {
        // Otherwise display the users Name only
        CompDisplayName = m_ComponentUserName;
    }

    // Special Case for SSTCONFIGURATION COMPONENT
    if (m_ComponentType == SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION) {
        CompDisplayName = m_ComponentName;
    }

    m_ComponentUserName = CompDisplayName;
    m_ComponentDisplayName = CompDisplayName;
    m_ComponentDisplayTypeName = CompTypeName;

    // Figure out the Component Unique Name
    m_ComponentUniqueName = m_ParentElementName + "." + m_ComponentName + "." + QString("%1").arg(m_ComponentIndex);
}

void GraphicItemComponent::CreateInitiaLVisualLayoutOfComponent()
{
    int                            YLeftOffset = 0;
    int                            YRightOffset = 0;
    int                            AssignedYOffset = 0;
    qreal                          AssignedXPos = 0;
    int                            x;
    GraphicItemPort*               NewPort;
    PortInfoData*                  PortInfo;
    PortInfoData::ComponentSide    PortAssignedSide;
    int                            PortSideSequence;

    // THIS ROUTINE CREATES THE INITIAL LAYOUT OF THE COMPONENT.  IT ASSUMES
    // THAT ALL PORTS (STATIC OR DYNAMIC) WILL CREATE A SINGLE GRAPHICAL
    // PORT OBJECT.  THE UpdateVisualLayoutOfComponent() ROUTINE WILL ADD/REMOVE
    // ANY GRAPHICAL PORTS REPRESENTING DYNAMIC PORTS AS NECESSARY.

    // Setup the box dimensions that show the rectangle of the component
    SetComponentBoxDimensions(m_NumPortsLeftSide, m_NumPortsRightSide);

    // Set the text and position of the component name
    m_ComponentGraphicDisplayName->setText(m_ComponentDisplayName);
    m_ComponentGraphicDisplayName->setPos(COMPONENT_LEFT_X, m_ComponentSideHeight);

    m_ComponentGraphicDisplayTypeName->setText(m_ComponentDisplayTypeName);
    m_ComponentGraphicDisplayTypeName->setPos(COMPONENT_LEFT_X + COMPONENT_TYPENAME_OFFSET, COMPONENT_TOP_Y);

    // Now Create the Graphical Ports, assign them to a side and set their position
    for (x = 0; x < m_PortInfoDataArray.count(); x++) {

        // Get the Port Info Data
        PortInfo = m_PortInfoDataArray.at(x);

        // Get what side to assign this port to
        PortAssignedSide = PortInfo->GetAssignedComponentSide();
        PortSideSequence = PortInfo->GetAssignedComponentSideSequence();

        // Check to see if it is a left or right port we are going to draw, and set its offset
        if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
            // Drawing a Left Port, set the offset
            // When Creating, Dynamic ports always have a single Graphical Port
            YLeftOffset = COMPONENT_PORTYOFFSET * PortSideSequence;
            AssignedYOffset = YLeftOffset;
            AssignedXPos = COMPONENT_LEFT_X;
        } else {
            // Drawing a Right Port, set the offset
            // When Creating, Dynamic ports always have a single Graphical Port
            YRightOffset = COMPONENT_PORTYOFFSET * PortSideSequence;
            AssignedYOffset = YRightOffset;
            AssignedXPos = COMPONENT_RIGHT_X;
        }

        // Create the port and append it to the port array
        NewPort = new GraphicItemPort(PortInfo, this);
        NewPort->SetPortPosition(AssignedXPos, AssignedYOffset);
        connect(NewPort, SIGNAL(ItemPortSetProjectDirty()), this, SIGNAL(ItemComponentSetProjectDirty()));
        connect(NewPort, SIGNAL(ItemPortMovedPosition(GraphicItemPort*)), this, SLOT(HandleItemPortModedPosition(GraphicItemPort*)));

        // Add this port to components Graphical Port Array
        m_GraphicalPortArray.append(NewPort);

        // Set the Port Info Starting Graphical Port
        PortInfo->SetStartingGraphicalPort(NewPort);

        // Add the new Port to the left or right side array, and also set its index
        if (PortAssignedSide == PortInfoData::SIDE_LEFT) {
            m_GraphicalPortArrayLeftSide.append(NewPort);
        } else {
            m_GraphicalPortArrayRightSide.append(NewPort);
        }
    }
}

void GraphicItemComponent::SetComponentBoxDimensions(int NumPortsLeftSide, int NumPortsRightSide)
{
    int LeftSideHeight;
    int RightSideHeight;

    // Compute the size of each side = (number of ports * offset distance between ports)
    // plus one extra offset distance between ports for the bottom area
    LeftSideHeight = (NumPortsLeftSide * COMPONENT_PORTYOFFSET) + COMPONENT_PORTYOFFSET;
    RightSideHeight = (NumPortsRightSide * COMPONENT_PORTYOFFSET) + COMPONENT_PORTYOFFSET;

    // Decide which is the larger height
    if (LeftSideHeight > RightSideHeight) {
        m_ComponentSideHeight = LeftSideHeight;
    } else {
        m_ComponentSideHeight = RightSideHeight;
    }

    // Now create a rectangle that has the dimensions of the box that represents the component
    m_ComponentRect = QRectF(COMPONENT_LEFT_X, COMPONENT_TOP_Y, COMPONENT_WIDTH, m_ComponentSideHeight);
    setRect(m_ComponentRect);
}

void GraphicItemComponent::ReconfigureAllGraphicalPorts()
{
    int                            x;
    GraphicItemPort*               StartGraphicalPort = NULL;
    GraphicItemPort*               CurrentGraphicalPort = NULL;
    GraphicItemPort*               NextGraphicalPort = NULL;
    GraphicItemPort*               NewGraphicalPort = NULL;
    PortInfoData*                  PortInfo;
    int                            TotalDynPorts;
    int                            CreatedDynPorts;
    bool                           PortConfiguredFlag;
    int                            CurrentPortCount;

    // Clear the Graphical Port Arrays and Num Ports on a side
    m_GraphicalPortArray.clear();

    // NOTE: TotalInstances, and CreatedInstances of a dynamic port = 0 when unconfigured, however
    //       there will always be 1 Graphic Port (StartGraphicalPort) created to provide a visual
    //       represendation of the Unconfigured Dynamic Port.  When TotalInstances is set = 1,
    //       We just used the pre-created Graphic Port (StartGraphicalPort) and mark it as configured.
    //       When TotalInstances is set > 1, we will add new Graphic Ports as necessary to the system.
    //       Each Graphic Port has a pointer (linked list) to its later peer Graphic Port.  If the
    //       pointer is NULL, this will be the last Graphic Port in the linked list.
    //       Created Instances reflects the temporary difference between what is currently created
    //       Graphic Ports and What the desired Total number of ports need to be (TotalInstances).

    // Now go through the PortInfoData Array.  For static ports, add its
    // associated Graphical Port to the GraphicalPortArray.  For Dynamic
    // Ports, See if we need to add or remove any Graphical Ports, then add
    // all the Graphical Ports to the GraphicalPortArray
    for (x = 0; x < m_PortInfoDataArray.count(); x++) {
        PortInfo = m_PortInfoDataArray.at(x);
        StartGraphicalPort = PortInfo->GetStartingGraphicalPort();

        // Is the Port Dynamic
        if (PortInfo->IsPortDynamic() == false) {

            // This is a Static Port, Just add the existing Graphical Port to the GraphicalPortArray
            m_GraphicalPortArray.append(StartGraphicalPort);

        } else {
            // This is a Dynamic Port

            // Setup usable variables for the Total and Created number of port instances
            TotalDynPorts = PortInfo->GetNumTotalInstances();
            CreatedDynPorts = PortInfo->GetNumCreatedInstances();

            // Check to see if we need to Add Graphical Ports
            if (TotalDynPorts > CreatedDynPorts) {
                // User has Increased the number of Dynamic Ports required, We need to make more ports
                // Check for special case of a Dynamic Port Total Size of 1;
                // If Total = 1, we dont need to do anything as a visible graphic port already exists
                if (TotalDynPorts > 1) {
                    // Find the last Graphical Port in the linked list of Graphical Ports
                    NextGraphicalPort = StartGraphicalPort;  // The Starting Port
                    CurrentGraphicalPort = StartGraphicalPort;
                    while (NextGraphicalPort != NULL) {
                        // Save the Pointer as the current port
                        CurrentGraphicalPort = NextGraphicalPort;
                        // Get the next port
                        NextGraphicalPort = CurrentGraphicalPort->GetNextPeerDynamicPort();
                    }

                    // NOTE: At this point we Know TotalDynPorts must be > 1, But if CreatedDynPorts = 0, we need to
                    //       1 less port (it does not reflect the StartGraphicalPort)
                    if (CreatedDynPorts == 0) {
                        CreatedDynPorts++;
                    }
                    while (TotalDynPorts > CreatedDynPorts) {
                        // Now Add Ports; Starting from the end one
                        // Create the port and append it to the port array
                        NewGraphicalPort = new GraphicItemPort(PortInfo, this);
                        // Set its position to 0, 0, and to the same side as the Last Dynamic Port
                        NewGraphicalPort->SetPortPosition(0, 0);
                        connect(NewGraphicalPort, SIGNAL(ItemPortSetProjectDirty()), this, SIGNAL(ItemComponentSetProjectDirty()));
                        connect(NewGraphicalPort, SIGNAL(ItemPortMovedPosition(GraphicItemPort*)), this, SLOT(HandleItemPortModedPosition(GraphicItemPort*)));

                        // Add the new port as a the next peer
                        CurrentGraphicalPort->SetNextPeerDynamicPort(NewGraphicalPort);
                        // Set the Current Port to point to the New Port and increment the number of Created Ports
                        CurrentGraphicalPort = NewGraphicalPort;
                        CreatedDynPorts++;
                    }
                }
            }

            // Check to see if we need to Remove Graphical Ports
            if (TotalDynPorts < CreatedDynPorts) {
                // User has Decreased the number of Dynamic Ports required, We need to remove some created ports

                // Check for special case of a Dynamic Port Created Size of 1;
                // If Created = 1, we dont need to do anything as a visible graphic port already exists
                if (CreatedDynPorts >= 1) {
                    // Start the Count
                    CurrentPortCount = 0; // The StartGraphicalPort is always present Start the Count at 0

                    // Find the last Graphical Port that we want to keep
                    NextGraphicalPort = StartGraphicalPort;  // The Starting Port
                    CurrentGraphicalPort = StartGraphicalPort;
                    while ((NextGraphicalPort != NULL) &&  (CurrentPortCount != TotalDynPorts)) {
                        CurrentPortCount++;

                        // Save the Pointer as the current port
                        CurrentGraphicalPort = NextGraphicalPort;
                        // Get the next port
                        NextGraphicalPort = CurrentGraphicalPort->GetNextPeerDynamicPort();
                    }

                    // Special Case if TotalDynPorts = 0
                    if (TotalDynPorts == 0) {
                        NextGraphicalPort = CurrentGraphicalPort->GetNextPeerDynamicPort();
                    }

                    // CurrentGraphicalPort is were we want to stop the Linked List
                    CurrentGraphicalPort->SetNextPeerDynamicPort(NULL);

                    // Delete any remaining ports starting at NextGraphicalPort
                    while ((NextGraphicalPort != NULL)) {
                        // Save the Pointer as the current port
                        CurrentGraphicalPort = NextGraphicalPort;

                        // Get the next port
                        NextGraphicalPort = CurrentGraphicalPort->GetNextPeerDynamicPort();

                        // Disconnect any wires from this Port and move them based upon the Port side
                        if (CurrentGraphicalPort->GetAssignedComponentSide() == PortInfoData::SIDE_LEFT) {
                            CurrentGraphicalPort->DisconnectPortFromWire(-PORT_DISCONNECT_MOVE_OFFSET);
                        } else {
                            CurrentGraphicalPort->DisconnectPortFromWire(PORT_DISCONNECT_MOVE_OFFSET);
                        }

                        // Delete the Graphical Port pointed to by CurrentGraphicalPort
                        delete CurrentGraphicalPort;
                    }
                }
            }

            // At this Point, we now have a number of created instances that match our total instances
            // Set the NumCreatedInstances = NumTotalInstances
            PortInfo->SetNumCreatedInstances(PortInfo->GetNumTotalInstances());

            // If the TotalNumberOfInstances is >= 1, then we need to mark this PortInfoData as configured
            PortConfiguredFlag = (TotalDynPorts >= 1);
            PortInfo->SetPortConfigured(PortConfiguredFlag);

            // Walk the linked list of Graphic Ports that are associated with this
            // dynamic port and add them to the GraphicalPortArray
            NextGraphicalPort = StartGraphicalPort;
            CurrentPortCount = 0; // The StartGraphicalPort is always present Start the Count at 0

            while (NextGraphicalPort != NULL) {
                // Add this Graphical Port to the GraphicalPortArray
                m_GraphicalPortArray.append(NextGraphicalPort);

                // See if the Port is configured
                if (PortInfo->GetNumCreatedInstances() > 0) {
                    // Mark the Graphical Port as configured and update its Count
                    NextGraphicalPort->SetPortConfigured(true, CurrentPortCount);
                } else {
                    // Mark the Graphical Port as NOT configured
                    NextGraphicalPort->SetPortConfigured(false, 0);
                }

                // Get the next port
                NextGraphicalPort = NextGraphicalPort->GetNextPeerDynamicPort();
                CurrentPortCount++;
            }
        }
    }
}

void GraphicItemComponent::CommonInit()
{
    // Clear the Port Information Arrays
    m_PortInfoDataArray.clear();
    m_GraphicalPortArray.clear();
    m_GraphicalPortArrayLeftSide.clear();
    m_GraphicalPortArrayRightSide.clear();

    // Set the Initial Position Move Delay
    m_MoveDelayDistance = MOUSEMOVE_DELAYPIXELS;

    // Flag setting Moving ports move = off
    m_MovingPortsMode = false;
}

void GraphicItemComponent::CommonSetup()
{
    // Set Graphic parameters for the Component
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void GraphicItemComponent::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Reset the Delay Distance
    m_MoveDelayDistance = MOUSEMOVE_DELAYPIXELS;
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicItemComponent::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    // Set this as the only item selected
    scene()->clearSelection();
    setSelected(true);

    // Execute the Menu
    m_ItemMenu->exec(event->screenPos());  // Blocking Call
    QGraphicsItem::contextMenuEvent(event);

    // When the popup menu occurs, this component has grabbed the mouse, but when
    // the menu is completed, the component does not fire a mouse release so the
    // mouse is not "ungrabbed".This can cause wierd results in things moving when
    // they shouldnt.  So we manually ungrab the mouse here (This was a hard one to
    // figure out!).
    ungrabMouse();
}

QVariant GraphicItemComponent::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QPointF NewPos;
    QPointF OldPos;
    bool    SelectedState;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    // Check to see if the Item has moved enough
    if (change == QGraphicsItem::ItemPositionChange) {
        NewPos = value.toPointF();
        OldPos = pos();
        if ((OldPos - NewPos).manhattanLength() < m_MoveDelayDistance) {
            // The mouse has not moved enough, keep the old position
            return QVariant(OldPos);
        } else {
            // Clear the delay distance until the mouse is released
            m_MoveDelayDistance = 0;
        }
    }

    // If the position has moved, then mark the Project as dirty
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        emit ItemComponentSetProjectDirty();
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphicItemComponent::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*=0*/)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // We have to paint our own line because normally when the line is selected,
    // a large bounding rectable is shown... This bounding rect is drawn
    // by default by the base class paint function, therefore we override it

    painter->setPen(QPen(Qt::black, 0, Qt::SolidLine));
    painter->setBrush(brush());
    painter->drawRect(rect());

    if (isSelected() == true) {
        HighlightSelected(painter, option);
    }

    // Uncomment the following to draw the Default selection outline when item is selected
    //QGraphicsRectItem::paint(painter, option, widget);
}

void GraphicItemComponent::HighlightSelected(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    const qreal penWidth = 0;             // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);

    // Draw the outline using the background color to basically erase it
    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect());

    // Now draw the new line over the old one
    painter->setPen(QPen(Qt::black, 2, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect());
}

void GraphicItemComponent::PropertyChanged(QString& PropName, QString& NewPropValue)
{
    int           x;
    int           NumPortInfo;
    PortInfoData* PortInfo;
    bool          UpdateLayout = false;

    // A Property Changed, see if it was the User Defined Name
    if (PropName == COMPONENT_PROPERTY_USERNAME) {
        m_ComponentUserName = NewPropValue;

        // Call SetPortConfigured to set names appropriately
        CreateComponentDisplayName();
        UpdateVisualLayoutOfComponent();
    }

    // Get the Number of PortInfo's
    NumPortInfo = m_PortInfoDataArray.count();

    // Walk through all PortInfo's to see if its controlling parameter matches this Property name
    for (x = 0; x < NumPortInfo; x++) {
        PortInfo = m_PortInfoDataArray.at(x);
        if (PortInfo->GetDynamicPortContollingParameterName() == PropName) {
            // This parameter controls the number of graphicPorts tied to this PortInfo
            PortInfo->SetNumTotalInstances(NewPropValue.toInt());
            UpdateLayout = true;
        }
    }

    if (UpdateLayout == true) {
        UpdateVisualLayoutOfComponent();
    }

    // Set the project dirty
    emit ItemComponentSetProjectDirty();
}

void GraphicItemComponent::DynamicPropertiesChanged(ItemProperties* ptrExistingProperties)
{
    emit ItemComponentRefreshPropertiesWindow(ptrExistingProperties);
}

