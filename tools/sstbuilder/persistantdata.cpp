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

#include "persistantdata.h"

#define SSTINIFILENAME      "/.sstbuilder"
#define SSTPERSISTANCEVER   "__SSTBUILDER_1_0__"

// Static Variables
bool                     PersistantData::m_PersistantDataLoaded = false;
QSettings*               PersistantData::m_pCurrentINIFile = NULL;
QString                  PersistantData::m_SSTPersistanceFileVersion;
QString                  PersistantData::m_SSTRootPath;
QStringList              PersistantData::m_ConfigFileHeaderNameList;
QStringList              PersistantData::m_ConfigFileHeaderTypeList;
QStringList              PersistantData::m_IgnoredEntryNameList;
QStringList              PersistantData::m_EntryVarsThatEnableDirOptionList;
QStringList              PersistantData::m_EnvGroupNamesList;
QList<EnvGroupData*>     PersistantData::m_EnvGroupDataList;
QStringList              PersistantData::m_TabPageNamesList;
QList<TabPageEntryData*> PersistantData::m_TabPageEntryDataList;
bool                     PersistantData::m_GeneralInfoDirty = false;
bool                     PersistantData::m_GroupEnvInfoDirty = false;
bool                     PersistantData::m_TabPageEntryInfoDirty = false;

PersistantData::PersistantData()
{
    QString DefaultDataFile;

    DefaultDataFile = GetDefaultPersistantDataFile();

    // Since all internal data variables are static, only load the data from the
    // default location only once for all instantations
    if (false == m_PersistantDataLoaded) {
        // Initialize data with the Default Persistant Data
        LoadPersistantDataFromFile(DefaultDataFile);
        m_PersistantDataLoaded = true;
    }
}

PersistantData::~PersistantData()
{
    // Save any data when then class is finished
    SaveAllPersistantData();
}















bool PersistantData::LoadPersistantDataFromFile(QString FilePath)
{
    QString Version;

    // Close the existing INI file if it exists
    CloseINIFile();

    ClearAllInternalPersistantData();

    // Load the data from the Selected File
    OpenINIFile(FilePath);

    // Now check to see if the file is valid
    m_SSTPersistanceFileVersion = RetrieveData("SSTBUILDER", "VERSION");
    if (SSTPERSISTANCEVER == m_SSTPersistanceFileVersion) {
        // We have loaded a valid file, Load it
        LoadAllPersistantData();
    } else {
        // We have loaded something (maybe not a valid file), save it to the default location
        LoadAllPersistantData();
        SavePersistantDataToFile();
    }
    return true;
}

bool PersistantData::SavePersistantDataToFile(QString FilePath /*= ""*/)
{
    if ("" != FilePath) {
        // Open the INI file identified by the FilePath
        OpenINIFile(FilePath);
    }
    // Set the dirty flags to force object to save all data
    SetAllDirtyFlags();
    SaveAllPersistantData();
    return true;
}

bool PersistantData::SavePersistantData()
{
    // Just save the data
    SaveAllPersistantData();
    return true;
}

QString PersistantData::GetDefaultPersistantDataFile()
{
    QString RtnString;

    RtnString = QDir::homePath() + SSTINIFILENAME;
    return RtnString;
}

QString PersistantData::GetSSTRootPath()
{
    return m_SSTRootPath;
}

QStringList PersistantData::GetConfigFileHeaderNameList()
{
    return m_ConfigFileHeaderNameList;
}

QStringList PersistantData::GetConfigFileHeaderTypeList()
{
    return m_ConfigFileHeaderTypeList;
}

QStringList PersistantData::GetIgnoredEntryNameList()
{
    return m_IgnoredEntryNameList;
}

QStringList PersistantData::GetEntryVarsThatEnableDirOptionList()
{
    return m_EntryVarsThatEnableDirOptionList;
}

QStringList PersistantData::GetEnvGroupNamesList()
{
    return m_EnvGroupNamesList;
}

QStringList PersistantData::GetEnvGroupEnablesList()
{
    int           x;
    EnvGroupData* pData;
    QStringList   rtnList;

    // Look at all the Group Names and Build a QStringList
    for (x = 0; x < m_EnvGroupDataList.count(); x++) {
        pData =  m_EnvGroupDataList[x];
        rtnList += pData->getGroupEnabled();
    }

    return rtnList;
}

