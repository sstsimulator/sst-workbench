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

#ifndef PERSISTANTDATA_H
#define PERSISTANTDATA_H

#include <QDir>
#include <QSettings>

///////////////////////////////////////////////////////////////////////////////////////
// Support Classes
class EnvGroupData
{
public:
    explicit EnvGroupData(QString GroupName)
    {
        m_GroupName = GroupName;
        m_GroupEnabled = "0";
        m_Dirty = true;
    }

    QString     getGroupName() {return m_GroupName;}
    QString     getGroupEnabled() {return m_GroupEnabled;}
    QStringList getEnvVarNamesList() {return m_EnvVarNamesList;}
    QStringList getEnvVarValuesList() {return m_EnvVarValuesList;}
    bool        getDirtyFlag() {return m_Dirty;}

    void setGroupName(QString Name) {m_GroupName = Name; m_Dirty = true;}
    void setGroupEnabled(QString flag) {m_GroupEnabled = flag; m_Dirty = true;}
    void setEnvVarNamesList(QStringList list) {m_EnvVarNamesList = list; m_Dirty = true;}
    void setEnvVarValuesList(QStringList list) {m_EnvVarValuesList = list; m_Dirty = true;}
    void setDirtyFlag(bool flag) {m_Dirty = flag;}

private:
    QString     m_GroupName;
    QString     m_GroupEnabled;
    QStringList m_EnvVarNamesList;
    QStringList m_EnvVarValuesList;
    bool        m_Dirty;
};

class TabPageEntryData
{
public:
    explicit TabPageEntryData(QString TabPageName)
    {
       m_TabPageName = TabPageName;
       m_Dirty = true;
    }

    QString      getTagPageName() {return m_TabPageName;}
    QStringList* getTabPageEntryNamesList() {return &m_TabPageEntryNames;}
    QStringList* getTabPageEntryValuesList() {return &m_TabPageEntryValues;}
    QStringList* getTabPageEntryEnablesList() {return &m_TabPageEntryEnables;}
    bool         getDirtyFlag() {return m_Dirty;}

    void setTabPageEntryNamesList(QStringList list) {m_TabPageEntryNames = list; m_Dirty = true;}
    void setTabPageEntryValuesList(QStringList list) {m_TabPageEntryValues = list; m_Dirty = true;}
    void setTabPageEntryEnablesList(QStringList list) {m_TabPageEntryEnables = list; m_Dirty = true;}

    void addTabPageEntryName(QString Data) {m_TabPageEntryNames += Data; m_Dirty = true;}
    void addTabPageEntryValue(QString Data) {m_TabPageEntryValues += Data; m_Dirty = true;}
    void addTabPageEntryEnable(QString Data) {m_TabPageEntryEnables += Data; m_Dirty = true;}

    void setTabPageEntryName(int Index, QString Data) {m_TabPageEntryNames[Index] = Data; m_Dirty = true;}
    void setTabPageEntryValue(int Index, QString Data) {m_TabPageEntryValues[Index] = Data; m_Dirty = true;}
    void setTabPageEntryEnable(int Index, QString Data) {m_TabPageEntryEnables[Index] = Data; m_Dirty = true;}

