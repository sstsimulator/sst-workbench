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

#include "PythonExporter.h"
#include "DialogExportDisplayStatus.h"

PythonExporter::PythonExporter(WiringScene* Scene, QString FilePath)
{
    // Init Member Variables
    m_Scene = Scene;
    m_FilePath = FilePath;
    m_StartupConfigComponent = NULL;
    m_bFinalResult = true;
    m_ExportErrorsList.clear();
    m_ExportWarningsList.clear();
}

PythonExporter::~PythonExporter()
{
}

bool PythonExporter::PerformExportOfPythonFile()
{
    // Check for SST Startup Configuration Component Exists
    m_bFinalResult &= CheckSSTStartupConfigComponent();

    // Check the Component Parameters
    m_bFinalResult &= CheckComponentParameters();

    // Check the Component Ports
    m_bFinalResult &= CheckComponentUnconnectedPorts();

    // Check for Unconnected Wires
    m_bFinalResult &= CheckUnconnectedWires();

    // Now if everything is ok, Create the Python File
    if (m_bFinalResult == true) {
        m_bFinalResult &= CreatePythonFile();
    }

    // Display the status of the Export effort
    DisplayExportStatus();

    return m_bFinalResult;
}

bool PythonExporter::CheckSSTStartupConfigComponent()
{
    GraphicItemComponent* ptrComponent;

    // Search all the items on the Wiring Scene
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemComponent::Type) {
            // Get the item as a Graphic Ccomponent item
            ptrComponent = (GraphicItemComponent*)item;

            // Check to see if this object is a SSTStartupConfigurationObject
            if (ptrComponent->GetComponentType() == SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION) {
                m_StartupConfigComponent = ptrComponent;
                return true;
            }
        }
    }

    // We did not find the Startup Config Component
//    m_ExportErrorsList.append("ERROR: SST Startup Configuration Component not found.");
//    return false;

    // Changed to Warning that the configuration is not found, and no error
    m_ExportWarningsList.append("Warning: SST Startup Configuration Component not found.");
    return true;
}

bool PythonExporter::CheckComponentParameters()
{
    GraphicItemComponent* ptrComponent;
    int                   x;
    int                   NumProperties;
    ItemProperty*         Property;
    QString               PropName;
    QString               PropValue;
    QString               CompName;
    bool                  bRtn = true;

    // Search all the items on the Wiring Scene
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemComponent::Type) {
            // Get the item as a Graphic Ccomponent item
            ptrComponent = (GraphicItemComponent*)item;
            CompName = ptrComponent->GetItemProperties()->GetPropertyValue(COMPONENT_PROPERTY_USERNAME);

            // Get the number of properties for Component
            NumProperties = ptrComponent->GetItemProperties()->GetNumProperties();
            for (x = 0; x < NumProperties; x++) {
                // Get the property data
                Property  = ptrComponent->GetItemProperties()->GetProperty(x);
                PropName  = Property->GetName();
                PropValue = Property->GetValue();

                // Detirmine if the Value is Required
                if (PropValue == "REQUIRED") {
                    bRtn = false;
                    m_ExportErrorsList.append(QString("ERROR: Component %1 - Parameter %2 is REQUIRED.").arg(CompName).arg(PropName));
                }
            }
        }
    }
    return bRtn;
}

bool PythonExporter::CheckComponentUnconnectedPorts()
{
    GraphicItemComponent* ptrComponent;
    GraphicItemPort*      ptrPort;
    int                   x;
    int                   NumGraphicPorts;
    QString               CompName;
    QString               PortName;
    bool                  bRtn = true;

    // Search all the items on the Wiring Scene
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemComponent::Type) {
            // Get the item as a Graphic Ccomponent item
            ptrComponent = (GraphicItemComponent*)item;
            CompName = ptrComponent->GetItemProperties()->GetPropertyValue(COMPONENT_PROPERTY_USERNAME);

            NumGraphicPorts = ptrComponent->GetNumGraphicalPortsOnComponent();
            for (x = 0; x < NumGraphicPorts; x++) {
                ptrPort = ptrComponent->GetGraphicalPortArray().at(x);
                PortName = ptrPort->GetConfiguredPortName();

                // Is the Port Configured (Static Ports are always configured)
                // Also, Unconfigured ports cannot have a wire connected
                if (ptrPort->IsPortConfigured() == false) {
                    m_ExportWarningsList.append(QString("Warning: Component %1 - Port %2 is Dynamic and NOT Configured.").arg(CompName).arg(PortName));
                } else if (ptrPort->IsPortConnectedToWire() == false) {
                    // Removed this warning as it is annoying since lots of ports will not be connected
//                  m_ExportWarningsList.append(QString("Warning: Component %1 - Port %2 is Not Connected to a Wire.").arg(CompName).arg(PortName));
                }
            }
        }
    }
    return bRtn;
}

