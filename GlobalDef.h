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
// GLOBAL ENUMERATIONS

// Enumeration for Identifying the Operation mode of the scene
enum SceneOperationMode_enum {SCENEMODE_DONOTHING, SCENEMODE_SELECTMOVEITEM, SCENEMODE_MOVEWIREHANDLE, SCENEMODE_ADDCOMPONENT, SCENEMODE_ADDWIRE, SCENEMODE_ADDTEXT, SCENEMODE_RUBBERBANDSELECT};

// Enumeration for Identifying the item type (NOTE: ITEMTYPE_END MUST ALWAYS BE LAST)
enum GraphicItemType_enum { ITEMTYPE_UNDEFINED, ITEMTYPE_COMPONENT, ITEMTYPE_PORT, ITEMTYPE_TEXT, ITEMTYPE_WIRE, ITEMTYPE_WIREHANDLE, ITEMTYPE_WIRELINESEGMENT, ITEMTYPE_END };

// READWRITE / READ ONLY FLAG (Is Property ReadOnly)
enum RO_Flag_enum { READWRITE, READONLY };

// NOTPROTECTED / PROTECTED FLAG (Is Property Protected from Delete)
enum PROTECTED_Flag_enum { NOTPROTECTED, PROTECTED };

// NOTEXPORTABLED / EXPORTEABLE FLAG (Is Property Exportable to Python)
enum EXPORTABLE_Flag_enum { NOTEXPORTABLE, EXPORTABLE };

// Enumeration for Identifying what side of the component the port is on
enum ComponentSide_enum { SIDE_LEFT, SIDE_RIGHT };

// Enumerations for Identifying the component type (NOTE: COMP_END MUST ALWAYS BE LAST)
enum ComponentType_enum { COMP_SSTSTARTUPCONFIGURATION, COMP_UNCATEGORIZED, COMP_PROCESSOR, COMP_MEMORY, COMP_NETWORK, COMP_SYSTEM, COMP_END };

// Enumeration for Identifying the object being processed
enum XMLParserCurrentlyProcessing_enum { NONE, ELEMENT, COMPONENT, INTROSPECTOR, EVENT, MODULE, PARTITIONER, GENERATOR };

// Component Type sorting method
enum SortType_enum {SORTBY_ELEMENT, SORTBY_TYPE};

//////////////////////////////////////////////////////////////////////

// CORE APPLICATION SETTINGS
#define COREAPP_APPNAME                         "SSTWorkBench"
#define COREAPP_MAINWINDOWNAME                  "SST WorkBench"
#define COREAPP_VERSION                         "2.0"                           /*** INCREMENT FOR NEW APP VERSION ***/
#define COREAPP_ORGNAME                         "Sandia_Corporation"
#define COREAPP_DOMAINNAME                      "sst.sandia.gov"
#define COREAPP_SINGLEINSTANCEKEY               "__SST_WORKBENCH_SINGLEINSTANCEKEY__"

// GENERAL ENUM DEFINES
#define NUMGRAPHICITEMTYPES                     ITEMTYPE_END
#define NUMCOMPONENTTYPES                       COMP_END

// XML FILE FORMAT VERSION ALLOWED
#define SSTINFOXMLFILEFORMATVERSION             "1.0"                           /*** INCREMENT IF SSTINFO XML FORMAT CHANGES ***/

// PROJECT FILE SETTINGS (UPDATED WHEN PROJECT FILE FORMAT CHANGES)
#define SSTWORKBENCHPROJECTFILEFORMATVER_2_0    200                                  /*** VERSION 2.0 FILE FORMAT ***/
#define SSTWORKBENCHPROJECTFILEFORMATVER_1_0    100                                  /*** VERSION 1.0 FILE FORMAT ***/
#define SSTWORKBENCHPROJECTFILECURRFORMATVER    SSTWORKBENCHPROJECTFILEFORMATVER_2_0 /*** CURRENT FILE FORMAT VERSION; INCREMENT IF WORKBENCH PROJECT FILE STRUCTURE CHANGES ***/
#define SSTWORKBENCHPROJECTFILEFORMATNEWESTVER  SSTWORKBENCHPROJECTFILECURRFORMATVER
#define SSTWORKBENCHPROJECTFILEFORMATOLDESTVER  SSTWORKBENCHPROJECTFILEFORMATVER_1_0
#define SSTWORKBENCHPROJECTFILEMAGICNUMBER      0xCD4234DF

// FILE NAME EXTENSTIONS AND FILTERS
#define PROJECTFILEEXTENSION                    ".swb"
#define PROJECTFILEEXTENSIONFILTER              "*.swb"
#define PYTHONEXPORTFILEEXTENSION               ".py"
#define PYTHONEXPORTFILEEXTENSIONFILTER         "*.py"

