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

#include "SSTInfoData.h"

//////////////////////////////////////////////////////////////////////

SSTInfoDataParam::SSTInfoDataParam(QString ParamName, QString DefaultValue /*=""*/)
{
    m_ParamName = ParamName;
    m_ParamDesc.clear();
    m_ParamDefaultValue = DefaultValue;
}

SSTInfoDataParam::~SSTInfoDataParam()
{
}

void SSTInfoDataParam::SaveData(QDataStream& DataStreamOut)
{
    // Serialize and Save the General Information
    DataStreamOut << m_ParamName;
    DataStreamOut << m_ParamDesc;
    DataStreamOut << m_ParamDefaultValue;
}

SSTInfoDataParam::SSTInfoDataParam(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParamName;
    DataStreamIn >> m_ParamDesc;
    DataStreamIn >> m_ParamDefaultValue;
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataPort::SSTInfoDataPort(QString PortName)
{
    m_PortName = PortName;
    m_PortDesc.clear();
    m_PortValidEvents.clear();
}

SSTInfoDataPort::~SSTInfoDataPort()
{
}

void SSTInfoDataPort::SaveData(QDataStream& DataStreamOut)
{
    // Serialize and Save the General Information
    DataStreamOut << m_PortName;
    DataStreamOut << m_PortDesc;
    DataStreamOut << m_PortValidEvents;
}

SSTInfoDataPort::SSTInfoDataPort(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialiation: Load The General Information
    DataStreamIn >> m_PortName;
    DataStreamIn >> m_PortDesc;
    DataStreamIn >> m_PortValidEvents;
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataComponent::SSTInfoDataComponent(QString ParentElementName, QString ComponentName, ComponentType_enum Type, int NumAllowedInstances /*=-1*/)
{
     m_ParentElementName = ParentElementName;
     m_ComponentName = ComponentName;
     m_ComponentDesc.clear();
     m_ComponentType = Type;
     m_ParamNameList.clear();
     m_PortNameList.clear();

     // Set the default number of allowed instances of this component,  -1 means unlimited
     m_AllowedNumberOfInstances = NumAllowedInstances;
}

SSTInfoDataComponent::~SSTInfoDataComponent()
{
    int x;
    int count;
    // Remove all Parameters to prevent memory leaks
    count = m_ParamNameList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ParamNameList.at(0);
        m_ParamNameList.removeAt(0);
    }
    m_ParamNameList.clear();

    // Remove all Ports to prevent memory leaks
    count = m_PortNameList.count();
    for (x = 0; x < count; x++)
    {
        delete m_PortNameList.at(0);
        m_PortNameList.removeAt(0);
    }
    m_PortNameList.clear();
}

QString SSTInfoDataComponent::GetComponentTypeName(ComponentType_enum Type)
{
    QString rtnString;

    switch (Type) {
        case COMP_UNCATEGORIZED:           rtnString = SSTINFO_COMPONENTTYPESTR_UNCATEGORIZED; break;
        case COMP_PROCESSOR:               rtnString = SSTINFO_COMPONENTTYPESTR_PROCESSOR; break;
        case COMP_MEMORY:                  rtnString = SSTINFO_COMPONENTTYPESTR_MEMORY; break;
        case COMP_NETWORK:                 rtnString = SSTINFO_COMPONENTTYPESTR_NETWORK; break;
        case COMP_SYSTEM:                  rtnString = SSTINFO_COMPONENTTYPESTR_SYSTEM; break;
        case COMP_SSTSTARTUPCONFIGURATION: rtnString = SSTINFO_COMPONENTTYPESTR_SSTCONFIG; break;
        default:                                                 rtnString = SSTINFO_COMPONENTTYPESTR_UNDEFINED; break;
    }
    return rtnString;
}

void SSTInfoDataComponent::AddParam(SSTInfoDataParam* NewParam)
{
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataComponent::AddParam(QString NewParamName, QString DefaultValue, QString Desc/*="*/)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName, DefaultValue);
    NewParam->SetParamDesc(Desc);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataComponent::AddParam(QString NewParamName)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataComponent::AddPort(SSTInfoDataPort* NewPort)
{
    m_PortNameList.append(NewPort);
}

void SSTInfoDataComponent::AddPort(QString NewPortName)
{
    SSTInfoDataPort* NewPort = new SSTInfoDataPort(NewPortName);
    m_PortNameList.append(NewPort);
}

void SSTInfoDataComponent::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_ComponentName;
    DataStreamOut << m_ComponentDesc;
    DataStreamOut << (qint32)m_ComponentType;
    DataStreamOut << (qint32)m_AllowedNumberOfInstances;

    // Serialize and Save the Counts
    DataStreamOut << (qint32)m_ParamNameList.count();
    DataStreamOut << (qint32)m_PortNameList.count();

    // Serialize and Save all the Parameters
    for (x = 0; x < m_ParamNameList.count(); x++)
    {
        m_ParamNameList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Ports
    for (x = 0; x < m_PortNameList.count(); x++)
    {
        m_PortNameList.at(x)->SaveData(DataStreamOut);
    }
}

SSTInfoDataComponent::SSTInfoDataComponent(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int     x;
    int     ParamCount;
    int     PortCount;
    qint32  nComponentType;

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_ComponentName;
    DataStreamIn >> m_ComponentDesc;
    DataStreamIn >> nComponentType;                    // Read Enum in as an Int
    m_ComponentType = (ComponentType_enum)nComponentType;   // Cast it to the Enum
    DataStreamIn >> m_AllowedNumberOfInstances;

    // Load the Counts
    DataStreamIn >> ParamCount;
    DataStreamIn >> PortCount;

    // Load all the Parameters
    for (x = 0; x < ParamCount; x++)
    {
        SSTInfoDataParam* newParam = new SSTInfoDataParam(DataStreamIn, ProjectFileVersion);
        AddParam(newParam);
    }

    // Load all the Ports
    for (x = 0; x < PortCount; x++)
    {
        SSTInfoDataPort* newPort = new SSTInfoDataPort(DataStreamIn, ProjectFileVersion);
        AddPort(newPort);
    }
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataIntrospector::SSTInfoDataIntrospector(QString ParentElementName, QString IntrospectorName)
{
     m_ParentElementName = ParentElementName;
     m_IntrospectorName = IntrospectorName;
     m_IntrospectorDesc.clear();
     m_ParamNameList.clear();
}

SSTInfoDataIntrospector::~SSTInfoDataIntrospector()
{
    int x;
    int count;

    // Remove all Parameters to prevent memory leaks
    count = m_ParamNameList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ParamNameList.at(0);
        m_ParamNameList.removeAt(0);
    }
    m_ParamNameList.clear();
}

void SSTInfoDataIntrospector::AddParam(SSTInfoDataParam* NewParam)
{
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataIntrospector::AddParam(QString NewParamName, QString DefaultValue, QString Desc/*="*/)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName, DefaultValue);
    NewParam->SetParamDesc(Desc);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataIntrospector::AddParam(QString NewParamName)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataIntrospector::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_IntrospectorName;
    DataStreamOut << m_IntrospectorDesc;

    // Serialize and Save the Counts
    DataStreamOut << (qint32)m_ParamNameList.count();

    // Serialize and Save all the Parameters
    for (x = 0; x < m_ParamNameList.count(); x++)
    {
        m_ParamNameList.at(x)->SaveData(DataStreamOut);
    }
}