QStringList PersistantData::GetEnvVarNamesList(QString GroupName)
{
    int           x;
    QStringList   rtnList;

    // Look at all the Group Names and Build a QStringList
    for (x = 0; x < m_EnvGroupDataList.count(); x++) {
        if (m_EnvGroupDataList[x]->getGroupName() == GroupName) {
            rtnList = m_EnvGroupDataList[x]->getEnvVarNamesList();
        }
    }

    return rtnList;
}

QStringList PersistantData::GetEnvVarValuesList(QString GroupName)
{
    int           x;
    QStringList   rtnList;

    // Look at all the Group Names and Build a QStringList
    for (x = 0; x < m_EnvGroupDataList.count(); x++) {
        if (m_EnvGroupDataList[x]->getGroupName() == GroupName) {
            rtnList = m_EnvGroupDataList[x]->getEnvVarValuesList();
        }
    }

    return rtnList;
}

QString PersistantData::GetEntryValue(QString TabPageName, QString EntryName)
{
    QString            DataString;
    int                indexPage;
    int                indexEntry;
    TabPageEntryData*  pTabPageEntryData;

    // Now find the index of the tabpage
    indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);

    // Did we find any entries
    if (-1 == indexPage) {
        return "";
    }

    // Get a pointer to the object
    pTabPageEntryData = m_TabPageEntryDataList[indexPage];

    // Now find the index of the Entry on this TabPage
    indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);

    // Did we find any entries
    if (-1 == indexEntry) {
        return "";
    }

    // Finally Get the data
    DataString = pTabPageEntryData->getTabPageEntryValuesList()->at(indexEntry);

    // Final Return
    return DataString;
}

bool PersistantData::GetEntryState(QString TabPageName, QString EntryName)
{
    QString            DataString;
    int                indexPage;
    int                indexEntry;
    TabPageEntryData*  pTabPageEntryData;

    // Now find the index of the tabpage
    indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);

    // Did we find any entries
    if (-1 == indexPage) {
        return false;
    }

    // Get a pointer to the object
    pTabPageEntryData = m_TabPageEntryDataList[indexPage];

    // Now find the index of the Entry on this TabPage
    indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);

    // Did we find any entries
    if (-1 == indexEntry) {
        return false;
    }

    // Finally Get the data
    DataString = pTabPageEntryData->getTabPageEntryEnablesList()->at(indexEntry);

    // Final Return
    return (DataString == "1");
}

void PersistantData::SetSSTRootPath(QString Path)
{
    m_SSTRootPath = Path;
    m_GeneralInfoDirty = true;
}

void PersistantData::SetConfigFileHeaderNameList(QStringList List)
{
    m_ConfigFileHeaderNameList = List;
    m_GeneralInfoDirty = true;
}

void PersistantData::SetConfigFileHeaderTypeList(QStringList List)
{
    m_ConfigFileHeaderTypeList = List;
    m_GeneralInfoDirty = true;
}

void PersistantData::SetIgnoredEntryNameList(QStringList List)
{
    m_IgnoredEntryNameList = List;
    m_GeneralInfoDirty = true;
}

void PersistantData::SetEntryVarsThatEnableDirOptionList(QStringList List)
{
    m_EntryVarsThatEnableDirOptionList = List;
    m_GeneralInfoDirty = true;
}

void PersistantData::SetEnvGroupNameAndEnable(QString GroupName, QString GroupEnable)
{
    FillEnvGroupNameAndEnable(GroupName, GroupEnable);
    m_GroupEnvInfoDirty = true;
}

void PersistantData::SetEnvVarNamesList(QString GroupName, QStringList List)
{
    FillEnvVarDataNames(GroupName, List);
    m_GroupEnvInfoDirty = true;
}

void PersistantData::SetEnvVarValuesList(QString GroupName, QStringList List)
{
    FillEnvVarDataValues(GroupName, List);
    m_GroupEnvInfoDirty = true;
}

void PersistantData::SetEntryValue(QString TabPageName, QString EntryName, QString Value)
{
    FillEntryDataValue(TabPageName, EntryName, Value);
    m_TabPageEntryInfoDirty = true;
}

void PersistantData::SetEntryState(QString TabPageName, QString EntryName, bool Enabled)
{
    FillEntryDataState(TabPageName, EntryName, Enabled);
    m_TabPageEntryInfoDirty = true;
}

