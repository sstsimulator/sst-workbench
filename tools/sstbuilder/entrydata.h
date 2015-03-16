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

#ifndef ENTRYDATA_H
#define ENTRYDATA_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

class EntryData
{
public:
    // Constructor
    explicit EntryData(QString& Name);
    ~EntryData();

    // Get and Sets for Configuration File Data
    QString& getConfigEntryName();
    QString& getConfigVariableName();
    QString& getConfigInfoText();
    bool     getConfigVariableExists();
    bool     getConfigVariableRequired();
    bool     getDirButtonEnabled();
    int      getConfigEntryLineNumber();

    void setConfigEntryName(QString newName);
    void setConfigVariableName(QString newName);
    void setConfigInfoText(QString newInfo);
    void setConfigVariableExists(bool flag);
    void setConfigVariableRequired(bool flag);
    void setDirButtonEnabled(bool flag);
    void setConfigEntryLineNumber(int number);

    // Get and Sets for User Entered data
    bool    getUserEntrySelected();
    QString getUserEntryVarData();

    void setUserEntrySelected(bool flag);
    void setUserEntryVarData(QString data);

    // Gets and Sets of TabPage controls
    QCheckBox*   getControlEntryNameCheckBox();
    QLineEdit*   getControlEntryVarDataEdit();
    QPushButton* getControlEntryVarDataButton();

    void setControlEntryNameCheckBox(QCheckBox* ptr);
    void setControlEntryVarDataEdit(QLineEdit* ptr);
    void setControlEntryVarDataButton(QPushButton* ptr);

private:
    // Configuration File Data
    QString m_ConfigEntryName;
    QString m_ConfigVaribleName;
    QString m_ConfigInfoText;
    bool    m_ConfigVariableExists;
    bool    m_ConfigVariableRequired;
    bool    m_DirButtonEnabled;
    int     m_ConfigEntryLineNumber;

    // User Entered Data
    bool    m_UserEntrySelected;
    QString m_UserEntryVarData;

    // TabPage Controls
    QCheckBox*   m_EntryNameCheckBox;
    QLineEdit*   m_EntryVarDataEdit;
    QPushButton* m_EntryVarDataButton;
};

#endif // ENTRYDATA_H
