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

ItemProperty::ItemProperty(ItemProperties* ParentItemProperties, QString PropertyName, QString OrigPropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable, bool DynamicFlag, QString ControllingProperty)
{
    // Init the Variables
    m_ParentProperties = ParentItemProperties;
    m_PropertyName = PropertyName;
    m_OriginalPropertyName = OrigPropertyName;
    m_PropertyValue = PropertyValue;
    m_DefaultValue = PropertyValue;
    m_PropertyDesc = PropertyDesc;
    m_ReadOnly = ReadOnly;
    m_Protected = Protected;
    m_Exportable = Exportable;
    m_DynamicFlag = DynamicFlag;
    m_DefaultValue = PropertyValue;
    m_ControllingProperty = ControllingProperty;
    m_NumInstances = 0;
}

ItemProperty::~ItemProperty()
{
}

void ItemProperty::SetValue(QString NewValue, bool PerformCallback /*=true*/)
{
    // Set the Property Value, Callback the PropertyChanged if requested (happens most of the time)
    m_PropertyValue = NewValue;

    // Tell the Parent Properties List that this property changed, it may also tell the
    // Graphical Item if the PerformCallback is true
    m_ParentProperties->PropertyChanged(m_PropertyName, m_PropertyValue, PerformCallback);
}

void ItemProperty::SaveData(QDataStream& DataStreamOut)
{
    // Serialization Save
    // Save off the data
    DataStreamOut << m_PropertyName;
    DataStreamOut << m_OriginalPropertyName;
    DataStreamOut << m_PropertyValue;
    DataStreamOut << m_DefaultValue;
    DataStreamOut << m_PropertyDesc;
    DataStreamOut << m_ReadOnly;
    DataStreamOut << m_Protected;
    DataStreamOut << m_Exportable;
    DataStreamOut << m_DynamicFlag;
    DataStreamOut << m_ControllingProperty;
    DataStreamOut << (qint32)m_NumInstances;
}

ItemProperty::ItemProperty(ItemProperties* ParentItemProperties, QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    // Serialization Load
    m_ParentProperties = ParentItemProperties;

    // Read In the data
    DataStreamIn >> m_PropertyName;
    DataStreamIn >> m_OriginalPropertyName;
    DataStreamIn >> m_PropertyValue;
    DataStreamIn >> m_DefaultValue;
    DataStreamIn >> m_PropertyDesc;
    DataStreamIn >> m_ReadOnly;
    if (ProjectFileVersion == SSTWORKBENCHPROJECTFILEFORMATVER_2_0) {
        // Restore protected Properties
        DataStreamIn >> m_Protected;
    } else {
        // Version 1.0 did not have a Protected Property flag
        m_Protected = false;
    }
    DataStreamIn >> m_Exportable;
    DataStreamIn >> m_DynamicFlag;
    DataStreamIn >> m_ControllingProperty;
    DataStreamIn >> m_NumInstances;
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

void ItemProperties::AddProperty(QString PropertyName, QString PropertyValue /*=""*/, QString PropertyDesc /*=""*/, bool ReadOnly /*=false*/, bool Protected /*=false*/, bool Exportable/*=true*/)
{
    int     Index1;
    int     Index2;
    QString ControllingPropertyName;
    QString NewPropertyName;

    // Search The Property Name for NOT containing a "%d" or a "%("
    // that will indicate that it is a Static Property
    if ((PropertyName.contains("%d") == false) && (PropertyName.contains("%(") == false) ) {
        // This is a Static Property;
        AddStaticProperty(PropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable);
        return;
    }

    // If we get here, then this is a Dynamic Property, but we still have to figure a few things out.
    // Search the Property Name for a "%d" that will indicate that it is a Dynamic Property with no Associated property
    if (PropertyName.contains("%d") == true) {
        // This is a Dynamic Port; We Need to identify that the port is unconfigured
        AddDynamicProperty(PropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable, "");
        return;
    }

    // If we get here, then this is a Dynamic Property, but it has a assocated Property
    // (a Component Property). we need to crack the Property from the property name
    Index1 = PropertyName.indexOf("%(", 0);
    Index2 = PropertyName.indexOf(")d", Index1);
    // Make sure we found both items
    if ((Index1 >= 0) && (Index2 > Index1)) {
        // Extract the Propery Name
        ControllingPropertyName = PropertyName.mid(Index1 + 2, Index2 - Index1 - 2);

        // Create the New Property Name With the Controlling Property stripped out
        NewPropertyName = PropertyName.left(Index1+1) + PropertyName.mid(Index2 + 1);

        // Change the Property name Name
        AddDynamicProperty(NewPropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable, ControllingPropertyName);

        return;
    } else {
        // Fall back and say the Property is static
        AddStaticProperty(PropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable);
        return;
    }
}

void ItemProperties::RemoveProperty(QString PropertyName)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    RemoveProperty(PropIndex);
}

