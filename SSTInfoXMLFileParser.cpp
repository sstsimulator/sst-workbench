////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////

#include "SSTInfoXMLFileParser.h"

////////////////////////////////////////////////////////

XMLSSTInfoDataHandler::XMLSSTInfoDataHandler(QString InputFilePath, SSTInfoData* ptrSSTInfoData)
    : QXmlDefaultHandler()
{
    m_InputFilePath = InputFilePath;
    m_SSTInfoData = ptrSSTInfoData;
    m_XMLFileValid = false;
    m_XMLFileCorrectVersion = false;
    m_CurrentlyProcessing = NONE;

    m_CurrentElement = NULL;
    m_CurrentComponent = NULL;
    m_CurrentIntrospector = NULL;
    m_CurrentEvent = NULL;
    m_CurrentModule = NULL;
    m_CurrentPartitioner = NULL;
    m_CurrentGenerator = NULL;
    m_CurrentParam = NULL;
    m_CurrentPort = NULL;
}

XMLSSTInfoDataHandler::~XMLSSTInfoDataHandler()
{
}

bool XMLSSTInfoDataHandler::startDocument()
{
    // Called when we are starting to process the document
    m_CurrentElement = NULL;
    m_CurrentComponent = NULL;
    m_CurrentIntrospector = NULL;
    m_CurrentEvent = NULL;
    m_CurrentModule = NULL;
    m_CurrentPartitioner = NULL;
    m_CurrentGenerator = NULL;
    m_CurrentParam = NULL;
    m_CurrentPort = NULL;

    m_XMLFileValid = false;
    m_XMLFileCorrectVersion = false;
    return true;
}

bool XMLSSTInfoDataHandler::endDocument()
{
    // Called after processing the document
    // Did we fail to open the file and is it valid
    if ((m_XMLFileValid == false) || (m_XMLFileCorrectVersion == false)) {
        return false;
    }

    return true;
}

bool XMLSSTInfoDataHandler::characters(const QString& ch)
{
    Q_UNUSED(ch)

    // Do Nothing
    return true;
}