SSTInfoDataIntrospector::SSTInfoDataIntrospector(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int     x;
    int     ParamCount;

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_IntrospectorName;
    DataStreamIn >> m_IntrospectorDesc;

    // Load the Counts
    DataStreamIn >> ParamCount;

    // Load all the Parameters
    for (x = 0; x < ParamCount; x++)
    {
        SSTInfoDataParam* newParam = new SSTInfoDataParam(DataStreamIn, ProjectFileVersion);
        AddParam(newParam);
    }
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataEvent::SSTInfoDataEvent(QString ParentElementName, QString EventName)
{
     m_ParentElementName = ParentElementName;
     m_EventName = EventName;
     m_EventDesc.clear();
}

SSTInfoDataEvent::~SSTInfoDataEvent()
{
}

void SSTInfoDataEvent::SaveData(QDataStream& DataStreamOut)
{
    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_EventName;
    DataStreamOut << m_EventDesc;
}

SSTInfoDataEvent::SSTInfoDataEvent(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_EventName;
    DataStreamIn >> m_EventDesc;
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataModule::SSTInfoDataModule(QString ParentElementName, QString ModuleName)
{
     m_ParentElementName = ParentElementName;
     m_ModuleName = ModuleName;
     m_ModuleDesc.clear();
}

SSTInfoDataModule::~SSTInfoDataModule()
{
    int x;
    int count;

    // Remove all Parameters to prevent memory leaks
    count = m_ParamNameList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ParamNameList.at(0);
        m_ParamNameList.removeAt(0);
    }
    m_ParamNameList.clear();
}

void SSTInfoDataModule::AddParam(SSTInfoDataParam* NewParam)
{
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataModule::AddParam(QString NewParamName, QString DefaultValue, QString Desc /*="*/)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName, DefaultValue);
    NewParam->SetParamDesc(Desc);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataModule::AddParam(QString NewParamName)
{
    SSTInfoDataParam* NewParam = new SSTInfoDataParam(NewParamName);
    m_ParamNameList.append(NewParam);
}

void SSTInfoDataModule::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_ModuleName;
    DataStreamOut << m_ModuleDesc;

    // Serialize and Save the Counts
    DataStreamOut << (qint32)m_ParamNameList.count();

    // Serialize and Save all the Parameters
    for (x = 0; x < m_ParamNameList.count(); x++)
    {
        m_ParamNameList.at(x)->SaveData(DataStreamOut);
    }
}