void ItemProperties::RemoveProperty(int Index)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        delete PropItem;
        m_PropertyList.removeAt(Index);
    }
}

int ItemProperties::GetNumProperties()
{
    return m_PropertyList.count();
}

void ItemProperties::SetPropertyValue(QString PropertyName, QString PropertyValue)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    SetPropertyValue(PropIndex, PropertyValue);
}

void ItemProperties::SetPropertyValue(int Index, QString PropertyValue)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetValue(PropertyValue);
    }
}

void ItemProperties::SetPropertyDesc(QString PropertyName, QString PropertyDesc)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    SetPropertyDesc(PropIndex, PropertyDesc);
}

void ItemProperties::SetPropertyDesc(int Index, QString PropertyDesc)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetDesc(PropertyDesc);
    }
}

void ItemProperties::ChangePropertyName(QString OldName, QString NewName)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(OldName);
    ChangePropertyName(PropIndex, NewName);
}

void ItemProperties::ChangePropertyName(int Index, QString NewName)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetName(NewName);
    }
}

void ItemProperties::SetPropertyReadOnly(QString PropertyName, bool Flag)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    SetPropertyReadOnly(PropIndex, Flag);
}

void ItemProperties::SetPropertyReadOnly(int Index, bool Flag)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetReadOnly(Flag);
    }
}

void ItemProperties::SetPropertyProtected(QString PropertyName, bool Flag)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    SetPropertyProtected(PropIndex, Flag);
}

void ItemProperties::SetPropertyProtected(int Index, bool Flag)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetProtected(Flag);
    }
}

void ItemProperties::SetPropertyExportable(QString PropertyName, bool Flag)
{
    int           PropIndex;

    PropIndex = GetPropertyIndex(PropertyName);
    SetPropertyExportable(PropIndex, Flag);
}