bool XMLSSTInfoDataHandler::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts )
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(qName)

    bool    ElementHandled = false;
    QString ErrorMsg;
    QString AttributeSSTInfoVersion;
    QString AttributeFileFormat;
    QString AttributeIndex;
    QString AttributeName;
    QString AttributeDesc;
    QString AttributeCategory;
    QString AttributeDefault;
    QString AttributeEvent;
    SSTInfoDataComponent::ComponentType ComponentType;

    // THIS IS CALLED WHEN THE XML PARSER STARTS PROCESSING AN ELEMENT

    // Check the Element Name for signature that it is an SSTInfoXML file,
    // and that it the first element encountred
    if (localName == "SSTInfoXML") {
        ElementHandled = true;
        m_XMLFileValid = true;
    }

    // Now Check that we have a valid File before checking the version,
    // and that the version is checked before the sub-elements are processed
    if (m_XMLFileValid == true) {
        if (localName == "FileInfo") {
            ElementHandled = true;
            // Get the FileFormat
            AttributeSSTInfoVersion = atts.value("SSTInfoVersion");
            AttributeFileFormat = atts.value("FileFormat");
            if (AttributeFileFormat == SSTINFOXMLFILEFORMATVERSION) {
                m_XMLFileCorrectVersion = true;
            } else {
                ErrorMsg = QString("ERROR: SSTInfo XML File: File Format is Version %1; SSTWorkbench expected Version %2").arg(AttributeFileFormat).arg(SSTINFOXMLFILEFORMATVERSION);
                QMessageBox::critical(NULL, "Parsing Error", ErrorMsg);
            }
            m_SSTInfoData->SetSSTInfoVersion(AttributeSSTInfoVersion);
        }

        // Check to see that the version is correct before processing sub-elements
        if (m_XMLFileCorrectVersion == true) {

            if (localName == "Element") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if (AttributeName != "") {
                    // Build the New Element
                    m_CurrentElement = new SSTInfoDataElement(AttributeName);
                    m_CurrentElement->SetElementDesc(AttributeDesc);
                    m_CurrentlyProcessing = ELEMENT;
                } else {
                    ErrorMsg = QString("ERROR: SSTInfo XML File: Element Index %1; Element Name is undefined - Cannot add this Element to the SST Workbench Element/Component Toolbox").arg(AttributeIndex);
                    QMessageBox::warning(NULL, "Parsing Error", ErrorMsg);
                    return false;
                }

            } else if (localName == "Component") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                AttributeCategory = atts.value("Category");
                ComponentType = ConvertStringToComponentType(AttributeCategory);
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Component
                    m_CurrentComponent = new SSTInfoDataComponent(m_CurrentElement->GetElementName(), AttributeName, ComponentType);
                    m_CurrentComponent->SetComponentDesc(AttributeDesc);
                    m_CurrentlyProcessing = COMPONENT;
                } else {
                    return GenerateParsingError("Component", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Introspector") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Introspector
                    m_CurrentIntrospector = new SSTInfoDataIntrospector(m_CurrentElement->GetElementName(), AttributeName);
                    m_CurrentIntrospector->SetIntrospectorDesc(AttributeDesc);
                    m_CurrentlyProcessing = INTROSPECTOR;
                } else {
                    return GenerateParsingError("Introspector", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Event") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Event
                    m_CurrentEvent = new SSTInfoDataEvent(m_CurrentElement->GetElementName(), AttributeName);
                    m_CurrentEvent->SetEventDesc(AttributeDesc);
                    m_CurrentlyProcessing = EVENT;
                } else {
                    return GenerateParsingError("Event", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Module") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Module
                    m_CurrentModule = new SSTInfoDataModule(m_CurrentElement->GetElementName(), AttributeName);
                    m_CurrentModule->SetModuleDesc(AttributeDesc);
                    m_CurrentlyProcessing = MODULE;
                } else {
                    return GenerateParsingError("Module", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Partitioner") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Partitioner
                    m_CurrentPartitioner = new SSTInfoDataPartitioner(m_CurrentElement->GetElementName(), AttributeName);
                    m_CurrentPartitioner->SetPartitionerDesc(AttributeDesc);
                    m_CurrentlyProcessing = PARTITIONER;
                } else {
                    return GenerateParsingError("Partitioner", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Generator") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Generator
                    m_CurrentGenerator = new SSTInfoDataGenerator(m_CurrentElement->GetElementName(), AttributeName);
                    m_CurrentGenerator->SetGeneratorDesc(AttributeDesc);
                    m_CurrentlyProcessing = GENERATOR;
                } else {
                    return GenerateParsingError("Generator", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Parameter") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                AttributeDefault = atts.value("Default");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Param
                    m_CurrentParam = new SSTInfoDataParam(AttributeName, AttributeDefault);
                    m_CurrentParam->SetParamDesc(AttributeDesc);
                } else {
                    return GenerateParsingError("Param", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "Port") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeName = atts.value("Name");
                AttributeDesc = atts.value("Description");
                if ((m_CurrentElement != NULL) && (AttributeName != "")) {
                    // Build the New Port
                    m_CurrentPort = new SSTInfoDataPort(AttributeName);
                    m_CurrentPort->SetPortDesc(AttributeDesc);
                } else {
                    return GenerateParsingError("Port", AttributeName, AttributeIndex, m_CurrentElement);
                }

            } else if (localName == "PortValidEvent") {
                ElementHandled = true;
                // Get Name and Desc
                AttributeIndex = atts.value("Index");
                AttributeEvent = atts.value("Event");
                if ((m_CurrentPort != NULL) && (AttributeEvent != "")) {
                    // Build the New Valid Event
                    m_CurrentPort->AddPortValidEvent(AttributeEvent);
                } else {
                    return GenerateParsingError("PortValidEvent", AttributeEvent, AttributeIndex, m_CurrentElement);
                }

            }
        }
    }

    if (ElementHandled == false) {
        ErrorMsg = QString("ERROR: SSTInfo XML File: Element <%1> is not a valid element of the SSTInfo XML file format").arg(localName);
        QMessageBox::critical(NULL, "Parsing Error", ErrorMsg);
        return false;
    }

    return true;
};

bool XMLSSTInfoDataHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(qName)

    QString ErrorMsg;

    // Check to see if the XML is valid and correct version
    if ((m_XMLFileValid == true) && (m_XMLFileCorrectVersion == true)) {

        // Add the element item to its parent
        if (localName == "Element") {
            // Add the Library (Element) to the SSTInfoData Structure
            if ((m_SSTInfoData != NULL) && (m_CurrentElement != NULL)) {
                m_SSTInfoData->AddElement(m_CurrentElement);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentElement = NULL;

        } else if (localName == "Component") {
            if ((m_CurrentElement != NULL) && (m_CurrentComponent != NULL)) {
                // Add the Component to the Element
                m_CurrentElement->AddComponent(m_CurrentComponent);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentComponent = NULL;

        } else if (localName == "Introspector") {
            if ((m_CurrentElement != NULL) && (m_CurrentIntrospector != NULL)) {
                // Add the Introspector to the Element
                m_CurrentElement->AddIntrospector(m_CurrentIntrospector);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentIntrospector = NULL;

        } else if (localName == "Event") {
            if ((m_CurrentElement != NULL) && (m_CurrentEvent != NULL)) {
                // Add the Event to the Element
                m_CurrentElement->AddEvent(m_CurrentEvent);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentEvent = NULL;

        } else if (localName == "Module") {
            if ((m_CurrentElement != NULL) && (m_CurrentModule != NULL)) {
                // Add the Module to the Element
                m_CurrentElement->AddModule(m_CurrentModule);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentModule = NULL;

        } else if (localName == "Partitioner") {
            if ((m_CurrentElement != NULL) && (m_CurrentPartitioner != NULL)) {
                // Add the Partitioner to the Element
                m_CurrentElement->AddPartitioner(m_CurrentPartitioner);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentPartitioner = NULL;

        } else if (localName == "Generator") {
            if ((m_CurrentElement != NULL) && (m_CurrentGenerator != NULL)) {
                // Build the New Element
                m_CurrentElement->AddGenerator(m_CurrentGenerator);
                m_CurrentlyProcessing = NONE;
            }
            m_CurrentGenerator = NULL;

        } else if (localName == "Parameter") {
            if (m_CurrentParam != NULL) {
                switch (m_CurrentlyProcessing) {
                    case COMPONENT :
                        if (m_CurrentComponent != NULL) {
                            m_CurrentComponent->AddParam(m_CurrentParam);
                        }
                        break;
                    case INTROSPECTOR :
                        if (m_CurrentIntrospector != NULL) {
                            m_CurrentIntrospector->AddParam(m_CurrentParam);
                        }
                        break;
                    case MODULE :
                        if (m_CurrentModule != NULL) {
                            m_CurrentModule->AddParam(m_CurrentParam);
                        }
                        break;

                    case EVENT :
                    case PARTITIONER :
                    case GENERATOR :
                    case ELEMENT :
                    case NONE :
                    default :
                        ErrorMsg = QString("ERROR: SSTInfo XML File: <Param> cannot be Sub-element of <Event/Partitioner/Generator/Element> Elements - Illegal SSTInfo XML formating.");
                        QMessageBox::critical(NULL, "Parsing Error", ErrorMsg);
                        return false;
                }
            }
            m_CurrentParam = NULL;

        } else if (localName == "Port") {
            if (m_CurrentPort != NULL) {
                switch (m_CurrentlyProcessing) {
                    case COMPONENT :
                        if (m_CurrentComponent != NULL) {
                            m_CurrentComponent->AddPort(m_CurrentPort);
                        }
                        break;
                    case INTROSPECTOR :
                    case MODULE :
                    case EVENT :
                    case PARTITIONER :
                    case GENERATOR :
                    case ELEMENT :
                    case NONE :
                    default :
                        ErrorMsg = QString("ERROR: SSTInfo XML File: <Port> cannot be Sub-element of <Introspector/Module/Event/Partitioner/Generator/Element> Elements - Illegal SSTInfo XML formating.");
                        QMessageBox::critical(NULL, "Parsing Error", ErrorMsg);
                        return false;
                }
            }
            m_CurrentPort = NULL;

        } else if (localName == "PortValidEvent") {

        }
    }

    return true;
}

bool XMLSSTInfoDataHandler::fatalError(const QXmlParseException& exception)
{
    QString ErrorMsg;

    // Display the Error Information
    ErrorMsg = QString("ERROR: Parsing Fatal error in file %1, on line %2, column %3 - %4")
               .arg(m_InputFilePath)
               .arg(exception.lineNumber())
               .arg(exception.columnNumber())
               .arg(exception.message());

    QMessageBox::information(NULL, "Fatal Error", ErrorMsg);

    // Stop Processing the XML
    return false;
}

SSTInfoDataComponent::ComponentType XMLSSTInfoDataHandler::ConvertStringToComponentType(QString ComponentTypeString)
{
    if (ComponentTypeString == "PROCESSOR COMPONENT") {
        return SSTInfoDataComponent::COMP_PROCESSOR;
    } else if (ComponentTypeString == "MEMORY COMPONENT") {
        return SSTInfoDataComponent::COMP_MEMORY;
    } else if (ComponentTypeString == "NETWORK COMPONENT") {
        return SSTInfoDataComponent::COMP_NETWORK;
    } else if (ComponentTypeString == "SYSTEM COMPONENT") {
        return SSTInfoDataComponent::COMP_SYSTEM;
    } else {
        return SSTInfoDataComponent::COMP_UNCATEGORIZED;
    }
}

bool XMLSSTInfoDataHandler::GenerateParsingError(QString ObjectTitle, QString ObjectName, QString Index, SSTInfoDataElement* CurrentElement)
{
    QString ErrorMsg;

    if (ObjectName == "") {
        ErrorMsg = QString("ERROR: SSTInfo XML File: %1 Index %2; %1 Name is undefined - Cannot add this %1 to the SST Workbench Element/Component Toolbox").arg(ObjectTitle).arg(Index);
        QMessageBox::warning(NULL, "Parsing Eror", ErrorMsg);
        return false;
    }

    if (CurrentElement == NULL) {
        ErrorMsg = QString("ERROR: SSTInfo XML File: %1 Index %2; is Defined before the Parent Element - Cannot add this %1 to the SST Workbench Element/Component Toolbox").arg(ObjectTitle).arg(Index);
        QMessageBox::critical(NULL, "Parsing Error", ErrorMsg);
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////

SSTInfoXMLFileParser::SSTInfoXMLFileParser() :
    QXmlSimpleReader()
{
    // Init member variables
    m_InputFilePath.clear();
    m_XMLFile = NULL;
    m_XMLInputSource = NULL;
    m_XMLSSTInfoDataHandler = NULL;
    m_SSTInfoData = NULL;
}

SSTInfoXMLFileParser::~SSTInfoXMLFileParser()
{
    // Cleanup any dynamic objects
    Cleanup();
}

void SSTInfoXMLFileParser::Cleanup()
{
    // Delete any Dynamic Objects created
    if (m_XMLFile != NULL) {
        delete m_XMLFile;
    }

    if (m_XMLInputSource != NULL) {
        delete m_XMLInputSource;
    }

    if (m_XMLSSTInfoDataHandler != NULL) {
        delete m_XMLSSTInfoDataHandler;
    }

    // NOTE: m_SSTInfoData is shared with and will be cleaned up by WindowComponentToolbox.cpp
}

void SSTInfoXMLFileParser::SetXMLInputFile(QString InputFilePath)
{
    // Cleanup any dynamic objects
    Cleanup();

    // Create the XML Input Source
    m_InputFilePath = InputFilePath;
    m_XMLFile = new QFile(m_InputFilePath);
    m_XMLInputSource = new QXmlInputSource(m_XMLFile);
}

bool SSTInfoXMLFileParser::ParseXMLFile()
{
    bool bRtn;

    // Check that we have an Input Source
    if (m_XMLInputSource == NULL) {
        return false;
    }

    // Create a new SSTInfoData object
    m_SSTInfoData = new SSTInfoData();

    // Create the Handler (Class to decode xml file and populate the m_SSTInfoData variable)
    m_XMLSSTInfoDataHandler = new XMLSSTInfoDataHandler(m_InputFilePath, m_SSTInfoData);

    // Set the Handler for Data and Errors
    setContentHandler(m_XMLSSTInfoDataHandler);
    setErrorHandler(m_XMLSSTInfoDataHandler);

    // Parse the data (this will send the data to the Handler object)
    bRtn = parse(m_XMLInputSource);

    // Check if parse failed
    if (bRtn == false ) {
        delete m_SSTInfoData;
        m_SSTInfoData = NULL;
    }

    return bRtn;
}
