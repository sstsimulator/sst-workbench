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
    enum RO_FLAG { READWRITE, READONLY };

    // Constructor / Destructor
    ItemProperty(ItemProperties* ParentItemProperties, QString PropertyName, QString PropertyValue = "", QString PropertyDesc = "", bool ReadOnly = false, bool Exportable = true);
    ItemProperty(ItemProperties* ParentItemProperties, QDataStream& DataStreamIn);  // Only used for serialization

    ~ItemProperty();

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

    // Save the Property Data (Used for serialization)
    void SaveData(QDataStream& DataStreamOut);

private:
    QString          m_PropertyName;
    QString          m_PropertyValue;
    QString          m_PropertyDesc;
    bool             m_ReadOnly;
    bool             m_Exportable;
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

private:
    QList<ItemProperty*> m_PropertyList;
    GraphicItemBase*     m_ParentGraphicItemBase;
};

#endif // ITEMPROPERTIES_H