void PersistantData::RemoveEnvGroup(QString GroupName)
{
    int x;
    int count;
    EnvGroupData*     pEnvGroupData;

    // Get the number of GroupNames
    count = m_EnvGroupNamesList.count();

    // Set all the group data dirty
    m_GroupEnvInfoDirty = true;
    for (x = 0; x < count; x++) {
        m_EnvGroupDataList[x]->setDirtyFlag(true);
    }

    // Search the Group Names list for the specific group,
    // Then delete that index from both the GroupNamesList
    // and the GroupDataList
    for (x = 0; x < count; x++) {
        if (m_EnvGroupNamesList[x] == GroupName) {
            // We found the name, so delete it from both lists
            m_EnvGroupNamesList.removeAt(x);
            // Get a ptr to the EnvGroupData Object and then delete it
            pEnvGroupData = m_EnvGroupDataList.takeAt(x);
            delete pEnvGroupData;
            return; // We got it, so dont do any more
        }
    }
}

void PersistantData::RenameEnvGroup(QString OldGroupName, QString NewGroupName)
{
    int x;
    int count;
    EnvGroupData*     pEnvGroupData;

    // Get the number of GroupNames
    count = m_EnvGroupNamesList.count();

    // Set all the group data dirty
    m_GroupEnvInfoDirty = true;
    for (x = 0; x < count; x++) {
        m_EnvGroupDataList[x]->setDirtyFlag(true);
    }

    // Search the Group Names list for the specific group, then rename it
    for (x = 0; x < count; x++) {
        if (m_EnvGroupNamesList[x] == OldGroupName) {
            m_EnvGroupNamesList[x] = NewGroupName;

            // Change the name in the group data
            pEnvGroupData = m_EnvGroupDataList[x];
            pEnvGroupData->setGroupName(NewGroupName);
        }
    }
}

void PersistantData::CopyEnvGroup(QString ExistingGroupName, QString NewGroupName, QString GroupEnable)
{
    QStringList   EnvVarNames;
    QStringList   EnvVarValues;

    // Add the new Group Name
    AddNewEnvGroupNameAndEnable(NewGroupName, GroupEnable);

    // Copy the Group Env Var Names
    EnvVarNames = GetEnvVarNamesList(ExistingGroupName);
    SetEnvVarNamesList(NewGroupName, EnvVarNames);

    // Copy the Group Env Var Values
    EnvVarValues = GetEnvVarValuesList(ExistingGroupName);
    SetEnvVarValuesList(NewGroupName, EnvVarValues);
}

void PersistantData::AddNewEnvGroupNameAndEnable(QString GroupName, QString GroupEnable)
{
    EnvGroupData* pData;

    // The GroupName does not exist, add a new one, and set its enable
    m_EnvGroupNamesList += GroupName;

    pData = new EnvGroupData(GroupName);
    pData->setGroupEnabled(GroupEnable);
    // Add it to the list
    m_EnvGroupDataList.append(pData);
}

void PersistantData::FillEnvGroupNameAndEnable(QString GroupName, QString GroupEnable)
{
    int           indexFound;

    // See if the GroupName already exists
    indexFound = m_EnvGroupNamesList.indexOf(GroupName, 0);
    if (indexFound < 0) {
        // The GroupName does not exist, add a new one, and set its enable
        AddNewEnvGroupNameAndEnable(GroupName, GroupEnable);
    } else {
        // Double check that the index matches (possible if the group name has data, but data not yet filled in)
        for (int x = 0; x < m_EnvGroupDataList.count(); x++) {
            if (m_EnvGroupDataList[x]->getGroupName() == GroupName) {
                // We found the name, do the indexes match
                if (indexFound == x) {
                    // Set the enabled flag for this group
                    m_EnvGroupDataList[indexFound]->setGroupEnabled(GroupEnable);
                    break; // out of the for loop
                }
            }
        }
    }
}

void PersistantData::FillEnvVarDataNames(QString GroupName, QStringList Names)
{
    for (int x = 0; x < m_EnvGroupDataList.count(); x++) {
        if (m_EnvGroupDataList[x]->getGroupName() == GroupName) {
            m_EnvGroupDataList[x]->setEnvVarNamesList(Names);
        }
    }
}

void PersistantData::FillEnvVarDataValues(QString GroupName, QStringList Values)
{
    for (int x = 0; x < m_EnvGroupDataList.count(); x++) {
        if (m_EnvGroupDataList[x]->getGroupName() == GroupName) {
            m_EnvGroupDataList[x]->setEnvVarValuesList(Values);
        }
    }
}