// MISC SETTINGS
#define UNTITLED                                "Untitled"
#define MOUSEMOVE_DELAYPIXELS                   5
#define DEFAULT_PASTE_OFFSET                    20
#define SHORTWIRELENGTHLIMIT                    20
#define DEFAULTGRIDSPACING                      20

// ZOOM IN AND OUT LIMITS
#define ZOOM_IN_LIMIT                           500
#define ZOOM_OUT_LIMIT                          25
#define ZOOM_STEP_SIZE                          25

// PERSISTENT DATA BETWEEN RUNS (SAVED IN PERSISTFILENAME)
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
#define PERSISTVALUE_PREF_AUTODELTOOSHORTWIRES  "AutoDeleteTooShortWires"
#define PERSISTVALUE_PREF_DISPLAYGRIDENABLE     "DisplayGridEnable"
#define PERSISTVALUE_PREF_SNAPTOGRIDENABLE      "SnapToGridEnable"
#define PERSISTVALUE_PREF_SNAPTOGRIDSIZE        "SnapToGridSize"
#define PERSISTVALUE_PREF_COMPWIDTHFULLSIZE     "ComponentWidthFullSize"

// DRAG & DROP NAME
#define DRAGDROP_COMPONENTNAME                  "SSTWORKBENCH_COMPONENT"

// GRAPHIC ITEM COMPONENT SETTINGS
#define COMPONENT_PORTYOFFSET                   DEFAULTGRIDSPACING * 1
#define COMPONENT_WIDTH                         DEFAULTGRIDSPACING * 8
#define COMPONENT_LEFT_X                        DEFAULTGRIDSPACING * -4
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

// GRAPHIC ITEM PORT SETTINGS
#define PORT_LINE_LENGTH                        DEFAULTGRIDSPACING * 1
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
#define PORT_PROPERTY_CONTROLPROP               "Component Controlling Property"
#define PORT_TYPE_STATIC                        "Static"
#define PORT_TYPE_DYNAMIC                       "Dynamic"
#define PORT_DISCONNECT_MOVE_OFFSET             25

// GRAPHIC ITEM TEXT SETTINGS
#define TEXT_ZVALUE                             1000.0
#define TEXT_EMPTYTEXTSTRING                    "Empty Text"

// GRAPHIC ITEM WIRE SETTINGS
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

// PORT CONFIG DIALOG SETTINGS
#define CONFIGPORTDLG_DLGTITLE                  "Set Component Dynamic Ports"
#define CONFIGPORTDLG_COMPONENTTITLE            "Setup Dynamic Ports for Component:\n"
#define CONFIGPORTDLG_MAX_NUM_DYNAMIC_PORTS     100
#define CONFIGPORTDLG_HEADER_INDEX              "Index"
#define CONFIGPORTDLG_HEADER_DYN_PORT_NAME      "Dynamic Port Name"
#define CONFIGPORTDLG_HEADER_SET_NUM_PORTS      "Set Number of Ports"
#define CONFIGPORTDLG_HEADER_0_WIDTH            50
#define CONFIGPORTDLG_HEADER_1_WIDTH            200

// PROPERTY CONFIG DIALOG SETTINGS
#define CONFIGPROPDLG_DLGTITLE                  "Set Component Dynamic Property"
#define CONFIGPROPDLG_COMPONENTTITLE            "Setup Dynamic Property"
#define CONFIGPROPDLG_MAX_NUM_DYNAMIC_PROPS     100
#define CONFIGPROPDLG_HEADER_DYN_PROP_NAME      "Dynamic Property Name"
#define CONFIGPROPDLG_HEADER_SET_NUM_PROPS      "Set Number of Properties"
#define CONFIGPROPDLG_HEADER_0_WIDTH            200

// MANAGE MODULE DIALOG SETTINGS
#define MANAGEMODULESDLG_DLGTITLE               "Manage Component Modules"

// PORT INFO DATA SETTINGS
#define PORTINFO_UNCONFIGURED                   "UNCONFIGURED - "

// SSTInfo DATA SETTINGS
#define SSTINFO_COMPONENTTYPESTR_UNCATEGORIZED  "Uncategorized"
#define SSTINFO_COMPONENTTYPESTR_PROCESSOR      "Processor"
#define SSTINFO_COMPONENTTYPESTR_MEMORY         "Memory"
#define SSTINFO_COMPONENTTYPESTR_NETWORK        "Network"
#define SSTINFO_COMPONENTTYPESTR_SYSTEM         "System"
#define SSTINFO_COMPONENTTYPESTR_SSTCONFIG      "SST Configuration"
#define SSTINFO_COMPONENTTYPESTR_UNDEFINED      "ERROR - UNDEFINED"

#endif // GLOBALDEF_H
