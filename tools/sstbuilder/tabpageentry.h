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

#ifndef TABPAGEENTRY_H
#define TABPAGEENTRY_H

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>

#include "persistantdata.h"
#include "headerdata.h"

class TabPageEntry : public QWidget
{
    Q_OBJECT
public:
    explicit TabPageEntry(QString PageName, QWidget *parent = 0);
    ~TabPageEntry();

    void setParentSize(QSize parentSize);
    void populateHeaderInfo(HeaderData* pHeaderData);
    HeaderData* GetHeaderData() {return m_pHeaderData;}

signals:

public slots:

private slots:
    void on_checkbox_clicked(int state);
    void on_text_changed(const QString&);
    void on_button_clicked(bool checked = false);

private:
    QVBoxLayout*   m_pVBoxLayout;
    QGridLayout*   m_pGridLayout;
    QLabel*        m_pPageName;
    HeaderData*    m_pHeaderData;
    PersistantData m_PersistData;
};

#endif // TABPAGEENTRY_H