void PersistantData::FillEntryDataState(QString TabPageName, QString EntryName, bool Enabled)
{
    int                indexPage;
    int                indexEntry;
    TabPageEntryData*  pTabPageEntryData;

    // Search the m_TabPageNamesList for the TabPageName to see if it already exists
    indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);
    if (indexPage < 0) {
        // The name has not been added yet, we need to add it to the m_TabPageNamesList
        m_TabPageNamesList += TabPageName;
        // Also we need to create a new TabPageEntryData object
        m_TabPageEntryDataList += new TabPageEntryData(TabPageName);
        // And get the index
        indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);
    }

    // Get a pointer to the object
    pTabPageEntryData = m_TabPageEntryDataList[indexPage];

    // Now search the Entry Names to see if it already exists
    indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);
    if (indexEntry < 0) {
        // Create new entrys for the EntryName, Value, and Enable flag
        pTabPageEntryData->addTabPageEntryName(EntryName);
        pTabPageEntryData->addTabPageEntryValue("");
        pTabPageEntryData->addTabPageEntryEnable(Enabled ? "1" : "0");
        // Get the Index of the new data
        indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);
    } else {
        pTabPageEntryData->setTabPageEntryEnable(indexEntry, Enabled ? "1" : "0");
    }
}

void PersistantData::FillEntryDataValue(QString TabPageName, QString EntryName, QString Value)
{
    int                indexPage;
    int                indexEntry;
    TabPageEntryData*  pTabPageEntryData;

    // Search the m_TabPageNamesList for the TabPageName to see if it already exists
    indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);
    if (indexPage < 0) {
        // The name has not been added yet, we need to add it to the m_TabPageNamesList
        m_TabPageNamesList += TabPageName;
        // Also we need to create a new TabPageEntryData object
        m_TabPageEntryDataList += new TabPageEntryData(TabPageName);
        // And get the index
        indexPage = m_TabPageNamesList.indexOf(TabPageName, 0);
    }

    // Get a pointer to the object
    pTabPageEntryData = m_TabPageEntryDataList[indexPage];

    // Now search the Entry Names to see if it already exists
    indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);
    if (indexEntry < 0) {
        // Create new entrys for the EntryName, Value, and Enable flag
        pTabPageEntryData->addTabPageEntryName(EntryName);
        pTabPageEntryData->addTabPageEntryValue(Value);
        pTabPageEntryData->addTabPageEntryEnable("0");
        // Get the Index of the new data
        indexEntry = pTabPageEntryData->getTabPageEntryNamesList()->indexOf(EntryName, 0);
    } else {
        pTabPageEntryData->setTabPageEntryValue(indexEntry, Value);
    }
}

void PersistantData::SetAllDirtyFlags()
{
    int x;

    m_GeneralInfoDirty = true;
    m_GroupEnvInfoDirty = true;
    m_TabPageEntryInfoDirty = true;

    for (x = 0; x < m_EnvGroupNamesList.count(); x++) {
        m_EnvGroupDataList[x]->setDirtyFlag(true);
    }

    for (x = 0; x < m_TabPageNamesList.count(); x++) {
        m_TabPageEntryDataList[x]->setDirtyFlag(true);
    }
}

void PersistantData::ClearAllDirtyFlags()
{
    int x;

    m_GeneralInfoDirty = false;
    m_GroupEnvInfoDirty = false;
    m_TabPageEntryInfoDirty = false;

    for (x = 0; x < m_EnvGroupNamesList.count(); x++) {
        m_EnvGroupDataList[x]->setDirtyFlag(false);
    }

    for (x = 0; x < m_TabPageNamesList.count(); x++) {
        m_TabPageEntryDataList[x]->setDirtyFlag(false);
    }
}

void PersistantData::ClearAllInternalPersistantData()
{
    int x;
    int count;
    EnvGroupData*     pEnvGroupData;
    TabPageEntryData* pTabPageEntryData;

    if (NULL != m_pCurrentINIFile) {
        delete m_pCurrentINIFile;
        m_pCurrentINIFile = NULL;
    }

    m_ConfigFileHeaderNameList = QStringList();
    m_ConfigFileHeaderTypeList = QStringList();
    m_IgnoredEntryNameList = QStringList();
    m_EntryVarsThatEnableDirOptionList = QStringList();

    m_EnvGroupNamesList = QStringList();
    m_TabPageNamesList = QStringList();

    m_PersistantDataLoaded = false;
    m_SSTPersistanceFileVersion = "";
    m_SSTRootPath = "";

    m_GeneralInfoDirty = false;
    m_GroupEnvInfoDirty = false;
    m_TabPageEntryInfoDirty = false;

    count = m_EnvGroupDataList.count();
    for (x = 0; x < count; x++) {
        // Get a ptr to the EnvGroupData Object and then delete it
        pEnvGroupData = m_EnvGroupDataList.takeAt(0);
        delete pEnvGroupData;
    }

    count = m_TabPageEntryDataList.count();
    for (x = 0; x < count; x++) {
        // Get a ptr to the TabPageEntry Object and then delete it
        pTabPageEntryData = m_TabPageEntryDataList.takeAt(0);
        delete pTabPageEntryData;
    }
}

