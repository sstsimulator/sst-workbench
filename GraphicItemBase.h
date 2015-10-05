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

#ifndef GRAPHICITEMBASE_H
#define GRAPHICITEMBASE_H

#include "GlobalIncludes.h"

// Forward declarations to allow compile
class ItemProperties;

////////////////////////////////////////////////////////////

class GraphicItemBase
{
public:
    // Constructor / Destructor
    GraphicItemBase(const GraphicItemType_enum itemType);
    virtual ~GraphicItemBase();

    // Return the Item Type
    GraphicItemType_enum GetItemType() const {return m_ItemType;}

    // Return the Item Properties
    ItemProperties* GetItemProperties() {return m_Properties;}

    // Properties Callback: Derived class should implement this function
    // if it wants to receive notification of changes to a property
    virtual void PropertyChanged(QString& PropName, QString& NewPropValue);

    // Properties Callback: Derived class should implement this function
    // if it wants to be notified that dynamic properties have changed and should
    // be refreshed
    virtual void DynamicPropertiesChanged(ItemProperties* ptrExistingProperties);

    // Required Virtual Function to save Item Data
    virtual void SaveData(QDataStream& DataStreamOut) = 0;

protected:
    // Override this items properties with properties from somewhere else
    void SetExistingPropertiesStructure(ItemProperties* ptrExistingProperties);

private:
    // Functions to handle creation/ deletions of Properties structure
    void CreatePropertiesStructure();
    void DeletePropertiesStructure();

private:
    GraphicItemType_enum  m_ItemType;
    ItemProperties*       m_Properties;
};

#endif // GRAPHICITEMBASE_H
