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
    // Constructor / Destructor
    ItemProperty(ItemProperties* ParentItemProperties, QString PropertyName, QString OrigPropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable, bool DynamicFlag, QString ControllingProperty);
    ItemProperty(ItemProperties* ParentItemProperties, QDataStream& DataStreamIn, qint32 ProjectFileVersion);  // Only used for serialization

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

    // Property Protected Flag
    void SetProtected(bool flag) {m_Protected = flag;}
    bool GetProtected() {return m_Protected;}

    // Property Exportable Flag
    void SetExportable(bool flag) {m_Exportable = flag;}
    bool GetExportable() {return m_Exportable;}

    // Dynamic Property Settings
    bool GetDynamicFlag() {return m_DynamicFlag;}
    QString GetDefaultValue() {return m_DefaultValue;}
    QString GetOriginalPropertyName() {return m_OriginalPropertyName;}
    QString GetControllingProperty() {return m_ControllingProperty;}
    void SetNumInstances(int NumInstances) {m_NumInstances = NumInstances;}
    int GetNumInstances() {return m_NumInstances;}

    // Save the Property Data (Used for serialization)
    void SaveData(QDataStream& DataStreamOut);

private:
    QString          m_PropertyName;           // The Displayed Property Name after
    QString          m_OriginalPropertyName;   // Orig Prop Name from SSTInfo.xml
    QString          m_PropertyValue;          // Property Value
    QString          m_DefaultValue;           // Default Value from SSTInfo.xml
    QString          m_PropertyDesc;           // Property Description
    bool             m_ReadOnly;               // Is Property Value Read Only
    bool             m_Protected;              // Is Property Cannot be Deleted
    bool             m_Exportable;             // Is Property Exportable to Python
    bool             m_DynamicFlag;            // Is Property Dynamic
    QString          m_ControllingProperty;    // If Dynamic, what is the controlling Property
    int              m_NumInstances;           // Number of instances of a Dynamic Property
    ItemProperties*  m_ParentProperties;       // Owning list of Properties
};

////////////////////////////////////////////////////////////

class ItemProperties
{
public:
    // Constructor / Destructor
    ItemProperties(GraphicItemBase* ParentGraphicItemBase);
    ~ItemProperties();

    // Add a new property
    void AddProperty(QString PropertyName, QString PropertyValue = "", QString PropertyDesc = "", bool ReadOnly = false, bool Protected = false, bool Exportable = true);

    // Remove a Property
    void RemoveProperty(QString PropertyName);
    void RemoveProperty(int Index);

    // Get the Graphic Item base that is parent to this object
    GraphicItemBase* GetParentGraphicItemBase() {return m_ParentGraphicItemBase;}

    // Return the Number of properties this object holds
    int GetNumProperties();

    // Set a Property Value
    void SetPropertyValue(QString PropertyName, QString PropertyValue);
    void SetPropertyValue(int Index, QString PropertyValue);

    // Set a Property Desc
    void SetPropertyDesc(QString PropertyName, QString PropertyDesc);
    void SetPropertyDesc(int Index, QString PropertyDesc);

    // Change a Property Name
    void ChangePropertyName(QString OldName, QString NewName);
    void ChangePropertyName(int Index, QString NewName);

    // Set Property Flags
    void SetPropertyReadOnly(QString PropertyName, bool Flag);
    void SetPropertyReadOnly(int Index, bool Flag);

    void SetPropertyProtected(QString PropertyName, bool Flag);
    void SetPropertyProtected(int Index, bool Flag);

    void SetPropertyExportable(QString PropertyName, bool Flag);
    void SetPropertyExportable(int Index, bool Flag);

    // Get a Property Value
    int GetPropertyIndex(QString PropertyName);
    QString GetPropertyValue(QString PropertyName);
    QString GetPropertyDesc(QString PropertyName);

    // Get an actual Property Object
    ItemProperty* GetProperty(QString PropertyName);
    ItemProperty* GetProperty(int Index);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);
    void LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion);

    // Called only when a property changed
    void PropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback);
    void CheckIfDynamicPropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback);
    void AdjustDynamicPropertyInList(QString PropertyName, int NumInstances, bool PerformCallback);

private:
    void AddStaticProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable);
    void AddDynamicProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable, QString ControllingProperty);
    bool IsPropertyNameNotInList(QString PropertyName);

private:
    QList<ItemProperty*> m_PropertyList;
    GraphicItemBase*     m_ParentGraphicItemBase;
};

#endif // ITEMPROPERTIES_H