void PersistantData::LoadAllPersistantData()
{
    QStringList DefaultVarEmpty;
    QStringList DefaultHeaderNameList;
    QStringList DefaultHeaderTypeList;
    QStringList DefaultIgnoredEntryNameList;
    QStringList DefaultVarsThatEnableOptionsList;

    int         NumEntries;
    int         x;
    int         y;
    QString     SectionName;

    QString     GroupName;
    QString     GroupEnabled;
    QStringList GroupNamesList;
    QStringList EnvNamesList;
    QStringList EnvValuesList;

    QString     TabName;
    QStringList TabNamesList;
    QStringList EntryNamesList;
    QStringList EntryValuesList;
    QStringList EntryEnablesList;

    // Build the list of Default headers to look for in the config file, in case the file has no data yet
    DefaultHeaderNameList += "Installation directories:";
    DefaultHeaderNameList += "Fine tuning of the installation directories:";
    DefaultHeaderNameList += "Program names:";
    DefaultHeaderNameList += "System types:";
    DefaultHeaderNameList += "Optional Features:";
    DefaultHeaderNameList += "Optional Packages:";
    DefaultHeaderNameList += "Some influential environment variables:";

    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Normal";
    DefaultHeaderTypeList += "Special";

    DefaultIgnoredEntryNameList += "disable-FEATURE";
    DefaultIgnoredEntryNameList += "enable-FEATURE";
    DefaultIgnoredEntryNameList += "with-PACKAGE";
    DefaultIgnoredEntryNameList += "without-PACKAGE";

    DefaultVarsThatEnableOptionsList += "PREFIX =";
    DefaultVarsThatEnableOptionsList += "EPREFIX =";
    DefaultVarsThatEnableOptionsList += "DIR =";
    DefaultVarsThatEnableOptionsList += "DIR@ =";
    DefaultVarsThatEnableOptionsList += "LIB_DIR =";

    /////////////////////////////////////////////////////////////////////////////////

    // Save the SST Root Path (use the users home directory as default)
    m_SSTRootPath = RetrieveData("SSTBUILDER", "SSTROOTPATH", QDir::homePath());

    // Load the Config File Header Name List
    m_ConfigFileHeaderNameList = RetrieveDataBlock("HeaderNames", "Header", DefaultHeaderNameList);
    m_ConfigFileHeaderTypeList = RetrieveDataBlock("HeaderNames", "Type", DefaultHeaderTypeList);

    // Other General Information
    m_IgnoredEntryNameList = RetrieveDataBlock("IgnoredEntryNames", "Name", DefaultIgnoredEntryNameList);
    m_EntryVarsThatEnableDirOptionList = RetrieveDataBlock("EntryVarsThatEnableDirOption", "Name", DefaultVarsThatEnableOptionsList);

    // Load the Env Groups
    NumEntries = RetrieveData("EnvGroups", "NumEntries", "0").toInt();
    GroupNamesList = RetrieveDataBlock("EnvGroups", "Name", DefaultVarEmpty, NumEntries);

    // Load all entries for each Group Name in a section and Build the strings of Enables
    for (x = 0; x < GroupNamesList.count(); x++) {
        // Get the Tab Name and Section Name
        GroupName = GroupNamesList[x];
        SectionName = "GroupEnvVariables_" + GroupName;

        // Get the Enabled flag and set the GroupName and Enable
        GroupEnabled = RetrieveData(SectionName, "GroupEnabled", "0");
        FillEnvGroupNameAndEnable(GroupName, GroupEnabled);

        // Get the number of entries for the Entry Names of this particualar Group
        NumEntries = RetrieveData(SectionName, "NumEntries", "0").toInt();

        // Get the Entry Name, Enabled flag, and Value data into string lists
        EnvNamesList = RetrieveDataBlock(SectionName, "EnvName", DefaultVarEmpty, NumEntries);
        EnvValuesList = RetrieveDataBlock(SectionName, "EnvValue", DefaultVarEmpty, NumEntries);

        // Now add them into the data structures
        FillEnvVarDataNames(GroupName, EnvNamesList);
        FillEnvVarDataValues(GroupName, EnvValuesList);
    }

    // Get the Tab Names
    NumEntries = RetrieveData("TabPageNames", "NumEntries", "0").toInt();
    TabNamesList = RetrieveDataBlock("TabPageNames", "TabName", DefaultVarEmpty, NumEntries);

    // Load all entries for each Tab Name in a section
    for (x = 0; x < TabNamesList.count(); x++) {
        // Get the Tab Name and Section Name
        TabName = TabNamesList[x];
        SectionName = "TabPageEntries_" + TabName;

        // Get the number of entries for this particuallar tab
        NumEntries = RetrieveData(SectionName, "NumEntries", "0").toInt();

        // Get the Entry Name, Enabled flag, and Value data into string lists
        EntryNamesList = RetrieveDataBlock(SectionName, "EntryName", DefaultVarEmpty, NumEntries);
        EntryValuesList = RetrieveDataBlock(SectionName, "EntryValue", DefaultVarEmpty, NumEntries);
        EntryEnablesList = RetrieveDataBlock(SectionName, "EntryEnabled", DefaultVarEmpty, NumEntries);

        // Now add them into the data structures
        for (y = 0; y < EntryNamesList.count(); y++) {
            QString EntryName = EntryNamesList[y];
            QString EntryValue = EntryValuesList[y];
            QString EntryEnable = EntryEnablesList[y];
            FillEntryDataValue(TabName, EntryName, EntryValue);
            FillEntryDataState(TabName, EntryName, EntryEnable == "1");
        }
    }
}