bool PythonExporter::CheckUnconnectedWires()
{
    GraphicItemWire*         ptrParentWire;
    QList<GraphicItemWire*>  ProcessedWiresList;
    QString                  WireIndex;
    bool                     bRtn = true;

    // We need to get the lists of objects from the selections
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemWireLineSegment::Type) {
            // Get the items Parent (Graphic Wire item)
            ptrParentWire = (GraphicItemWire*)item->parentItem();

            // Check to see if we have already processed this parent wire
            if (ProcessedWiresList.contains(ptrParentWire) == false) {
                // This parent wire has not been processed it, so add it to the ProcessedWiresList
                ProcessedWiresList.append(ptrParentWire);

                // Get the Wire's Properties
                WireIndex = ptrParentWire->GetItemProperties()->GetPropertyValue(WIRE_PROPERTY_NUMBER);

                // Check the wire for its status
                if (ptrParentWire->GetWireConnectedState() == GraphicItemWire::NO_CONNECTIONS) {
                    bRtn = false;
                    m_ExportErrorsList.append(QString("ERROR: Wire %1 - Has No Connections.").arg(WireIndex));
                }
                if ((ptrParentWire->GetWireConnectedState() == GraphicItemWire::STARTPOINT_CONNECTED) ||
                    (ptrParentWire->GetWireConnectedState() == GraphicItemWire::ENDPOINT_CONNECTED)) {
                    bRtn = false;
                    m_ExportErrorsList.append(QString("ERROR: Wire %1 - Has only one connection to a Component Port.").arg(WireIndex));
                }
            }
        }
    }

    return bRtn;
}

void PythonExporter::DisplayExportStatus()
{
    QStringList FinalList;
    QString     FinalResults;
    int         NumErrorsFound = m_ExportErrorsList.count();
    int         NumWarningsFound = m_ExportWarningsList.count();

    FinalList = m_ExportErrorsList + m_ExportWarningsList;

    if (NumErrorsFound == 0) {
        FinalResults = QString("SUCCESS with %1 Errors; %2 Warnings").arg(NumErrorsFound).arg(NumWarningsFound);
    } else {
        FinalResults = QString("FAILED TO EXPOPRT with %1 Errors; %2 Warnings").arg(NumErrorsFound).arg(NumWarningsFound);
    }

    // Run the Dialog to display the status
    DialogExportDisplayStatus dlg(FinalResults, FinalList);
    dlg.exec();
}

bool PythonExporter::CreatePythonFile()
{
    // Create the File
    QFile PythonFile(m_FilePath);

    // Attempt to open the file
    if (PythonFile.open(QFile::WriteOnly | QFile::Truncate)) {

        // Create a Text Stream to write our output to
        QTextStream output(&PythonFile);

        // Write the Header
        WriteHeader(output);

        // Write the Program Options
        WriteSSTStartupProgramOptions(output);

        // Write the Components
        WriteComponents(output);

        // Write the Links
        WriteLinks(output);

        // Write the Footer
        WriteFooter(output);

        // Close the File
        PythonFile.close();

    } else {
        QString ErrorMsg = QString("Error: Unable to create SST Python file %1.").arg(m_FilePath);
        QMessageBox::critical(NULL, "Unable to create file", ErrorMsg);
        return false;
    }

    return true;
}

void PythonExporter::WriteHeader(QTextStream& out)
{
    QDateTime DateTime = QDateTime::currentDateTime();
    QString   DateTimeStr = DateTime.toString("yyyy-MM-dd HH:mm:ss");

    // Header
    SEPERATOR;
    out << QString("# SST Workbench (V%1) - SST Python Data generated on %2").arg(COREAPP_VERSION).arg(DateTimeStr) << endl;
    SEPERATOR;
    BLANKLINE;
    HEADER;
    out << "# Import the SST Components" << endl;
    out << "import sst" << endl;
}

