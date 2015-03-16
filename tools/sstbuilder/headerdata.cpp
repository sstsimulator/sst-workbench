// Copyright 2009-2015 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2015, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include "headerdata.h"

HeaderData::HeaderData(QString& HeaderName, QString& HeaderType)
{
    m_HeaderName = HeaderName;
    m_HeaderType = HeaderType;
}

HeaderData::~HeaderData()
{
    int x;
    EntryData* pEntry;

    for (x = 0; x < m_EntryDataList.count(); x++) {
        pEntry = getEntry(0);
        if (NULL != pEntry) {
            delete pEntry;
        }
        m_EntryDataList.removeFirst();
    }
}

void HeaderData::addEntryData(EntryData* pEntryData)
{
    m_EntryDataList.push_back(pEntryData);
}

QString& HeaderData::getHeaderName()
{
    return m_HeaderName;
}

QString& HeaderData::getHeaderType()
{
    return m_HeaderType;
}

int HeaderData::getNumEntries()
{
    return m_EntryDataList.count();
}

EntryData* HeaderData::getEntry(int index)
{
    return m_EntryDataList[index];
}
