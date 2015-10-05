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

#ifndef SSTINFOXMLFILEPARSER_H
#define SSTINFOXMLFILEPARSER_H

#include "GlobalIncludes.h"

class XMLSSTInfoDataHandler :public QXmlDefaultHandler
{
public:
    // Constructor / Destructor
    XMLSSTInfoDataHandler(QString InputFilePath, SSTInfoData* ptrSSTInfoData);
    ~XMLSSTInfoDataHandler();

private:
    bool fatalError(const QXmlParseException& exception);

    bool startDocument();
    bool endDocument();

    bool characters(const QString& ch);
    bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName);

    ComponentType_enum ConvertStringToComponentType(QString ComponentTypeString);
    bool GenerateParsingError(QString ObjectTitle, QString ObjectName, QString Index, SSTInfoDataElement* CurrentElement);

private:
    QString                               m_InputFilePath;
    SSTInfoData*                          m_SSTInfoData;
    bool                                  m_XMLFileValid;
    bool                                  m_XMLFileCorrectVersion;
    XMLParserCurrentlyProcessing_enum     m_CurrentlyProcessing;

    SSTInfoDataElement*                    m_CurrentElement;
    SSTInfoDataComponent*                  m_CurrentComponent;
    SSTInfoDataIntrospector*               m_CurrentIntrospector;
    SSTInfoDataEvent*                      m_CurrentEvent;
    SSTInfoDataModule*                     m_CurrentModule;
    SSTInfoDataPartitioner*                m_CurrentPartitioner;
    SSTInfoDataGenerator*                  m_CurrentGenerator;
    SSTInfoDataParam*                      m_CurrentParam;
    SSTInfoDataPort*                       m_CurrentPort;
};

////////////////////////////////////////////////////////

class SSTInfoXMLFileParser : public QXmlSimpleReader
{
public:
    // Constructor / Destructor
    SSTInfoXMLFileParser();
    ~SSTInfoXMLFileParser();

public:
    void SetXMLInputFile(QString InputFilePath);
    bool ParseXMLFile();
    SSTInfoData* GetParsedSSTInfoData() {return m_SSTInfoData;}

private:
    void Cleanup();

private:
    QString                 m_InputFilePath;
    QFile*                  m_XMLFile;
    QXmlInputSource*        m_XMLInputSource;
    XMLSSTInfoDataHandler*  m_XMLSSTInfoDataHandler;
    SSTInfoData*            m_SSTInfoData;
};

#endif // SSTINFOXMLFILEPARSER_H