void PersistantData::SaveAllPersistantData()
{
    int               x;
    QString           SectionName;

    QString           GroupName;
    QString           GroupEnabled;
    EnvGroupData*     pEnvGroupData;

    QString           TabName;
    TabPageEntryData* pTabPageEntryData;

    // Check to see if all of the data is dirty
    if (m_GeneralInfoDirty && m_GroupEnvInfoDirty && m_TabPageEntryInfoDirty) {
        // Clear all of the data
        m_pCurrentINIFile->clear();
    }

    if (true == m_GeneralInfoDirty) {
        // Save the Version Information
        SaveData("SSTBUILDER", "VERSION", SSTPERSISTANCEVER);

        // Save SST Root Path
        SaveData("SSTBUILDER", "SSTROOTPATH", m_SSTRootPath);

        // Save the Header Name Info
        SaveDataBlock("HeaderNames", "Type", m_ConfigFileHeaderTypeList);
        SaveDataBlock("HeaderNames", "Header", m_ConfigFileHeaderNameList);

        // Other General Information
        SaveDataBlock("IgnoredEntryNames", "Name", m_IgnoredEntryNameList);
        SaveDataBlock("EntryVarsThatEnableDirOption", "Name", m_EntryVarsThatEnableDirOptionList);

        m_GeneralInfoDirty = false;
    }


    // Save the  Env Groups and their associated data
    if (true == m_GroupEnvInfoDirty) {
        SaveDataBlock("EnvGroups", "Name", m_EnvGroupNamesList);
        for (x = 0; x < m_EnvGroupNamesList.count(); x++) {
            // Get the Group Name and Section Name
            GroupName = m_EnvGroupNamesList[x];
            SectionName = "GroupEnvVariables_" + GroupName;

            // Get a ptr to the EnvGroupData Object
            pEnvGroupData = m_EnvGroupDataList[x];

            if (true == m_EnvGroupDataList[x]->getDirtyFlag()) {
                // Save the data for the EnvGroupData object
                GroupEnabled = pEnvGroupData->getGroupEnabled();
                SaveData(SectionName, "GroupEnabled", GroupEnabled);

                SaveDataBlock(SectionName, "EnvName", pEnvGroupData->getEnvVarNamesList());
                SaveDataBlock(SectionName, "EnvValue", pEnvGroupData->getEnvVarValuesList());

                m_EnvGroupDataList[x]->setDirtyFlag(false);
            }

            m_GroupEnvInfoDirty = false;
        }
    }

    // Save the Tab Names
    if (true == m_TabPageEntryInfoDirty) {
        SaveDataBlock("TabPageNames", "TabName", m_TabPageNamesList);
        // Save all entries for each Tab Name in a section
        for (x = 0; x < m_TabPageNamesList.count(); x++) {
            // Get the Tab Name and Section Name
            TabName = m_TabPageNamesList[x];
            SectionName = "TabPageEntries_" + TabName;

            // Get a ptr to the TabPageEntryData Object
            pTabPageEntryData = m_TabPageEntryDataList[x];

            if (true == m_TabPageEntryDataList[x]->getDirtyFlag()) {
                SaveDataBlock(SectionName, "EntryName", *pTabPageEntryData->getTabPageEntryNamesList());
                SaveDataBlock(SectionName, "EntryValue", *pTabPageEntryData->getTabPageEntryValuesList());
                SaveDataBlock(SectionName, "EntryEnabled", *pTabPageEntryData->getTabPageEntryEnablesList());

                m_TabPageEntryDataList[x]->setDirtyFlag(false);
            }

            m_TabPageEntryInfoDirty = false;
        }
    }
}

