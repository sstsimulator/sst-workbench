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

#ifndef CONFIGFILEPARSER_H
#define CONFIGFILEPARSER_H

#include <QMessageBox>
#include <QTextStream>
#include <QDir>

#include "persistantdata.h"
#include "headerdata.h"

class ConfigFileParser : public QObject
{
    Q_OBJECT
public:
    explicit ConfigFileParser(QObject *parent = 0);
    
    void        setConfigFilePath(QString& filePath);
    int         getNumberOfHeaders();
    HeaderData* getHeader(int index);
    void        parseConfigFile();

signals:
    
public slots:
    
private:
    void    initializeHeaderNamesAndRules();
    void    matchAndPopulateHeaderAndItsEntries(QString& HeaderName, QString& HeaderType, QFile& File);
    void    populateHeaderBlock(QTextStream& Stream, HeaderData* pHeaderData, int HeaderLine);
    QString getEntryName(QString& testLine, int startIndex, int* endIndexRtn);
    QString getVariableName(QString& testLine, int startIndex, int* endIndexRtn);
    QString getInfoTextAndFindNextLineNormal(QTextStream& Stream, QString& testLine, int startIndex);
    QString getInfoTextAndFindNextLineSpecial(QTextStream& Stream, QString& testLine, int startIndex);

    QString             m_sstConfigFilePath;
    QStringList         m_HeaderNameList;
    QStringList         m_HeaderTypeList;
    QStringList         m_ParsingErrorList;
    QList<HeaderData*>  m_HeaderDataList;
    QString             m_nextLine;
    int                 m_currentFilelinecount;
    int                 m_nextFilelinecount;
    PersistantData      m_PersistantData;
};

#endif // CONFIGFILEPARSER_H