    void setDirtyFlag(bool flag) {m_Dirty = flag;}

private:
    QString m_TabPageName;
    QStringList m_TabPageEntryNames;
    QStringList m_TabPageEntryValues;
    QStringList m_TabPageEntryEnables;
    bool        m_Dirty;
};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class PersistantData
{
public:
    // Constructor / Destructor
    explicit PersistantData();
    ~PersistantData();

    /////////////////////////////////////////////////

    // General Saving to / Loading From Persistant Data files
    bool LoadPersistantDataFromFile(QString FilePath);
    bool SavePersistantDataToFile(QString FilePath = "");
    bool SavePersistantData();
    QString GetDefaultPersistantDataFile();

    /////////////////////////////////////////////////

    // Get Root Path to SST Directory
    QString     GetSSTRootPath();

    // Get the List of Config File headers that we look for
    QStringList GetConfigFileHeaderNameList();
    QStringList GetConfigFileHeaderTypeList();

    // Get Other Info
    QStringList GetIgnoredEntryNameList();
    QStringList GetEntryVarsThatEnableDirOptionList();

    // Get Environment Groups Data
    QStringList GetEnvGroupNamesList();
    QStringList GetEnvGroupEnablesList();

    // Get Environment Variable data
    QStringList GetEnvVarNamesList(QString GroupName);
    QStringList GetEnvVarValuesList(QString GroupName);

    // Get User Selected entries for the configuration
    QString GetEntryValue(QString TabPageName, QString EntryName);
    bool    GetEntryState(QString TabPageName, QString EntryName);

    /////////////////////////////////////////////////

    // Set Root Path to SST Directory
    void SetSSTRootPath(QString Path);

    // Set the List of Config File headers that we look for
    void SetConfigFileHeaderNameList(QStringList List);
    void SetConfigFileHeaderTypeList(QStringList List);

    // Set Other Info
    void SetIgnoredEntryNameList(QStringList List);
    void SetEntryVarsThatEnableDirOptionList(QStringList List);

    // Set Environment Groups Data
    void SetEnvGroupNameAndEnable(QString GroupName, QString GroupEnable);

    // Set Environment Variable Data
    void SetEnvVarNamesList(QString GroupName, QStringList List);
    void SetEnvVarValuesList(QString GroupName, QStringList List);

    // Set User Selected entries for the configuration
    void SetEntryValue(QString TabPageName, QString EntryName, QString Value);
    void SetEntryState(QString TabPageName, QString EntryName, bool Enabled);

    /////////////////////////////////////////////////

    // Public Support Routines
    void RemoveEnvGroup(QString GroupName);
    void RenameEnvGroup(QString OldGroupName, QString NewGroupName);
    void CopyEnvGroup(QString ExistingGroupName, QString NewGroupName, QString GroupEnable);

    /////////////////////////////////////////////////

private:
    // Build Environment Var Data
    void AddNewEnvGroupNameAndEnable(QString GroupName, QString GroupEnable);
    void FillEnvGroupNameAndEnable(QString GroupName, QString GroupEnable);
    void FillEnvVarDataNames(QString GroupName, QStringList Names);
    void FillEnvVarDataValues(QString GroupName, QStringList Values);

    // Build Entry Data
    void FillEntryDataState(QString TabPageName, QString EntryName, bool Enabled);
    void FillEntryDataValue(QString TabPageName, QString EntryName, QString Value);

    // Dirty Flag Management
    void SetAllDirtyFlags();
    void ClearAllDirtyFlags();

    // Blow everything away, this gives us a fresh and clean object to populate with data
    void ClearAllInternalPersistantData();

    // Load all the various Persistant Data
    void LoadAllPersistantData();

    // Save of the various Persistant Data
    void SaveAllPersistantData();

    // Low Level INI Processing
    void OpenINIFile(QString FilePath);
    void CloseINIFile();

    void SaveData(QString Section, QString Key, QString Value);                 // Save Single Data
    void SaveDataBlock(QString Section, QString Key, QStringList ValueList);    // Save a block of data

    QString RetrieveData(QString Section, QString Key, QString Default = "");   // Load a single data
    QStringList RetrieveDataBlock(QString Section, QString Key, QStringList DefaultList, int NumEntries = -1);  // Load a block of data (if NumEntries != -1, then load that many entries)

public:
    // DEBUG ROUTINES
    void DebugWalkAllPersistantData();
    void DebugWalkGeneralPersistantData();
    void DebugWalkEnvGroupNamePersistantData();
    void DebugWalkTabPagePersistantData();

private:
    static bool                     m_PersistantDataLoaded;

    // The INI File
    static QSettings*               m_pCurrentINIFile;

    // General Information
    static QString                  m_SSTPersistanceFileVersion;          // File Version of the Persistance Datra
    static QString                  m_SSTRootPath;                        // The saved path to the SST Root directory
    static QStringList              m_ConfigFileHeaderNameList;           // The List of Header names to search for in the Config File
    static QStringList              m_ConfigFileHeaderTypeList;           // The List of Header types assigned to the Header names

    static QStringList              m_IgnoredEntryNameList;               // The List of Entry Names that we should not populate (ignore)
    static QStringList              m_EntryVarsThatEnableDirOptionList;   // The List of Entry Variable that enable the Dir Select Button

    // Group Environment Variable Data
    static QStringList              m_EnvGroupNamesList;                  // List of Groups of Environment Variables
    static QList<EnvGroupData*>     m_EnvGroupDataList;

    // User Select Tab Page Entry Data
    static QStringList              m_TabPageNamesList;                   // List of Tab Names
    static QList<TabPageEntryData*> m_TabPageEntryDataList;

    // Dirty Flags
    static bool                     m_GeneralInfoDirty;
    static bool                     m_GroupEnvInfoDirty;
    static bool                     m_TabPageEntryInfoDirty;
};

#endif // PERSISTANTDATA_H