SSTInfoDataModule::SSTInfoDataModule(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int     x;
    int     ParamCount;

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_ModuleName;
    DataStreamIn >> m_ModuleDesc;

    // Load the Counts
    DataStreamIn >> ParamCount;

    // Load all the Parameters
    for (x = 0; x < ParamCount; x++)
    {
        SSTInfoDataParam* newParam = new SSTInfoDataParam(DataStreamIn, ProjectFileVersion);
        AddParam(newParam);
    }
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataPartitioner::SSTInfoDataPartitioner(QString ParentElementName, QString PartitionerName)
{
     m_ParentElementName = ParentElementName;
     m_PartitionerName = PartitionerName;
     m_PartitionerDesc.clear();
}

SSTInfoDataPartitioner::~SSTInfoDataPartitioner()
{
}

void SSTInfoDataPartitioner::SaveData(QDataStream& DataStreamOut)
{
    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_PartitionerName;
    DataStreamOut << m_PartitionerDesc;
}

SSTInfoDataPartitioner::SSTInfoDataPartitioner(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_PartitionerName;
    DataStreamIn >> m_PartitionerDesc;
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataGenerator::SSTInfoDataGenerator(QString ParentElementName, QString GeneratorName)
{
     m_ParentElementName = ParentElementName;
     m_GeneratorName = GeneratorName;
     m_GeneratorDesc.clear();
}

SSTInfoDataGenerator::~SSTInfoDataGenerator()
{
}

void SSTInfoDataGenerator::SaveData(QDataStream& DataStreamOut)
{
    // Serialize and Save the General Information
    DataStreamOut << m_ParentElementName;
    DataStreamOut << m_GeneratorName;
    DataStreamOut << m_GeneratorDesc;
}

SSTInfoDataGenerator::SSTInfoDataGenerator(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialiation: Load The General Information
    DataStreamIn >> m_ParentElementName;
    DataStreamIn >> m_GeneratorName;
    DataStreamIn >> m_GeneratorDesc;
}

//////////////////////////////////////////////////////////////////////

SSTInfoDataElement::SSTInfoDataElement(QString ElementName)
{
    m_ElementName = ElementName;
    m_ElementDesc.clear();

    m_ComponentList.clear();
    m_IntrospectorList.clear();
    m_EventList.clear();
    m_ModuleList.clear();
    m_PartitionerList.clear();
    m_GeneratorList.clear();
}

SSTInfoDataElement::~SSTInfoDataElement()
{
    int x;
    int count;

    // Remove all Components to prevent memory leaks
    count = m_ComponentList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ComponentList.at(0);
        m_ComponentList.removeAt(0);
    }
    m_ComponentList.clear();

    // Remove all Introspectors to prevent memory leaks
    count = m_IntrospectorList.count();
    for (x = 0; x < count; x++)
    {
        delete m_IntrospectorList.at(0);
        m_IntrospectorList.removeAt(0);
    }
    m_IntrospectorList.clear();

    // Remove all Events to prevent memory leaks
    count = m_EventList.count();
    for (x = 0; x < count; x++)
    {
        delete m_EventList.at(0);
        m_EventList.removeAt(0);
    }
    m_EventList.clear();

    // Remove all Modules to prevent memory leaks
    count = m_ModuleList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ModuleList.at(0);
        m_ModuleList.removeAt(0);
    }
    m_ModuleList.clear();

    // Remove all Partitioners to prevent memory leaks
    count = m_PartitionerList.count();
    for (x = 0; x < count; x++)
    {
        delete m_PartitionerList.at(0);
        m_PartitionerList.removeAt(0);
    }
    m_PartitionerList.clear();

    // Remove all Generators to prevent memory leaks
    count = m_GeneratorList.count();
    for (x = 0; x < count; x++)
    {
        delete m_GeneratorList.at(0);
        m_GeneratorList.removeAt(0);
    }
    m_GeneratorList.clear();
}