void PythonExporter::WriteSSTStartupProgramOptions(QTextStream& out)
{
    int           x;
    int           NumProperties;
    ItemProperty* Property;
    QString       PropName;
    QString       PropValue;
    bool          PropExportable;

    // Comment for the export file
    BLANKLINE;
    HEADER;
    BLANKLINE;
    out << "# Set the SST Startup Configuration" << endl;

    // Check to se that we have a startup config component
    if (m_StartupConfigComponent == NULL)  {
        out << "# SST Startup Configuration Component in SST Workbench was not found." << endl;
        return;
    }

    // Get the number of properties for the SST Startup Configuration Component
    NumProperties = m_StartupConfigComponent->GetItemProperties()->GetNumProperties();

    for (x = 0; x < NumProperties; x++) {
        // Get the property data
        Property  = m_StartupConfigComponent->GetItemProperties()->GetProperty(x);
        PropName  = Property->GetName();
        PropValue = Property->GetValue();
        PropExportable = Property->GetExportable();

        // Make sure we are only writing the Exportable Properties (not the Generic Info)
        if ((PropExportable == true) && (PropValue.isEmpty() == false)) {
            // Write it out to the export file
            out << QString("sst.setProgramOption(\"%1\", \"%2\")").arg(PropName).arg(PropValue) << endl;
        }
    }
}

void PythonExporter::WriteComponents(QTextStream& out)
{
    GraphicItemComponent* ptrComponent;
    int                   x;
    int                   NumProperties;
    ItemProperties*       Properties;
    ItemProperty*         Property;
    QString               PropName;
    QString               PropValue;
    bool                  PropExportable;
    QString               CompUniqueName;
    QString               CompUserName;
    QString               CompElementName;
    QString               CompBaseName;
    QString               CompType;
    QString               CompComment;
    QString               Rank;
    QString               Weight;
    QString               CompVariableName;
    bool                  FirstLineDone;

    BLANKLINE;
    HEADER;
    out << "# Setup Components" << endl;

    // Search all the items on the Wiring Scene
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemComponent::Type) {
            // Get the item as a Graphic Ccomponent item
            ptrComponent = (GraphicItemComponent*)item;

            // Check to see that this object is Not a SSTStartupConfigurationObject
            if (ptrComponent->GetComponentType() != SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION) {
                // Get some specific properties
                Properties = ptrComponent->GetItemProperties();
                CompUniqueName = Properties->GetPropertyValue(COMPONENT_PROPERTY_UNIQUENAME);
                CompUserName = Properties->GetPropertyValue(COMPONENT_PROPERTY_USERNAME);
                CompElementName = Properties->GetPropertyValue(COMPONENT_PROPERTY_COMPPARENTELEM);
                CompBaseName = Properties->GetPropertyValue(COMPONENT_PROPERTY_COMPNAME);
                CompType = Properties->GetPropertyValue(COMPONENT_PROPERTY_TYPE);
                CompComment = Properties->GetPropertyValue(COMPONENT_PROPERTY_COMMENT);
                Rank = Properties->GetPropertyValue(COMPONENT_PROPERTY_RANK);
                Weight = Properties->GetPropertyValue(COMPONENT_PROPERTY_WEIGHT);

                // Create the Component's Variable Name
                CompVariableName = GetComponentVarName(CompUniqueName);

                // Write the Component Instantiation
                BLANKLINE;
                out << "# Component : " << QString("[%1] %2 (%3) - %4").arg(CompType).arg(CompUniqueName).arg(CompUserName).arg(CompComment) << endl;
                out << CompVariableName << QString(" = sst.Component(\"%1\", \"%2.%3\")").arg(CompUniqueName).arg(CompElementName).arg(CompBaseName) << endl;

                // Set Global Component Settings
                if (Rank.isEmpty() == false) {
                    out << CompVariableName << QString(".setRank(%1)").arg(Rank) << endl;
                }
                if (Weight.isEmpty() == false) {
                    out << CompVariableName << QString(".setWeight(%1)").arg(Weight) << endl;
                }

                FirstLineDone = false;
                // Set Component Parameters
                out << CompVariableName << ".addParams( {";

                // Get the number of properties for Component
                NumProperties = Properties->GetNumProperties();
                for (x = 0; x < NumProperties; x++) {
                    // Get the property data
                    Property  = Properties->GetProperty(x);
                    PropName  = Property->GetName();
                    PropValue = Property->GetValue();
                    PropExportable = Property->GetExportable();

                    // Make sure we are only writing the Exportable Properties (not the Generic Info)
                    if ((PropExportable == true) && (PropValue.isEmpty() == false)) {
                        if (FirstLineDone == true) {
                            out << "," << endl;
                        } else {
                            out << endl;
                        }

                        // Write the parameter out to the export file
                        out << TAB << QString("\"%1\" : \"%2\"").arg(PropName).arg(PropValue);
                        FirstLineDone = true;
                    }
                }

                out << endl << TAB << "} )" << endl;
                BLANKLINE;
            }
        }
    }
}

