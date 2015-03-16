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

#include "entrydata.h"

EntryData::EntryData(QString& Name)
{
    // Initialize the Variables
    m_ConfigEntryName = Name;
    m_ConfigVaribleName = "";
    m_ConfigInfoText = "";
    m_ConfigVariableExists = false;
    m_ConfigVariableRequired = true;
    m_DirButtonEnabled = false;
    m_ConfigEntryLineNumber = 0;

    m_UserEntrySelected = false;
    m_UserEntryVarData = "";

    m_EntryNameCheckBox = NULL;
    m_EntryVarDataEdit = NULL;
    m_EntryVarDataButton = NULL;
}

EntryData::~EntryData()
{
}

QString& EntryData::getConfigEntryName()
{
    return m_ConfigEntryName;
}

QString& EntryData::getConfigVariableName()
{
    return m_ConfigVaribleName;
}

QString& EntryData::getConfigInfoText()
{
    return m_ConfigInfoText;
}

bool EntryData::getConfigVariableExists()
{
    return m_ConfigVariableExists;
}

bool EntryData::getConfigVariableRequired()
{
    return m_ConfigVariableRequired;
}

bool EntryData::getDirButtonEnabled()
{
    return m_DirButtonEnabled;
}

int EntryData::getConfigEntryLineNumber()
{
    return m_ConfigEntryLineNumber;
}

void EntryData::setConfigEntryName(QString newName)
{
    m_ConfigEntryName = newName;
}

void EntryData::setConfigVariableName(QString newName)
{
    m_ConfigVaribleName = newName;
}

void EntryData::setConfigInfoText(QString newInfo)
{
    m_ConfigInfoText = newInfo;
}

void EntryData::setConfigVariableExists(bool flag)
{
    m_ConfigVariableExists = flag;
}

void EntryData::setConfigVariableRequired(bool flag)
{
    m_ConfigVariableRequired = flag;
}

void EntryData::setDirButtonEnabled(bool flag)
{
    m_DirButtonEnabled = flag;
}

void EntryData::setConfigEntryLineNumber(int number)
{
    m_ConfigEntryLineNumber = number;
}

bool    EntryData::getUserEntrySelected()
{
    return m_UserEntrySelected;
}

QString EntryData::getUserEntryVarData()
{
    return m_UserEntryVarData;
}

void EntryData::setUserEntrySelected(bool flag)
{
    m_UserEntrySelected = flag;
}

void EntryData::setUserEntryVarData(QString data)
{
    m_UserEntryVarData = data;
}

QCheckBox*   EntryData::getControlEntryNameCheckBox()
{
    return m_EntryNameCheckBox;
}

QLineEdit*   EntryData::getControlEntryVarDataEdit()
{
    return m_EntryVarDataEdit;
}

QPushButton* EntryData::getControlEntryVarDataButton()
{
    return m_EntryVarDataButton;
}

void EntryData::setControlEntryNameCheckBox(QCheckBox* ptr)
{
    m_EntryNameCheckBox = ptr;
}

void EntryData::setControlEntryVarDataEdit(QLineEdit* ptr)
{
    m_EntryVarDataEdit = ptr;
}

void EntryData::setControlEntryVarDataButton(QPushButton* ptr)
{
    m_EntryVarDataButton = ptr;
}