void SSTInfoDataElement::AddComponent(SSTInfoDataComponent* NewComponent)
{
    m_ComponentList.append(NewComponent);
}

void SSTInfoDataElement::AddIntrospector(SSTInfoDataIntrospector* NewIntrospector)
{
    m_IntrospectorList.append(NewIntrospector);
}

void SSTInfoDataElement::AddEvent(SSTInfoDataEvent* NewEvent)
{
    m_EventList.append(NewEvent);
}

void SSTInfoDataElement::AddModule(SSTInfoDataModule* NewModule)
{
    m_ModuleList.append(NewModule);
}

void SSTInfoDataElement::AddPartitioner(SSTInfoDataPartitioner* NewPartitioner)
{
    m_PartitionerList.append(NewPartitioner);
}

void SSTInfoDataElement::AddGenerator(SSTInfoDataGenerator* NewGenerator)
{
    m_GeneratorList.append(NewGenerator);
}

void SSTInfoDataElement::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialize and Save the General Information
    DataStreamOut << m_ElementName;
    DataStreamOut << m_ElementDesc;

    // Serialize and Save the Counts
    DataStreamOut << (qint32)m_ComponentList.count();
    DataStreamOut << (qint32)m_IntrospectorList.count();
    DataStreamOut << (qint32)m_EventList.count();
    DataStreamOut << (qint32)m_ModuleList.count();
    DataStreamOut << (qint32)m_PartitionerList.count();
    DataStreamOut << (qint32)m_GeneratorList.count();

    // Serialize and Save all the Components
    for (x = 0; x < m_ComponentList.count(); x++)
    {
        m_ComponentList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Introspectors
    for (x = 0; x < m_IntrospectorList.count(); x++)
    {
        m_IntrospectorList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Events
    for (x = 0; x < m_EventList.count(); x++)
    {
        m_EventList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Modules
    for (x = 0; x < m_ModuleList.count(); x++)
    {
        m_ModuleList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Partitioners
    for (x = 0; x < m_PartitionerList.count(); x++)
    {
        m_PartitionerList.at(x)->SaveData(DataStreamOut);
    }

    // Serialize and Save all the Generators
    for (x = 0; x < m_GeneratorList.count(); x++)
    {
        m_GeneratorList.at(x)->SaveData(DataStreamOut);
    }
}

SSTInfoDataElement::SSTInfoDataElement(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int     x;
    int     ComponentCount;
    int     IntrospectorCount;
    int     EventCount;
    int     ModuleCount;
    int     PartitionerCount;
    int     GeneratorCount;

    // Serialiation: Load The General Information
    DataStreamIn >> m_ElementName;
    DataStreamIn >> m_ElementDesc;

    // Load the Counts
    DataStreamIn >> ComponentCount;
    DataStreamIn >> IntrospectorCount;
    DataStreamIn >> EventCount;
    DataStreamIn >> ModuleCount;
    DataStreamIn >> PartitionerCount;
    DataStreamIn >> GeneratorCount;

    // Load all the Components
    for (x = 0; x < ComponentCount; x++)
    {
        SSTInfoDataComponent* newComponent = new SSTInfoDataComponent(DataStreamIn, ProjectFileVersion);
        AddComponent(newComponent);
    }

    // Load all the Introspectors
    for (x = 0; x < IntrospectorCount; x++)
    {
        SSTInfoDataIntrospector* newIntrospector = new SSTInfoDataIntrospector(DataStreamIn, ProjectFileVersion);
        AddIntrospector(newIntrospector);
    }

    // Load all the Events
    for (x = 0; x < EventCount; x++)
    {
        SSTInfoDataEvent* newEvent = new SSTInfoDataEvent(DataStreamIn, ProjectFileVersion);
        AddEvent(newEvent);
    }

    // Load all the Modules
    for (x = 0; x < ModuleCount; x++)
    {
        SSTInfoDataModule* newModule = new SSTInfoDataModule(DataStreamIn, ProjectFileVersion);
        AddModule(newModule);
    }

    // Load all the Partitioners
    for (x = 0; x < PartitionerCount; x++)
    {
        SSTInfoDataPartitioner* newPartitioner = new SSTInfoDataPartitioner(DataStreamIn, ProjectFileVersion);
        AddPartitioner(newPartitioner);
    }

    // Load all the Generators
    for (x = 0; x < GeneratorCount; x++)
    {
        SSTInfoDataGenerator* newGenerator = new SSTInfoDataGenerator(DataStreamIn, ProjectFileVersion);
        AddGenerator(newGenerator);
    }

}

//////////////////////////////////////////////////////////////////////

SSTInfoData::SSTInfoData()
{
    m_ElementList.clear();
    m_SSTInfoVersion.clear();
}

SSTInfoData::~SSTInfoData()
{
    int x;
    int count;

    // Remove all Elements to prevent memory leaks
    count = m_ElementList.count();
    for (x = 0; x < count; x++)
    {
        delete m_ElementList.at(0);
        m_ElementList.removeAt(0);
    }
    m_ElementList.clear();
}

void SSTInfoData::AddElement(SSTInfoDataElement* NewElement)
{
    m_ElementList.append(NewElement);
}

void SSTInfoData::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialize and Save the Counts
    DataStreamOut << m_SSTInfoVersion;
    DataStreamOut << (qint32)m_ElementList.count();

    // Serialize and Save all the Elements
    for (x = 0; x < m_ElementList.count(); x++)
    {
        m_ElementList.at(x)->SaveData(DataStreamOut);
    }
}

SSTInfoData::SSTInfoData(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int     x;
    int     ElementCount;

    // Serialize: Load the Counts
    DataStreamIn >> m_SSTInfoVersion;
    DataStreamIn >> ElementCount;

    // Load all the Parameters
    for (x = 0; x < ElementCount; x++)
    {
        SSTInfoDataElement* newElement = new SSTInfoDataElement(DataStreamIn, ProjectFileVersion);
        AddElement(newElement);
    }
}