void PythonExporter::WriteLinks(QTextStream& out)
{
    GraphicItemWire*         ptrParentWire;
    QList<GraphicItemWire*>  ProcessedWiresList;
    QString                  WireIndex;
    QString                  WireComment;
    QString                  LinkVarName;
    int                      LinkIndex = 0;

    GraphicItemPort*         ptrStartConnectionPort;
    GraphicItemPort*         ptrEndConnectionPort;
    GraphicItemComponent*    ptrStartConnectionComponent;
    GraphicItemComponent*    ptrEndConnectionComponent;

    QString                  StartPortLatency;
    QString                  EndPortLatency;
    QString                  StartPortName;
    QString                  EndPortName;
    QString                  StartComponentUniqueName;
    QString                  EndComponentUniqueName;
    QString                  StartComponentVarName;
    QString                  EndComponentVarName;

    BLANKLINE;
    HEADER;
    out << "# Setup Links" << endl;

    // We need to get the lists of objects from the selections
    foreach (QGraphicsItem* item, m_Scene->items(Qt::DescendingOrder)) {
        if (item->type() == GraphicItemWireLineSegment::Type) {
            // Get the items Parent (Graphic Wire item)
            ptrParentWire = (GraphicItemWire*)item->parentItem();

            // Check to see if we have already processed this parent wire
            if (ProcessedWiresList.contains(ptrParentWire) == false) {
                // This parent wire has not been processed it, so add it to the ProcessedWiresList
                ProcessedWiresList.append(ptrParentWire);

                // Get the Wire's Properties
                WireIndex = ptrParentWire->GetItemProperties()->GetPropertyValue(WIRE_PROPERTY_NUMBER);
                WireComment = ptrParentWire->GetItemProperties()->GetPropertyValue(WIRE_PROPERTY_COMMENT);
                LinkVarName = QString("Link_%1").arg(LinkIndex++);

                // Get the Ports that the Wire Ends are tied to and asssociated data
                ptrStartConnectionPort = ptrParentWire->GetStartPointConnectedPort();
                ptrEndConnectionPort   = ptrParentWire->GetEndPointConnectedPort();
                StartPortLatency = ptrStartConnectionPort->GetItemProperties()->GetPropertyValue(PORT_PROPERTY_LATENCY);
                EndPortLatency   = ptrEndConnectionPort->GetItemProperties()->GetPropertyValue(PORT_PROPERTY_LATENCY);
                StartPortName = ptrStartConnectionPort->GetConfiguredPortName();
                EndPortName = ptrEndConnectionPort->GetConfiguredPortName();

                // Get the Components (of the ports) that the Wire Ends are tied to and asssociated data
                ptrStartConnectionComponent = (GraphicItemComponent*)ptrStartConnectionPort->parentItem();
                ptrEndConnectionComponent   = (GraphicItemComponent*)ptrEndConnectionPort->parentItem();
                StartComponentUniqueName = ptrStartConnectionComponent->GetItemProperties()->GetPropertyValue(COMPONENT_PROPERTY_UNIQUENAME);
                EndComponentUniqueName   = ptrEndConnectionComponent->GetItemProperties()->GetPropertyValue(COMPONENT_PROPERTY_UNIQUENAME);
                StartComponentVarName = GetComponentVarName(StartComponentUniqueName);
                EndComponentVarName = GetComponentVarName(EndComponentUniqueName);

                // Write the Wire Instantiation and Connection
                BLANKLINE;
                out << "# " << QString("%1 : Wire %2 - %3").arg(LinkVarName).arg(WireIndex).arg(WireComment) << endl;
                out << LinkVarName << QString(" = sst.Link(\"%1_Wire_%2\")").arg(LinkVarName).arg(WireIndex) << endl;
                out << LinkVarName << ".connect( " << QString("(%1, \"%2\", \"%3\")").arg(StartComponentVarName).arg(StartPortName).arg(StartPortLatency)
                                   << ", "         << QString("(%1, \"%2\", \"%3\")").arg(EndComponentVarName).arg(EndPortName).arg(EndPortLatency)
                                   << " )" << endl;
            }
        }
    }
}

void PythonExporter::WriteFooter(QTextStream& out)
{
    BLANKLINE;
    HEADER;
    out << "# Finished with SST Workbench Python Data Export" << endl;
    out << "print \"Done Configuring SST Model\"" << endl;
    BLANKLINE;
    SEPERATOR;
}

QString PythonExporter::GetComponentVarName(QString CompUniqueName)
{
    QString CompVariableName;

    // Create the Component's Variable Name
    CompVariableName = CompUniqueName;
    CompVariableName.replace(".", "_");
    return CompVariableName;
}


