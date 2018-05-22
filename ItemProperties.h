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

#ifndef ITEMPROPERTIES_H
#define ITEMPROPERTIES_H

#include "GlobalIncludes.h"

// Forward declarations to allow compile
class GraphicItemBase;
class ItemProperties;

////////////////////////////////////////////////////////////

class ItemProperty
{
public:
    enum RO_FLAG { READWRITE, READONLY };

    // Constructor / Destructor
    ItemProperty(ItemProperties* ParentItemProperties, QString PropertyName, QString OrigPropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Exportable, bool DynamicFlag, QString ControllingParam);
    ItemProperty(ItemProperties* ParentItemProperties, QDataStream& DataStreamIn);  // Only used for serialization

    ~ItemProperty();

    // Parent Properties
    ItemProperties* GetParentProperties() {return m_ParentProperties;}

    // Property Name
    void SetName(QString NewName) {m_PropertyName = NewName;}
    QString GetName() {return m_PropertyName;}

    // Property  Value
    void SetValue(QString NewValue, bool PerformCallback = true);
    QString GetValue() {return m_PropertyValue;}

    // Property  Description
    void SetDesc(QString NewDesc) {m_PropertyDesc = NewDesc;}
    QString GetDesc() {return m_PropertyDesc;}

    // Property Read Only Flag
    void SetReadOnly(bool flag) {m_ReadOnly = flag;}
    bool GetReadOnly() {return m_ReadOnly;}

    // Property Exportable Flag
    void SetExportable(bool flag) {m_Exportable = flag;}
    bool GetExportable() {return m_Exportable;}

    // Dynamic Parameter Settings
    bool GetDynamicFlag() {return m_DynamicFlag;}
    QString GetDefaultValue() {return m_DefaultValue;}
    QString GetOriginalPropertyName() {return m_OriginalPropertyName;}
    QString GetControllingProperty() {return m_ControllingProperty;}
    void SetNumInstances(int NumInstances) {m_NumInstances = NumInstances;}
    int GetNumInstances() {return m_NumInstances;}

    // Save the Property Data (Used for serialization)
    void SaveData(QDataStream& DataStreamOut);

private:
    QString          m_PropertyName;
    QString          m_OriginalPropertyName;
    QString          m_PropertyValue;
    QString          m_DefaultValue;
    QString          m_PropertyDesc;
    bool             m_ReadOnly;
    bool             m_Exportable;
    bool             m_DynamicFlag;
    QString          m_ControllingProperty;
    int              m_NumInstances;
    ItemProperties*  m_ParentProperties;
};

////////////////////////////////////////////////////////////

class ItemProperties
{
public:
    // Constructor / Destructor
    ItemProperties(GraphicItemBase* ParentGraphicItemBase);
    ~ItemProperties();

    // Add a new property
    void AddProperty(QString PropertyName, QString PropertyValue = "", QString PropertyDesc = "", bool ReadOnly = false, bool Exportable = true);

    // Get the Graphic Item base that is parent to this object
    GraphicItemBase* GetParentGraphicItemBase() {return m_ParentGraphicItemBase;}

    // Return the Number of properties this object holds
    int GetNumProperties();

    // Set a Property Value
    void SetPropertyValue(QString PropertyName, QString PropertyValue);
    void SetPropertyValue(int Index, QString PropertyValue);

    // Get a Property Value
    int GetPropertyIndex(QString PropertyName);
    QString GetPropertyValue(QString PropertyName);
    QString GetPropertyDesc(QString PropertyName);

    // Get an actual Property Object
    ItemProperty* GetProperty(QString PropertyName);
    ItemProperty* GetProperty(int Index);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);
    void LoadData(QDataStream& DataStreamIn);

    // Called only when a property changed
    void PropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback);
    void CheckIfDynamicPropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback);
    void AdjustDynamicPropertyInList(QString PropertyName, int NumInstances, bool PerformCallback);

private:
    void AddStaticProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Exportable);
    void AddDynamicProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Exportable, QString ControllingParam);
    bool IsPropertyNameNotInList(QString PropertyName);

private:
    QList<ItemProperty*> m_PropertyList;
    GraphicItemBase*     m_ParentGraphicItemBase;
};

#endif // ITEMPROPERTIES_H

