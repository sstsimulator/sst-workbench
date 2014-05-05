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

#ifndef GLOBALDEF_H
#define GLOBALDEF_H

//////////////////////////////////////////////////////////////////////

// CORE APPLICATION SETTINGS
#define COREAPP_APPNAME                         "SSTWorkBench"
#define COREAPP_VERSION                         "1.0"                           /*** INCREMENT FOR NEW APP VERSION ***/
#define COREAPP_ORGNAME                         "Sandia_Corporation"
#define COREAPP_DOMAINNAME                      "sst.sandia.gov"
#define COREAPP_SINGLEINSTANCEKEY               "__SST_WORKBENCH_SINGLEINSTANCEKEY__"

// GENERAL DEFINES
#define NUMGRAPHICITEMTYPES                     GraphicItemBase::ITEMTYPE_END
#define NUMCOMPONENTTYPES                       SSTInfoDataComponent::COMP_END
#define SSTINFOXMLFILEFORMATVERSION             "1.0"                           /*** INCREMENT IF SSTINFO XML FORMAT CHANGES ***/
#define SSTWORKBENCHPROJECTFILEFORMATVERSION    100                             /*** INCREMENT IF WORKBENCH PROJECT FILE STRUCTURE CHANGES ***/
#define SSTWORKBENCHPROJECTFILEMAGICNUMBER      0xCD4234DF
#define PROJECTFILEEXTENSION                    ".swb"
#define PROJECTFILEEXTENSIONFILTER              "*.swb"
#define PYTHONEXPORTFILEEXTENSION               ".py"
#define PYTHONEXPORTFILEEXTENSIONFILTER         "*.py"
#define MAINSCREENTITLE                         "Project - "
#define UNTITLED                                "Untitled"
#define TOOLBOX_INITNUMBUTTONSACROSS            1
#define MOUSEMOVE_DELAYPIXELS                   5
#define DEFAULT_PASTE_OFFSET                    20

// Zoom In and Out Limits
#define ZOOM_IN_LIMIT                           500
#define ZOOM_OUT_LIMIT                          25
#define ZOOM_STEP_SIZE                          25

// PERSISTENT DATA BETWEEN RUNS
#define PERSISTFILENAME                         "//.SSTWorkbench"
#define PERSISTGROUP_GENERAL                    "General"
#define PERSISTGROUP_MAINWINDOW                 "MainWindow"
#define PERSISTGROUP_PREFERENCES                "Preferences"
#define PERSISTVALUE_SIZE                       "Size"
#define PERSISTVALUE_POSIITON                   "Position"
#define PERSISTVALUE_MAINWINSTATE               "MainWinState"
#define PERSISTVALUE_SPLITTERSTATE              "SplitterState"
#define PERSISTVALUE_SSTINFOXMLFILEPATHNAME     "SSTInfoXMLDataFilePathName"
#define PERSISTVALUE_PROJECTFILEPATHNAME        "ProjectDataFilePathName"
#define PERSISTVALUE_PYTHONEXPORTFILEPATHNAME   "PythonExportFilePathName"
#define PERSISTVALUE_PREF_RETURNTOSELAFTERWIRE  "ReturnToSelectAfterWire"
#define PERSISTVALUE_PREF_RETURNTOSELAFTERTEXT  "ReturnToSelectAfterText"

// DRAG & DROP NAME
#define DRAGDROP_COMPONENTNAME                  "SSTWORKBENCH_COMPONENT"

// Graphic Item Component Settings
#define COMPONENT_PORTYOFFSET                   20
#define COMPONENT_WIDTH                         160
#define COMPONENT_LEFT_X                        -80
#define COMPONENT_RIGHT_X                       COMPONENT_LEFT_X + COMPONENT_WIDTH
#define COMPONENT_TOP_Y                         0
#define COMPONENT_TYPENAME_OFFSET               5
#define COMPONENT_ZVALUE                        0.0
#define COMPONENT_PROPERTY_USERNAME             "User Name"
#define COMPONENT_PROPERTY_UNIQUENAME           "Unique Name"
#define COMPONENT_PROPERTY_COMPNAME             "Component Name"
#define COMPONENT_PROPERTY_COMPPARENTELEM       "Element Name"
#define COMPONENT_PROPERTY_DESCRIPTION          "Description"
#define COMPONENT_PROPERTY_TYPE                 "Type"
#define COMPONENT_PROPERTY_INDEX                "Index"
#define COMPONENT_PROPERTY_COMMENT              "Comment"
#define COMPONENT_PROPERTY_RANK                 "Rank"
#define COMPONENT_PROPERTY_WEIGHT               "Weight"
#define COMPONENT_DISPLAY_PREFIX_UNCAT          "[UNCAT] "
#define COMPONENT_DISPLAY_PREFIX_PROCESSOR      "[PROC] "
#define COMPONENT_DISPLAY_PREFIX_MEMORY         "[MEM] "
#define COMPONENT_DISPLAY_PREFIX_NETWORK        "[NET] "
#define COMPONENT_DISPLAY_PREFIX_SYSTEM         "[SYS] "
#define COMPONENT_DISPLAY_PREFIX_SSTCONFIG      "[SST] "
#define COMPONENT_DISPLAY_PREFIX_ERROR          "[ERROR] + "
#define COMPONENT_DISPLAY_MOVING_PORTS          "** MOVING PORTS (ESC to exit) **"

