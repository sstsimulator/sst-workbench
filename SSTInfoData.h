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

#ifndef SSTINFODATA_H
#define SSTINFODATA_H

#include "GlobalIncludes.h"

//////////////////////////////////////////////////////////////////////

class SSTInfoDataParam
{
public:
    // Constructor / Destructor
    SSTInfoDataParam(QString ParamName, QString DefaultValue = "");
    SSTInfoDataParam(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataParam();

    // Parameter Name, Description and Default Value
    void SetParamDesc(QString NewDesc) {m_ParamDesc = NewDesc;}

    QString GetParamName() {return m_ParamName;}
    QString GetParamDesc() {return m_ParamDesc;}
    QString GetDefaultValue() {return m_ParamDefaultValue;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    QString m_ParamName;
    QString m_ParamDesc;
    QString m_ParamDefaultValue;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataPort
{
public:
    // Constructor / Destructor
    SSTInfoDataPort(QString PortName);
    SSTInfoDataPort(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataPort();

    // Port Name and Description
    void SetPortDesc(QString NewDesc) {m_PortDesc = NewDesc;}

    QString GetPortName() {return m_PortName;}
    QString GetPortDesc() {return m_PortDesc;}

    // Port Events
    void AddPortValidEvent(QString Event) {m_PortValidEvents.append(Event);}
    int GetNumPortValidEvents() {return m_PortValidEvents.count();}
    QString GetPortValidEvent(int Index) {return m_PortValidEvents.at(Index);}
    QStringList& GetPortValidEvents() {return m_PortValidEvents;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    QString     m_PortName;
    QString     m_PortDesc;
    QStringList m_PortValidEvents;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataComponent
{
public:
    // Constructor / Destructor
    SSTInfoDataComponent(QString ParentElementName, QString ComponentName, ComponentType_enum Type, int NumAllowedInstances = -1);
    SSTInfoDataComponent(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataComponent();

    // Component Names, Type & Description
    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetComponentName() const {return m_ComponentName;}
    ComponentType_enum GetComponentType() {return m_ComponentType;}
    static QString GetComponentTypeName(ComponentType_enum Type);
    QString GetComponentDesc() const {return m_ComponentDesc;}
    void SetComponentDesc(QString NewDesc) {m_ComponentDesc = NewDesc;}

    // Parameters and Ports
    int GetNumOfParams() {return m_ParamNameList.count();}
    int GetNumOfPorts() {return m_PortNameList.count();}

    SSTInfoDataParam* GetParam(int index) {return m_ParamNameList.at(index);}
    SSTInfoDataPort*  GetPort(int index) {return m_PortNameList.at(index);}

    void AddParam(SSTInfoDataParam* NewParam);
    void AddParam(QString NewParamName, QString DefaultValue, QString Desc = "");
    void AddParam(QString NewParamName);

    void AddPort(SSTInfoDataPort* NewPort);
    void AddPort(QString NewPortName);

    // The allowed number of instances of this component
    int GetAllowedNumberOfInstances() {return m_AllowedNumberOfInstances;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_ComponentName;
    QString                      m_ComponentDesc;
    ComponentType_enum           m_ComponentType;
    QList<SSTInfoDataParam*>     m_ParamNameList;
    QList<SSTInfoDataPort*>      m_PortNameList;

    // Info that SSTWorkbench needs
    int                          m_AllowedNumberOfInstances;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataIntrospector
{
public:
    // Constructor / Destructor
    SSTInfoDataIntrospector(QString ParentElementName, QString IntrospectorName);
    SSTInfoDataIntrospector(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataIntrospector();

    // Element Name and Description
    void SetIntrospectorDesc(QString NewDesc) {m_IntrospectorDesc = NewDesc;}

    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetIntrospectorName() const {return m_IntrospectorName;}
    QString GetIntrospectorDesc() const {return m_IntrospectorDesc;}

    // Parameters
    int GetNumOfParams() {return m_ParamNameList.count();}

    SSTInfoDataParam* GetParam(int index) {return m_ParamNameList.at(index);}

    void AddParam(SSTInfoDataParam* NewParam);
    void AddParam(QString NewParamName, QString DefaultValue, QString Desc = "");
    void AddParam(QString NewParamName);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_IntrospectorName;
    QString                      m_IntrospectorDesc;
    QList<SSTInfoDataParam*>     m_ParamNameList;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataEvent
{
public:
    // Constructor / Destructor
    SSTInfoDataEvent(QString ParentElementName, QString EventName);
    SSTInfoDataEvent(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataEvent();

    // Event Name & description
    void SetEventDesc(QString NewDesc) {m_EventDesc = NewDesc;}

    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetEventName() const {return m_EventName;}
    QString GetEventDesc() const {return m_EventDesc;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_EventName;
    QString                      m_EventDesc;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataModule
{
public:
    // Constructor / Destructor
    SSTInfoDataModule(QString ParentElementName, QString ModuleName);
    SSTInfoDataModule(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataModule();

    // Data Name and Description
    void SetModuleDesc(QString NewDesc) {m_ModuleDesc = NewDesc;}

    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetModuleName() const {return m_ModuleName;}
    QString GetModuleDesc() const {return m_ModuleDesc;}

    // Parameters
    int GetNumOfParams() {return m_ParamNameList.count();}

    SSTInfoDataParam* GetParam(int index) {return m_ParamNameList.at(index);}

    void AddParam(SSTInfoDataParam* NewParam);
    void AddParam(QString NewParamName, QString DefaultValue, QString Desc = "");
    void AddParam(QString NewParamName);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_ModuleName;
    QString                      m_ModuleDesc;
    QList<SSTInfoDataParam*>     m_ParamNameList;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataPartitioner
{
public:
    // Constructor / Destructor
    SSTInfoDataPartitioner(QString ParentElementName, QString PartitionerName);
    SSTInfoDataPartitioner(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataPartitioner();

    // Partitioner Name & Description
    void SetPartitionerDesc(QString NewDesc) {m_PartitionerDesc = NewDesc;}

    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetPartitionerName() const {return m_PartitionerName;}
    QString GetPartitionerDesc() const {return m_PartitionerDesc;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_PartitionerName;
    QString                      m_PartitionerDesc;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataGenerator
{
public:
    // Constructor / Destructor
    SSTInfoDataGenerator(QString ParentElementName, QString GeneratorName);
    SSTInfoDataGenerator(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataGenerator();

    // Generator Name & Description
    void SetGeneratorDesc(QString NewDesc) {m_GeneratorDesc = NewDesc;}

    QString GetParentElementName() const {return m_ParentElementName;}
    QString GetGeneratorName() const {return m_GeneratorName;}
    QString GetGeneratorDesc() const {return m_GeneratorDesc;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    // Info from SST Info
    QString                      m_ParentElementName;
    QString                      m_GeneratorName;
    QString                      m_GeneratorDesc;
};

//////////////////////////////////////////////////////////////////////

class SSTInfoDataElement
{
public:
    // Constructor / Destructor
    SSTInfoDataElement(QString ElementName);
    SSTInfoDataElement(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoDataElement();

    // Element Name & Description
    void SetElementDesc(QString NewDesc) {m_ElementDesc = NewDesc;}

    QString GetElementName() const {return m_ElementName;}
    QString GetElementDesc() const {return m_ElementDesc;}

    // Sub-objects manipulation
    int GetNumOfComponents() {return m_ComponentList.count();}
    int GetNumOfIntrospectors() {return m_IntrospectorList.count();}
    int GetNumOfEvents() {return m_EventList.count();}
    int GetNumOfModules() {return m_ModuleList.count();}
    int GetNumOfPartitionerss() {return m_PartitionerList.count();}
    int GetNumOfGenerators() {return m_GeneratorList.count();}

    SSTInfoDataComponent* GetComponent(int index) {return m_ComponentList.at(index);}
    SSTInfoDataIntrospector* GetIntrospector(int index) {return m_IntrospectorList.at(index);}
    SSTInfoDataEvent* GetEvent(int index) {return m_EventList.at(index);}
    SSTInfoDataModule* GetModule(int index) {return m_ModuleList.at(index);}
    SSTInfoDataPartitioner* GetPartitioner(int index) {return m_PartitionerList.at(index);}
    SSTInfoDataGenerator* GetGenerator(int index) {return m_GeneratorList.at(index);}

    void AddComponent(SSTInfoDataComponent* NewComponent);
    void AddIntrospector(SSTInfoDataIntrospector* NewIntrospector);
    void AddEvent(SSTInfoDataEvent* NewEvent);
    void AddModule(SSTInfoDataModule* NewModule);
    void AddPartitioner(SSTInfoDataPartitioner* NewPartitioner);
    void AddGenerator(SSTInfoDataGenerator* NewGenerator);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    QString                         m_ElementName;
    QString                         m_ElementDesc;

    QList<SSTInfoDataComponent*>    m_ComponentList;
    QList<SSTInfoDataIntrospector*> m_IntrospectorList;
    QList<SSTInfoDataEvent*>        m_EventList;
    QList<SSTInfoDataModule*>       m_ModuleList;
    QList<SSTInfoDataPartitioner*>  m_PartitionerList;
    QList<SSTInfoDataGenerator*>    m_GeneratorList;
};


//////////////////////////////////////////////////////////////////////

class SSTInfoData
{
public:
    // Constructor / Destructor
    SSTInfoData();
    SSTInfoData(QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization
    ~SSTInfoData();

    int GetNumOfElements() {return m_ElementList.count();}
    SSTInfoDataElement* GetElement(int index) {return m_ElementList.at(index);}

    void AddElement(SSTInfoDataElement* NewElement);

    QString GetSSTInfoVersion() {return m_SSTInfoVersion;}
    void SetSSTInfoVersion(QString Ver) {m_SSTInfoVersion = Ver;}

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

private:
    QList<SSTInfoDataElement*> m_ElementList;
    QString                    m_SSTInfoVersion;
};

#endif // SSTINFODATA_H
