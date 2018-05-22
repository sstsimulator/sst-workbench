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

#include "GraphicItemBase.h"

////////////////////////////////////////////////////////////

GraphicItemBase::GraphicItemBase(const ItemType itemType)
{
    // Init Variables
    m_ItemType = itemType;
    m_Properties = NULL;

    // Automatically Create a Properties Structure for this graphic object
    CreatePropertiesStructure();
}

GraphicItemBase::~GraphicItemBase()
{
    DeletePropertiesStructure();
}

void GraphicItemBase::DeletePropertiesStructure()
{
    if (m_Properties != NULL) {
        delete m_Properties;
    }
    m_Properties = NULL;
}

void GraphicItemBase::CreatePropertiesStructure()
{
    // Create a Properties object when requested (but only one can be created)
    if (m_Properties == NULL) {
        m_Properties = new ItemProperties(this);
    }
}

void GraphicItemBase::SetExistingPropertiesStructure(ItemProperties* ptrExistingProperties)
{
    // Check to see if properties exist and the user is assigning a new set of properties
    if ((m_Properties != NULL) && (ptrExistingProperties != NULL)) {
        // NOTE: This may orphan some memory if not used correctly.
        //       However, This is the intended design
        DeletePropertiesStructure();
    }
    m_Properties = ptrExistingProperties;
}

void GraphicItemBase::PropertyChanged(QString& PropName, QString& NewPropValue)
{
    // This is a virtual function that the derived class should implement if
    // it wants to receive notification of a property change.  The function
    // will be directly called by the ItemProperty class
    Q_UNUSED(PropName)
    Q_UNUSED(NewPropValue)
}

void GraphicItemBase::DynamicPropertiesChanged(ItemProperties* ptrExistingProperties)
{
    // This is a virtual function that the derived class should implement if
    // it wants to receive notification of a dynamic property change.  The function
    // will be directly called by the ItemProperty class
    Q_UNUSED(ptrExistingProperties)
}