void PersistantData::OpenINIFile(QString FilePath)
{
    // Open the new Data File
    m_pCurrentINIFile = new QSettings(FilePath, QSettings::IniFormat);
}

void PersistantData::CloseINIFile()
{
    // Do we already have an Data File opened?
    if (NULL != m_pCurrentINIFile) {
        // Save any data we have to the currently open INI file
        SavePersistantDataToFile();

        // Delete the file
        delete m_pCurrentINIFile;
        m_pCurrentINIFile = NULL;
    }
}

void PersistantData::SaveData(QString Section, QString Key, QString Value)
{
    QString Newsection = Section;
    QString Newkey = Key;
    QString Newvalue = Value;

    // Get the data from the file
    m_pCurrentINIFile->beginGroup(Section);
    m_pCurrentINIFile->setValue(Key, Value);
    m_pCurrentINIFile->endGroup();

    // Write out the file
    m_pCurrentINIFile->sync();
}

void PersistantData::SaveDataBlock(QString Section, QString Key, QStringList ValueList)
{
    int     x;
    QString KeyData;

    // Save a block of data
    for (x = 0; x < ValueList.count(); x++) {
        // Build the Key that we want to find
        KeyData = Key + QString::number(x);

        SaveData(Section, KeyData, ValueList[x]);
    }

    // Save the Number of entries for this block
    SaveData(Section, "NumEntries", QString::number(ValueList.count()));
}

QString PersistantData::RetrieveData(QString Section, QString Key, QString Default /*= ""*/)
{
    QString   RtnString;

    m_pCurrentINIFile->beginGroup(Section);
    RtnString = m_pCurrentINIFile->value(Key, Default).toString();
    m_pCurrentINIFile->endGroup();

    return RtnString;
}

QStringList PersistantData::RetrieveDataBlock(QString Section, QString Key, QStringList DefaultList, int NumEntries /*= -1*/)
{
    QStringList ReturnList;
    QString     KeyData;
    QString     Value = "DEADBEEF";
    int         KeyNum = 0;
    int         x;

    if (NumEntries < 0) {
        // Read all Entries until we get an empty string for the value,  using the defaults as necessary
        while ("" != Value) {

            // Build the Key that we want to find
            KeyData = Key + QString::number(KeyNum);

            if (KeyNum < DefaultList.count()) {
                // Get the Value; Use the default if the data is not found
                Value = RetrieveData(Section, KeyData, DefaultList[KeyNum]);
            } else {
                // Get the Value with no default
                Value = RetrieveData(Section, KeyData);
            }

            if ("" != Value) {
                // Add the value read to the Header Name List
                ReturnList += Value;
            }
            KeyNum++;
        }
    } else {
        // Read NumEntries,  using the defaults as necessary
        for (x = 0; x < NumEntries; x++) {

            // Build the Key that we want to find
            KeyData = Key + QString::number(x);

            if (x < DefaultList.count()) {
                // Get the Value; Use the default if the data is not found
                Value = RetrieveData(Section, KeyData, DefaultList[x]);
            } else {
                // Get the Value with no default
                Value = RetrieveData(Section, KeyData);
            }

            // Add the value read to the return list
            ReturnList += Value;
        }
    }
    return ReturnList;
}

void PersistantData::DebugWalkAllPersistantData()
{
    DebugWalkGeneralPersistantData();
    DebugWalkEnvGroupNamePersistantData();
    DebugWalkTabPagePersistantData();
}