// Graphic Item Port Settings
#define PORT_LINE_LENGTH                        20
#define PORT_LINE_START_EDGE_OFFSET             1   // Distance from edge of component to start drawing line (a Point)
#define PORT_LINE_END_EDGE_OFFSET               6   // Distance from center of Ellipse to stop drawing line (a Point)
#define PORT_ELLIPSE_SIZE                       -5, -5, 10, 10
#define PORT_LINE_WIDTH                         3
#define PORT_LINE_COLOR                         Qt::black
#define PORT_COLOR_CONNECTED                    Qt::green
#define PORT_COLOR_DISCONNECTED                 Qt::red
#define PORT_NAME_FONT_SIZE                     8
#define PORT_NAME_Y_OFFSET                      5
#define PORT_UNCONFIG_MARKER_SIZE               -6, -6, 12, 12
#define PORT_UNCONFIG_MARKER_COLOR              Qt::blue
#define PORT_UNCONFIG_MARKER_ROTATION           45
#define PORT_UNCONFIGURED_STRING                "UNCONFIGURED - "
#define PORT_PROPERTY_DESCRIPTION               "Description"
#define PORT_PROPERTY_CONFIGURED_NAME           "Name"
#define PORT_PROPERTY_GENERIC_NAME              "Generic Name"
#define PORT_PROPERTY_PORTTYPE                  "Port Type"
#define PORT_PROPERTY_LATENCY                   "Latency"
#define PORT_PROPERTY_COMMENT                   "Comment"
#define PORT_PROPERTY_ORIG_NAME                 "Original Port Name"
#define PORT_PROPERTY_CONTROLPARAM              "Component Controlling Parameter"
#define PORT_TYPE_STATIC                        "Static"
#define PORT_TYPE_DYNAMIC                       "Dynamic"
#define PORT_DISCONNECT_MOVE_OFFSET             25

// Port Config Dialog Settings
#define CONFIGPORTDLG_DLGTITLE                  "Set Component Dynamic Ports"
#define CONFIGPORTDLG_COMPONENTTITLE            "Setup Dynamic Ports for Component:\n"
#define CONFIGPORTDLG_MAX_NUM_DYNAMIC_PORTS     100
#define CONFIGPORTDLG_HEADER_INDEX              "Index"
#define CONFIGPORTDLG_HEADER_DYN_PORT_NAME      "Dynamic Port Name"
#define CONFIGPORTDLG_HEADER_SET_NUM_PORTS      "Set Number of Ports"
#define CONFIGPORTDLG_HEADER_0_WIDTH            50
#define CONFIGPORTDLG_HEADER_1_WIDTH            200

// Parameter Config Dialog Settings
#define CONFIGPARAMDLG_DLGTITLE                 "Set Component Dynamic Parameter"
#define CONFIGPARAMDLG_COMPONENTTITLE           "Setup Dynamic Parameter"
#define CONFIGPARAMDLG_MAX_NUM_DYNAMIC_PARAMS   100
#define CONFIGPARAMDLG_HEADER_DYN_PARAM_NAME     "Dynamic Parameter Name"
#define CONFIGPARAMDLG_HEADER_SET_NUM_PARAMS    "Set Number of Parameters"
#define CONFIGPARAMDLG_HEADER_0_WIDTH           200

// Manage Modules Dialog Settings
#define MANAGEMODULESDLG_DLGTITLE               "Manage Component Modules"

// Port Info Data Settings
#define PORTINFO_UNCONFIGURED                   "UNCONFIGURED - "

// SSTInfo Data Settings
#define SSTINFO_COMPONENTTYPESTR_UNCATEGORIZED  "Uncategorized"
#define SSTINFO_COMPONENTTYPESTR_PROCESSOR      "Processor"
#define SSTINFO_COMPONENTTYPESTR_MEMORY         "Memory"
#define SSTINFO_COMPONENTTYPESTR_NETWORK        "Network"
#define SSTINFO_COMPONENTTYPESTR_SYSTEM         "System"
#define SSTINFO_COMPONENTTYPESTR_SSTCONFIG      "SST Configuration"
#define SSTINFO_COMPONENTTYPESTR_UNDEFINED      "ERROR - UNDEFINED"

// Graphic Item Text Settings
#define TEXT_ZVALUE                             1000.0
#define TEXT_EMPTYTEXTSTRING                    "Empty Text"

// Graphic Item Wire Settings
#define WIRE_SELECTED_ZVALUE                    100.0
#define WIRE_DESELECTED_ZVALUE                  -100.0
#define WIRE_PEN_WIDTH                          5
#define WIRE_HANDLE_RECT                        -6, -6, 12, 12
#define WIRE_HANDLE_START_CONNECTED             Qt::green
#define WIRE_HANDLE_END_CONNECTED               Qt::green
#define WIRE_HANDLE_START_DISCONNECTED          Qt::red
#define WIRE_HANDLE_END_DISCONNECTED            Qt::red
#define WIRE_COLOR_FULL_CONNECTED               Qt::green
#define WIRE_COLOR_DISCONNECTED                 Qt::red
#define WIRE_STYLE_SELECTED                     Qt::DashLine
#define WIRE_STYLE_DESELECTED                   Qt::SolidLine
#define WIRE_PROPERTY_COMMENT                   "Comment"
#define WIRE_PROPERTY_NUMBER                    "Index"

#endif // GLOBALDEF_H
