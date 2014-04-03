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

#include "ItemProperties.h"

////////////////////////////////////////////////////////////

ItemProperty::ItemProperty(ItemProperties* ParentItemProperties, QString PropertyName, QString PropertyValue/*=""*/, QString PropertyDesc/*=""*/, bool ReadOnly/*=false*/, bool Exportable/*=true*/)
{
    // Init the Variables
    m_ParentProperties = ParentItemProperties;
    m_PropertyName = PropertyName;
    m_PropertyValue = PropertyValue;
    m_PropertyDesc = PropertyDesc;
    m_ReadOnly = ReadOnly;
    m_Exportable = Exportable;
}

ItemProperty::~ItemProperty()
{
}

void ItemProperty::SetValue(QString NewValue, bool PerformCallback /*=true*/)
{
    // Set the Property Value, Callback the PropertyChanged if requested (happens most of the time)
    m_PropertyValue = NewValue;
    if (PerformCallback == true) {
        m_ParentProperties->GetParentGraphicItemBase()->PropertyChanged(m_PropertyName, m_PropertyValue);
    }
}

void ItemProperty::SaveData(QDataStream& DataStreamOut)
{
    // Serialization Save
    // Save off the data
    DataStreamOut << m_PropertyName;
    DataStreamOut << m_PropertyValue;
    DataStreamOut << m_PropertyDesc;
    DataStreamOut << m_ReadOnly;
    DataStreamOut << m_Exportable;
}

ItemProperty::ItemProperty(ItemProperties* ParentItemProperties, QDataStream& DataStreamIn)
{
    // Serialization Load
    m_ParentProperties = ParentItemProperties;

    // Read In the data
    DataStreamIn >> m_PropertyName;
    DataStreamIn >> m_PropertyValue;
    DataStreamIn >> m_PropertyDesc;
    DataStreamIn >> m_ReadOnly;
    DataStreamIn >> m_Exportable;
}

////////////////////////////////////////////////////////////

ItemProperties::ItemProperties(GraphicItemBase* ParentGraphicItemBase)
{
    // Init Variables
    m_ParentGraphicItemBase = ParentGraphicItemBase;  // Base Graphic Item that holds these properties
    m_PropertyList.clear();
}

ItemProperties::~ItemProperties()
{
    ItemProperty* PropItem;
    int           x;
    int           NumProperties = GetNumProperties();

    // Delete all the properties that were created
    for (x = 0; x < NumProperties; x++) {
        PropItem = m_PropertyList.at(0);
        delete PropItem;
        m_PropertyList.removeAt(0);
    }
}

void ItemProperties::AddProperty(QString PropertyName, QString PropertyValue /*=""*/, QString PropertyDesc /*=""*/, bool ReadOnly /*=false*/, bool Exportable/*=true*/)
{
    ItemProperty* PropItem;

    // Create the new ItemProperty
    PropItem = new ItemProperty(this, PropertyName, PropertyValue, PropertyDesc, ReadOnly, Exportable);

    // Add it to the list
    m_PropertyList.push_back(PropItem);
}

int ItemProperties::GetNumProperties()
{
    return m_PropertyList.count();
}

void ItemProperties::SetPropertyValue(QString PropertyName, QString PropertyValue)
{
    ItemProperty* PropItem;

    PropItem = GetProperty(PropertyName);
    if (PropItem != NULL) {
        PropItem->SetValue(PropertyValue);
    }
}

void ItemProperties::SetPropertyValue(int Index, QString PropertyValue)
{
    ItemProperty* PropItem;

    if (Index < GetNumProperties()) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetValue(PropertyValue);
    }
}

int ItemProperties::GetPropertyIndex(QString PropertyName)
{
    int x;

    for (x = 0; x < GetNumProperties(); x++) {
        if (PropertyName == m_PropertyList.at(x)->GetName()) {
            return x;
        }
    }
    return -1;
}

QString ItemProperties::GetPropertyValue(QString PropertyName)
{
    int x;

    for (x = 0; x < GetNumProperties(); x++) {
        if (PropertyName == m_PropertyList.at(x)->GetName()) {
            return m_PropertyList.at(x)->GetValue();
        }
    }
    return "";
}

QString ItemProperties::GetPropertyDesc(QString PropertyName)
{
    int x;

    for (x = 0; x < GetNumProperties(); x++) {
        if (PropertyName == m_PropertyList.at(x)->GetName()) {
            return m_PropertyList.at(x)->GetDesc();
        }
    }
    return "";
}

ItemProperty* ItemProperties::GetProperty(QString PropertyName)
{
    int x;

    for (x = 0; x < GetNumProperties(); x++) {
        if (PropertyName == m_PropertyList.at(x)->GetName()) {
            return m_PropertyList.at(x);
        }
    }
    return NULL;
}

ItemProperty* ItemProperties::GetProperty(int Index)
{
    if (Index < GetNumProperties()) {
        return m_PropertyList.at(Index);
    }
    return NULL;
}

void ItemProperties::SaveData(QDataStream& DataStreamOut)
{
    int x;

    // Serialization Save
    // Save the number of Properties and then each one
    DataStreamOut << (qint32)m_PropertyList.count();
    for (x = 0; x < m_PropertyList.count(); x++) {
        m_PropertyList[x]->SaveData(DataStreamOut);
    }
}

void ItemProperties::LoadData(QDataStream& DataStreamIn)
{
    int NumProperties;

    // Serialization Load
    // Load the number of properties
    DataStreamIn >> NumProperties;

    // Load each individual property
    for (int x = 0; x < NumProperties; x++) {
        // Create the new ItemProperty
        ItemProperty* PropItem = new ItemProperty(this, DataStreamIn);

        // Add it to the list
        m_PropertyList.push_back(PropItem);
    }
}
