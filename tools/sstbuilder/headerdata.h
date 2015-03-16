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

#ifndef HEADERDATA_H
#define HEADERDATA_H

#include "entrydata.h"

class HeaderData
{
public:
    explicit HeaderData(QString& HeaderName, QString& HeaderType);
    ~HeaderData();

    void       addEntryData(EntryData* pEntryData);

    QString&   getHeaderName();
    QString&   getHeaderType();
    int        getNumEntries();
    EntryData* getEntry(int index);

private:
    QString m_HeaderName;
    QString m_HeaderType;
    QList<EntryData*> m_EntryDataList;
};

#endif // HEADERDATA_H