void PersistantData::DebugWalkGeneralPersistantData()
{
    int        x;
    int        count;
    QString    strVer;
    QString    strRootPath;
    QString    strHeaderName;
    QString    strHeaderType;
    QString    strIgnoredEntryName;
    QString    strEntryVarsThatEnableDirOption;

    bool       bDirtyFlag;
    QSettings* pINI;

    // The Pointer to the INI File
    pINI = m_pCurrentINIFile;

    // The Generic Strings
    strVer = m_SSTPersistanceFileVersion;
    strRootPath = m_SSTRootPath;

    // The Dirty Flags
    bDirtyFlag = m_GeneralInfoDirty;
    bDirtyFlag = m_GroupEnvInfoDirty;
    bDirtyFlag = m_TabPageEntryInfoDirty;

    // The Config File Header Name List
    count = m_ConfigFileHeaderNameList.count();
    for (x = 0; x < count; x++) {
        strHeaderName = m_ConfigFileHeaderNameList[x];
        strHeaderType = m_ConfigFileHeaderTypeList[x];
    }

    count = m_IgnoredEntryNameList.count();
    for (x = 0; x < count; x++) {
        strIgnoredEntryName = m_IgnoredEntryNameList[x];
    }

    count = m_EntryVarsThatEnableDirOptionList.count();
    for (x = 0; x < count; x++) {
        strEntryVarsThatEnableDirOption = m_EntryVarsThatEnableDirOptionList[x];
    }
}

void PersistantData::DebugWalkEnvGroupNamePersistantData()
{
    int              x;
    int              y;
    int              countNames;
    int              countData;
    int              countEnvNames;
    int              countEnvValues;
    QString          GroupName;
    QString          GroupEnabled;
    QString          EnvName;
    QString          EnvValue;
    QStringList      EnvNames;
    QStringList      EnvValues;
    bool             bDirtyFlag;
    EnvGroupData*    pEnvGroupData;

    // The Env Group Names List
    countNames = m_EnvGroupNamesList.count();
    countData = m_EnvGroupDataList.count();
    // NOTE: countNames MUST EQUAL countData
//    Q_ASSERT(countNames == countData);

    for (x = 0; x < countData; x++) {
        GroupName = m_EnvGroupNamesList[x];
        pEnvGroupData = m_EnvGroupDataList[x];
        bDirtyFlag = pEnvGroupData->getDirtyFlag();
        GroupEnabled = pEnvGroupData->getGroupEnabled();

        EnvNames = pEnvGroupData->getEnvVarNamesList();
        EnvValues = pEnvGroupData->getEnvVarValuesList();

        countEnvNames = EnvNames.count();
        countEnvValues = EnvValues.count();
        // NOTE: countEnvNames MUST EQUAL countEnvValues
//        Q_ASSERT(countEnvNames == countEnvValues);

        for (y = 0; y < countEnvNames; y++) {
            EnvName = EnvNames[y];
            EnvValue = EnvValues[y];
        }
    }
}

void PersistantData::DebugWalkTabPagePersistantData()
{
    int              x;
    int              y;
    int              countNames;
    int              countData;

    int              countTabPageEntryNames;
    int              countTabPageEntryValues;
    int              countTabPageEntryEnables;
    QString          TabName;

    QString          TabPageEntryName;
    QString          TabPageEntryValue;
    QString          TabPageEntryEnable;

    QStringList      TabPageEntryNames;
    QStringList      TabPageEntryValues;
    QStringList      TabPageEntryEnables;
    bool             bDirtyFlag;
    TabPageEntryData* pTabPageEntryData;

/////////////////////////////////////

    // The Tab Names List
    countNames = m_TabPageNamesList.count();
    countData = m_TabPageEntryDataList.count();
    // NOTE: countNames MUST EQUAL countData
    Q_ASSERT(countNames == countData);

    for (x = 0; x < countData; x++) {
        TabName = m_TabPageNamesList[x];
        pTabPageEntryData = m_TabPageEntryDataList[x];
        bDirtyFlag = pTabPageEntryData->getDirtyFlag();

        TabPageEntryNames = *pTabPageEntryData->getTabPageEntryNamesList();
        TabPageEntryValues = *pTabPageEntryData->getTabPageEntryValuesList();
        TabPageEntryEnables = *pTabPageEntryData->getTabPageEntryEnablesList();

        countTabPageEntryNames = TabPageEntryNames.count();
        countTabPageEntryValues = TabPageEntryValues.count();
        countTabPageEntryEnables = TabPageEntryEnables.count();

        // NOTE: countTabPageEntryNames MUST EQUAL countTabPageEntryValues MUST EQUAL countTabPageEntryEnables
        Q_ASSERT(countTabPageEntryNames == countTabPageEntryValues);
        Q_ASSERT(countTabPageEntryNames == countTabPageEntryEnables);

        for (y = 0; y < countTabPageEntryNames; y++)  {
            TabPageEntryName = TabPageEntryNames[y];
            TabPageEntryValue = TabPageEntryValues[y];
            TabPageEntryEnable = TabPageEntryEnables[y];
        }
    }
}



/////////////////////////