void ItemProperties::SetPropertyExportable(int Index, bool Flag)
{
    ItemProperty* PropItem;

    if ((Index >= 0) && (Index < GetNumProperties())) {
        PropItem = m_PropertyList.at(Index);
        PropItem->SetExportable(Flag);
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
    if ((Index >= 0) && (Index < GetNumProperties())) {
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

void ItemProperties::LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    int NumProperties;

    // Serialization Load
    // Load the number of properties
    DataStreamIn >> NumProperties;

    // Load each individual property
    for (int x = 0; x < NumProperties; x++) {
        // Create the new ItemProperty
        ItemProperty* PropItem = new ItemProperty(this, DataStreamIn, ProjectFileVersion);

        // Add it to the list
        m_PropertyList.push_back(PropItem);
    }
}

void ItemProperties::PropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback)
{
    // This property has changed, tell the Graphical Item if the PerformCallback is true
    if (PerformCallback == true) {
        GetParentGraphicItemBase()->PropertyChanged(PropName, PropNewValue);
    }

    CheckIfDynamicPropertyChanged(PropName, PropNewValue, PerformCallback);
}

void ItemProperties::CheckIfDynamicPropertyChanged(QString PropName, QString PropNewValue, bool PerformCallback)
{
    int           x;
    ItemProperty* Property;
    int           NumNewInstances;
    QStringList   ProcessedPropertyNamesList;
    QString       PropertyOriginalName;

    // Now check to see if this Property is a controlling Property for any dynamic Property
    for (x = 0; x < m_PropertyList.count(); x++) {
        Property = m_PropertyList.at(x);
        PropertyOriginalName = Property->GetOriginalPropertyName();

        // See if we have already processed this property
        if (ProcessedPropertyNamesList.contains(PropertyOriginalName) == false) {
            // See if the property Name matches a controlling Property name
            if ((PropName == Property->GetControllingProperty()) && (Property->GetDynamicFlag() == true)) {
                // Get the number of current instances of the dynamic property
                NumNewInstances = PropNewValue.toInt();

                // Adust Property in Main array
                AdjustDynamicPropertyInList(Property->GetName(), NumNewInstances, PerformCallback);

                // Reset the loop and do it again
                x = -1;
            }
        }
        ProcessedPropertyNamesList.append(PropertyOriginalName);
    }
}

void ItemProperties::AdjustDynamicPropertyInList(QString PropertyName, int NumInstances, bool PerformCallback)
{
    int           x;
    int           StartingListIndex;
    ItemProperty* CurrentProperty;
    ItemProperty* NewProperty;
    int           CurrentInstances;
    QString       OrigPropertyName;
    QString       NewName;
    QString       RemoveName;
    bool          AdjustmentMade = false;

    // Find the Index of the Dynamic Property Name and a pointer to the object.  Note: Name will either have a %d or a 0 inside it
    StartingListIndex = GetPropertyIndex(PropertyName);
    CurrentProperty = GetProperty(PropertyName);
    CurrentInstances = CurrentProperty->GetNumInstances();
    OrigPropertyName = CurrentProperty->GetOriginalPropertyName();

    // Do we add new instances
    if (NumInstances > CurrentInstances) {
        for (x = CurrentInstances; x < NumInstances; x++) {
            // Replace the %d in the original name with the new Index
            NewName = OrigPropertyName;
            NewName.replace("%d", QString("%1").arg(x));

            if (x == 0) {
                // Special case for the first entry
                CurrentProperty->SetName(NewName);
            } else {
                // Create the new ItemProperty
                NewProperty = new ItemProperty(this, NewName, CurrentProperty->GetOriginalPropertyName(), CurrentProperty->GetDefaultValue(), CurrentProperty->GetDesc(), CurrentProperty->GetReadOnly(), CurrentProperty->GetProtected(), CurrentProperty->GetExportable(), true, CurrentProperty->GetControllingProperty());

                // Add it to the list at the right location
                m_PropertyList.insert(StartingListIndex + x, NewProperty);
            }
            AdjustmentMade = true;
        }
    }

    // Do we remove instances
    if (NumInstances < CurrentInstances) {
        for (x = CurrentInstances - 1; x >= NumInstances; x--) {
            RemoveName = OrigPropertyName;
            RemoveName.replace("%d", QString("%1").arg(x));
            StartingListIndex = GetPropertyIndex(RemoveName);
            CurrentProperty = GetProperty(RemoveName);

            if (x == 0) {
                // Special Case for the first entry
                CurrentProperty->SetName(OrigPropertyName);
            } else {
                // Figure out what the name that we want to remove
                m_PropertyList.removeAt(StartingListIndex);
                delete CurrentProperty;
            }
            AdjustmentMade = true;
        }
    }

    // Perform the callback if an adjustment was made
    if (AdjustmentMade == true) {
        // Go through all properties with the original name we first processed and update the number of instances
        for (x = 0; x < m_PropertyList.count(); x++)
        {
            CurrentProperty = m_PropertyList.at(x);
            if (CurrentProperty->GetOriginalPropertyName() == OrigPropertyName) {
                CurrentProperty->SetNumInstances(NumInstances);
            }
        }

        if (PerformCallback == true) {
            m_ParentGraphicItemBase->DynamicPropertiesChanged(this);
        }
    }
}

void ItemProperties::AddStaticProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable)
{
    ItemProperty* PropItem;

    // Check to see that the Property Name is unique
    if (IsPropertyNameNotInList(PropertyName) == true) {

        // Create the new ItemProperty
        PropItem = new ItemProperty(this, PropertyName, PropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable, false, "");

        // Add it to the list
        m_PropertyList.push_back(PropItem);
    }
}

void ItemProperties::AddDynamicProperty(QString PropertyName, QString PropertyValue, QString PropertyDesc, bool ReadOnly, bool Protected, bool Exportable, QString ControllingProperty)
{
    ItemProperty* PropItem;

    // Check to see that the Property Name is unique
    if (IsPropertyNameNotInList(PropertyName) == true) {

        // Create the new ItemProperty
        PropItem = new ItemProperty(this, PropertyName, PropertyName, PropertyValue, PropertyDesc, ReadOnly, Protected, Exportable, true, ControllingProperty);

        // Add it to the list
        m_PropertyList.push_back(PropItem);
    }
}

bool ItemProperties::IsPropertyNameNotInList(QString PropertyName)
{
    int           x;

    // See if the property name is already in the list
    for (x = 0; x < m_PropertyList.count(); x++) {
        if (PropertyName == m_PropertyList.at(x)->GetName()) {
            return false;
        }
    }
    return true;
}
