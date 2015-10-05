////////////////////////////////////////////////////////////////////////
// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// inform ation, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

////////////////////////////////////////////////////////////
// NOTE: MainWindow UI Setup and Processing routines are large
//       and bloat the file.  They are moved into a separate file
//       called MainWindow_UI_Handling.cpp.  Also Printout Handling
//       is in a file called MainWindow_Print_Handling.cpp
////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget* parent /*=0*/)
    : QMainWindow(parent)
{
    // Create the Undo Stack
    m_UndoStack = new QUndoStack(this);

    // Create the User Actions
    CreateActions();

    // Create the Menus and Toolbars that use the User Actions
    CreateMenus();
    CreateToolbars();
    CreateStatusbar();

    // Create the Undo Window (Useful for debugging the Undo Stack)
  m_UndoViewWindow = new QUndoView(m_UndoStack);
  m_UndoViewWindow->setWindowTitle(tr("Command List"));
  m_UndoViewWindow->show();
  m_UndoViewWindow->setAttribute(Qt::WA_QuitOnClose, false);
  m_UndoViewWindow->move(0, 0);

    // Set the Applications default font that Text uses in the scene
    m_DefaultFont = QGuiApplication::font();
    SetFontControls(m_DefaultFont);

    // Create the Tabbed Widget Window that will house multiple QGraphics View windows
    m_TabWiringWindow = new QTabWidget(this);
    m_TabWiringWindow->setUsesScrollButtons(true);
    m_TabWiringWindow->setMovable(true);

    // Create the Components Right Side Window
    m_CompToolBox = new WindowComponentToolBox(this);

    // Create the Properties Left Side Window
    m_PropWin = new WindowItemProperties(this);

    // Connect Window and other Signals to their Handlers
    connect(m_TabWiringWindow,    SIGNAL(currentChanged(int)),                                         this, SLOT(HandleTabWindowEventTabChanged(int)));
    connect(m_TabWiringWindow,    SIGNAL(tabCloseRequested(int)),                                      this, SLOT(HandleTabWindowEventTabCloseRequested(int)));
    connect(m_CompToolBox,        SIGNAL(ComponentToolboxItemPressed(SSTInfoDataComponent*)),          this, SLOT(ToolboxWindowActionComponentItemPressed(SSTInfoDataComponent*)));
    connect(m_UndoStack,          SIGNAL(cleanChanged(bool)),                                          this, SLOT(HandleUndoStackCleanChanged(bool)));
    connect(this,                 SIGNAL(PerformPreprocessingCommonActions()),                         this, SLOT(HandlePreprocessingCommonActionsTriggered()));

    // Create an initial page
    MenuActionPageNew(UNTITLED);

    // Layout the 3 Main Windows (Components, Scene, Properties) from left to right
    m_MainSplitterWidget = new QSplitter;
    m_MainSplitterWidget->addWidget(m_CompToolBox);
    m_MainSplitterWidget->addWidget(m_TabWiringWindow);
    m_MainSplitterWidget->addWidget(m_PropWin);

    // Set the Main widget as the Main window.
    setCentralWidget(m_MainSplitterWidget);

    // Set the window title
    SetMainWindowTitle(UNTITLED);

    // Initialize Application Persistant Data
    m_PersistentSettings = new QSettings(QDir::homePath() + PERSISTFILENAME, QSettings::IniFormat);
    RestorePersistentStorage();

    // Init variables
    m_LoadedProjectDataFilePathName = "";
    m_ProjectIsDirty = false;
    m_SelectedComponent = NULL;
    m_ComponentMovingPorts = NULL;
    m_SelectedText = NULL;
    m_PasteOffset = DEFAULT_PASTE_OFFSET;

    // Start the Focus on the main Wiring Window
    m_CurrentWiringView->setFocus();

    // Create a New Project
    MenuActionHandlerNewProject();
}

MainWindow::~MainWindow()
{
    if (m_PersistentSettings != NULL) {
        delete m_PersistentSettings;
    }
}

void MainWindow::SetProjectDirty(bool DirtyFlag /*=true*/)
{
    // Set the Dirty Flag
    m_ProjectIsDirty = DirtyFlag;

    // Also set the undo stack as clean at this point
    if (DirtyFlag == false) {
        m_UndoStack->setClean();
    }

    // Update the Menus and Toolbars
    UpdateMenuAndToolbars();
}

bool MainWindow::IsProjectDirty()
{
    return m_ProjectIsDirty || !(m_UndoStack->isClean());
}

void MainWindow::SetCurrentViewScaleByPercent(double ScalePercent)
{
    double  NewScalePercent = ScalePercent;

    // Limit how much we can zoom in
    if (NewScalePercent > ZOOM_IN_LIMIT) {
        NewScalePercent = ZOOM_IN_LIMIT;
    }

    // Limit how much we can zoom out
    if (NewScalePercent < ZOOM_OUT_LIMIT) {
        NewScalePercent = ZOOM_OUT_LIMIT;
    }

    // Round it to the closest step
    NewScalePercent = RoundTo(NewScalePercent, (double) ZOOM_STEP_SIZE);

    // Scale the view to the new size
    m_CurrentWiringView->ScaleViewToNewSize(NewScalePercent);

    // Update the Display
    UpdateCurrentViewScaleDisplay();
}

void MainWindow::SavePersistentStorage()
{
    // Save the Persistent Data (Between runs of this app)
    QSettings* settings = MainWindow::GetPersistentSettings();

    // Main Window Settings
    settings->beginGroup(PERSISTGROUP_MAINWINDOW);
    settings->setValue(PERSISTVALUE_SIZE, size());
    settings->setValue(PERSISTVALUE_POSIITON, pos());
    settings->setValue(PERSISTVALUE_MAINWINSTATE, saveState());
    settings->setValue(PERSISTVALUE_SPLITTERSTATE, m_MainSplitterWidget->saveState());
    settings->endGroup();

    // General Settings
    settings->beginGroup(PERSISTGROUP_GENERAL);
    settings->setValue(PERSISTVALUE_SSTINFOXMLFILEPATHNAME, m_LastSavedSSTInfoXMLDataFilePathName);
    settings->setValue(PERSISTVALUE_PROJECTFILEPATHNAME, m_LastSavedProjectDataFilePathName);
    settings->setValue(PERSISTVALUE_PYTHONEXPORTFILEPATHNAME, m_LastExportedPythonFilePathName);
    settings->endGroup();

    settings->beginGroup(PERSISTGROUP_PREFERENCES);
    settings->setValue(PERSISTVALUE_PREF_RETURNTOSELAFTERWIRE, ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingWire());
    settings->setValue(PERSISTVALUE_PREF_RETURNTOSELAFTERTEXT, ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingText());
    settings->setValue(PERSISTVALUE_PREF_AUTODELTOOSHORTWIRES, ApplicationPreferences::GetAppPref_AutoDeleteTooShortWires());
    settings->setValue(PERSISTVALUE_PREF_DISPLAYGRIDENABLE,    ApplicationPreferences::GetAppPref_DisplayGridEnabled());
    settings->setValue(PERSISTVALUE_PREF_SNAPTOGRIDENABLE,     ApplicationPreferences::GetAppPref_SnapToGridEnabled());
    settings->setValue(PERSISTVALUE_PREF_SNAPTOGRIDSIZE,       ApplicationPreferences::GetAppPref_SnapToGridSize());
    settings->endGroup();
}

void MainWindow::RestorePersistentStorage()
{
    // Restore the Persistent Data (Between runs of this app)
    QSettings* settings = MainWindow::GetPersistentSettings();

    // Main Window Settings
    settings->beginGroup(PERSISTGROUP_MAINWINDOW);
    resize(settings->value(PERSISTVALUE_SIZE, QSize(1200, 800)).toSize());
    move(settings->value(PERSISTVALUE_POSIITON, QPoint(200, 200)).toPoint());
    restoreState(settings->value(PERSISTVALUE_MAINWINSTATE).toByteArray());
    m_MainSplitterWidget->restoreState(settings->value(PERSISTVALUE_SPLITTERSTATE).toByteArray());
    settings->endGroup();

    // General Settings
    settings->beginGroup(PERSISTGROUP_GENERAL);
    m_LastSavedSSTInfoXMLDataFilePathName = settings->value(PERSISTVALUE_SSTINFOXMLFILEPATHNAME, QDir::homePath()).toString();
    m_LastSavedProjectDataFilePathName = settings->value(PERSISTVALUE_PROJECTFILEPATHNAME, QDir::homePath()).toString();
    m_LastExportedPythonFilePathName = settings->value(PERSISTVALUE_PYTHONEXPORTFILEPATHNAME, QDir::homePath()).toString();
    settings->endGroup();

    settings->beginGroup(PERSISTGROUP_PREFERENCES);
    ApplicationPreferences::SetAppPref_ReturnToSelectToolAfterPlacingWire(settings->value(PERSISTVALUE_PREF_RETURNTOSELAFTERWIRE, true).toBool());
    ApplicationPreferences::SetAppPref_ReturnToSelectToolAfterPlacingText(settings->value(PERSISTVALUE_PREF_RETURNTOSELAFTERTEXT, true).toBool());
    ApplicationPreferences::SetAppPref_AutoDeleteTooShortWires(settings->value(PERSISTVALUE_PREF_AUTODELTOOSHORTWIRES, true).toBool());
    ApplicationPreferences::SetAppPref_DisplayGridEnabled(settings->value(PERSISTVALUE_PREF_DISPLAYGRIDENABLE, false).toBool());
    ApplicationPreferences::SetAppPref_SnapToGridEnabled(settings->value(PERSISTVALUE_PREF_SNAPTOGRIDENABLE, true).toBool());
    ApplicationPreferences::SetAppPref_SnapToGridSize(settings->value(PERSISTVALUE_PREF_SNAPTOGRIDSIZE, 20).toInt());
    emit PreferenceChangedAutoDeleteTooShortWire(ApplicationPreferences::GetAppPref_AutoDeleteTooShortWires());
    settings->endGroup();
}

bool MainWindow::SaveProjectData(QString ProjectFilePathName)
{
    QByteArray SaveBuffer;
    QByteArray ComputedHash;
    int        x;

    // Create a Data Stream and Connect it to the Buffer
    QDataStream DataStreamOut(&SaveBuffer, QIODevice::WriteOnly);

    // Save Version Informatgion
    DataStreamOut << (quint32)SSTWORKBENCHPROJECTFILEMAGICNUMBER;  // SSTWorkbench Magic Number
    DataStreamOut << (qint32)SSTWORKBENCHPROJECTFILECURRFORMATVER; // SSTWorkbench File Format Version (The Current Version)
    DataStreamOut.setVersion(QDataStream::Qt_5_2);                 // QT's Serialization Version (for complex Types)

    // Start Streaming the data structures to the file
    if (m_CompToolBox->GetSSTInfoData() != NULL) {

        // Save the SSTInfo Data from the Component Toolbox
        m_CompToolBox->GetSSTInfoData()->SaveData(DataStreamOut);

        // Save GraphicItemIndex data
        GraphicItemData::SaveData(DataStreamOut);

        // Now save the info about pages including Scene and View Data
        DataStreamOut << m_PageNamesList;  // Save the Page Names (this will also identify the count of Pages)

        // Save off each scene attached to a page
        for (x = 0; x < m_PageNamesList.count(); x++) {

            // Save the Scene Data
            m_WiringScenesList.at(x)->SaveData(DataStreamOut);

            // Save the View Rect and Zoom Level
            QRectF  FullSceneRect = m_WiringViewsList.at(x)->sceneRect();
            QPointF SceneCenterPoint = m_WiringViewsList.at(x)->mapToScene(m_WiringViewsList.at(x)->viewport()->rect().center());

            DataStreamOut << FullSceneRect;
            DataStreamOut << SceneCenterPoint;
            DataStreamOut << m_WiringViewsList.at(x)->GetCurrentScalePercent();
        }
    }

    // Compute the Checksum of the Save Buffer
    QCryptographicHash Checksum(QCryptographicHash::Md5);
    Checksum.addData(SaveBuffer);
    ComputedHash = Checksum.result();

    // Add the Checksum Computed Hash to the front of the buffer
    SaveBuffer.prepend(ComputedHash);

    // Open the file and save the buffer
    QFile SaveFile(ProjectFilePathName);
    SaveFile.open(QIODevice::WriteOnly);
    SaveFile.write(SaveBuffer);
    SaveFile.close();

    // Set the Main Window Title
    SetMainWindowTitle(QFileInfo(ProjectFilePathName));

    SetProjectDirty(false);
    return true;
}

bool MainWindow::LoadProjectData(QString ProjectFilePathName)
{
    QByteArray  LoadBuffer;
    QByteArray  LoadHash;
    QByteArray  ComputeHash;
    int         x;
    qreal       ZoomPercent;
    QStringList TempPageNamesList;
    QRectF      FullSceneRect;
    QPointF     SceneCenterPoint;

    // Open the file & Read its contents
    QFile LoadFile(ProjectFilePathName);
    LoadFile.open(QIODevice::ReadOnly);
    LoadBuffer = LoadFile.readAll();
    LoadFile.close();

    // Get and then remove the 16 bytes from the front of the file.
    // This should be the MD5 Hash
    LoadHash = LoadBuffer.left(16);
    LoadBuffer.remove(0, 16);

    // Compute the Checksum of the Remaining Load Buffer
    QCryptographicHash Checksum(QCryptographicHash::Md5);
    Checksum.addData(LoadBuffer);
    ComputeHash = Checksum.result();

    // Check to see that the Loaded Hash and the Computed Hash match
    if (ComputeHash != LoadHash) {
        QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File Corrupted; Checksum is incorrect.").arg(ProjectFilePathName));
        return false;
    }

    // Create a Data Stream and Connect it to the Buffer
    QDataStream DataStreamIn(&LoadBuffer, QIODevice::ReadOnly);

    // Read the Magic Number and Validate it to ensure that this is an SSTWorkbench file
    quint32 MagicNumber;
    DataStreamIn >> MagicNumber;
    if (MagicNumber != SSTWORKBENCHPROJECTFILEMAGICNUMBER) {
        QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File is not a SSTWorkbench file").arg(ProjectFilePathName));
        return false;
    }

    // Read The Version to ensure that we are correct
    qint32 FileVersion;
    DataStreamIn >> FileVersion;
    if (FileVersion < SSTWORKBENCHPROJECTFILEFORMATOLDESTVER) {
        QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File is too OLD; Version is %2 and Expected Version is %3").arg(ProjectFilePathName).arg(FileVersion).arg(SSTWORKBENCHPROJECTFILECURRFORMATVER));
        return false;
    }
    if (FileVersion > SSTWORKBENCHPROJECTFILEFORMATNEWESTVER) {
        QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File is too NEW; Version is %2 and Expected Version is %3").arg(ProjectFilePathName).arg(FileVersion).arg(SSTWORKBENCHPROJECTFILECURRFORMATVER));
        return false;
    }

    // If the version is allwed, but old, warn the user (we only get here if we passed the above 2 checks)
    if (FileVersion < SSTWORKBENCHPROJECTFILECURRFORMATVER) {
        QMessageBox::information(NULL, "Upgrading File", QString("Note: Project File = %1 is formatted for a previous version of SST Workbench.\n\nSaving the file will upgrade it to the latest version.\n\n Note: If snap to grid is on, wire connections may be broken or misshapen ").arg(ProjectFilePathName));
    }

    // Set the Streaming Version based upon our SSTWORKBENCHFILEFORMATVERSION
    DataStreamIn.setVersion(QDataStream::Qt_5_2);

    // Going into this we do not know how many Pages (scenes/views) exist any how many tabs are open
    // there will alwoys be at least 1 tab and its assocated page, but could be more in any combinations display.
    // We want to close everything down until there is only a single tab along with its ascoated Page info open
    // We will then load the data appending to the tabs and page/scene/view lists and then remove the index 0 items
    // that are no longer needed.

    // Remove all visible Tabs (and their assocated Pages Scenes and views) except the first one; Therefore Only Tab Index 0 exists
    int TabCount = m_TabWiringWindow->count();
    for (x = TabCount - 1; x >= 1; x--) {
        MenuActionPageDelete(x);
    }

    // Remove any remaining Pages Scenes and views (not tied to a tab)
    int PageCount = m_PageNamesList.count();
    QString pagename;
    QString tabname;
    for (x = PageCount - 1; x >= 0 ; x--) {
        // Check to see if there are more Pages that existing that have a different name than the tab
        // These might have been pages with closed tabs at the time
        // Delete these extra pages
        pagename = m_PageNamesList.at(x);
        tabname = m_TabWiringWindow->tabText(0);
        if (m_PageNamesList.at(x) != m_TabWiringWindow->tabText(0)) {
            DeletePageSceneView(x);
        }
    }

    // Rename the Page (to something a user wont use)
    MenuActionPageRename("___OOOGA--BOOGA__");

    // Reset all GraphicItemData Lists
    GraphicItemData::ResetGraphicItemWireList();
    GraphicItemData::ResetGraphicItemWComponentList();
    GraphicItemData::ResetGraphicItemWComponentByTypeList();

    // Do we need to upgrade some of the properties
    if (FileVersion == SSTWORKBENCHPROJECTFILEFORMATVER_1_0) {
        // We have to make the version 1 data look like the current version
        // First Load the new data into Index 1 (index 0 was the remaining page info that we will dump at the end)

        // Create a new SSTInfoData Structure & Load the data from the file
        SSTInfoData* NewSSTInfoData = new SSTInfoData(DataStreamIn, FileVersion);

        // Add the new SSTInfoData to the Component Toolbax
        m_CompToolBox->LoadSSTInfo(NewSSTInfoData, false);

        // We need to create a page name (use the old "Project - Filename" format
        QString NewProjectName = "Project - " + QFileInfo(ProjectFilePathName).baseName();

        // Create a New Page, Scene and View
        MenuActionPageNew(NewProjectName);

        // Load the scene (at Index 1, Index 0 will be dumped soon)
        m_WiringScenesList.at(1)->LoadData(DataStreamIn, FileVersion);

        // Now dump index 0, thereby causing the Index 1 to become the only index (ie index 0)
        MenuActionPageDelete(0);

        // Display all GraphicItems
        MenuActionSceneScaleZoomAll();
    }

    if (FileVersion == SSTWORKBENCHPROJECTFILECURRFORMATVER) {  // Currently Version 2.0

        // Create a new SSTInfoData Structure & Load the data from the file
        SSTInfoData* NewSSTInfoData = new SSTInfoData(DataStreamIn, FileVersion);

        // Add the new SSTInfoData to the Component Toolbax
        m_CompToolBox->LoadSSTInfo(NewSSTInfoData, false);

        // Load the GraphicItemIndex data
        GraphicItemData::LoadData(DataStreamIn, FileVersion);

        // Get the List of page names
        DataStreamIn >> TempPageNamesList;

        for (x = 0; x < TempPageNamesList.count(); x++) {
            // First Load the new data into Index 1 (index 0 was the remaining page info that we will dump at the end)

            // Create a New Page, Scene and View
            MenuActionPageNew(TempPageNamesList.at(x));

            // Load the scene into the next index
            m_WiringScenesList.at(x + 1)->LoadData(DataStreamIn, FileVersion);

            // Get the View's Rect and Zoom Percent and set them
            DataStreamIn >> FullSceneRect;
            DataStreamIn >> SceneCenterPoint;
            DataStreamIn >> ZoomPercent;
            m_WiringViewsList.at(x + 1)->centerOn(SceneCenterPoint);
            m_WiringViewsList.at(x + 1)->ScaleViewToNewSize(ZoomPercent);
        }
        // Now dump index 0, thereby causing the Index 1 to become the only index (ie index 0)
        MenuActionPageDelete(0);

        // Choose the tab at Index 0 to be the current tab
        m_TabWiringWindow->setCurrentIndex(0);
    }

    // Set the current Scene and View to the new selected tab
    m_CurrentWiringScene = m_WiringScenesList.at(0);
    m_CurrentWiringView  = m_WiringViewsList.at(0);

    // Set nothing selected
    m_CurrentWiringScene->SetNothingSelected();

    // Set the Main Window Title
    SetMainWindowTitle(QFileInfo(ProjectFilePathName));

    SetProjectDirty(false);
    return true;
}

void MainWindow::EnableMovingPorts(bool Enable)
{
    if (Enable == true) {
        // Make sure a Component has been selected
        // Then enable the Component to move ports
        if (m_SelectedComponent != NULL) {
            if (m_ComponentMovingPorts == NULL) {
                m_ComponentMovingPorts = m_SelectedComponent;
                m_ComponentMovingPorts->SetMovingPortsMode(true);
            }
        }
    } else {
        if (m_ComponentMovingPorts != NULL) {
            m_ComponentMovingPorts->SetMovingPortsMode(false);
            m_ComponentMovingPorts = NULL;
        }
    }
}

void MainWindow::ClearCopyPasteBuffer()
{
    int x;
    int count;

    // Delete all ByteArrays in the CopyPasteBufferLists
    count = m_CopyPasteBufferListComps.count();
    for (x = 0; x < count; x++) {
        delete m_CopyPasteBufferListComps.at(x);
    }
    m_CopyPasteBufferListComps.clear();

    count = m_CopyPasteBufferListWires.count();
    for (x = 0; x < count; x++) {
        delete m_CopyPasteBufferListWires.at(x);
    }
    m_CopyPasteBufferListWires.clear();
    m_CopyPasteWireBasesList.clear();

    count = m_CopyPasteBufferListTexts.count();
    for (x = 0; x < count; x++) {
        delete m_CopyPasteBufferListTexts.at(x);
    }
    m_CopyPasteBufferListTexts.clear();

    m_PasteOffset = DEFAULT_PASTE_OFFSET;
}

void MainWindow::HandlePreprocessingCommonActionsTriggered()
{
    // All menu and toolbar actions will call this Signal Handler first to perform global actions

    // Disable Moving Ports
    EnableMovingPorts(false);
}

void MainWindow::ToolboxWindowActionComponentItemPressed(SSTInfoDataComponent* ptrComponent)
{
    // THIS HANDLER IS CALLED WHEN A BUTTON IN THE COMPONENT TOOLBOX (LEFT WINDOW) IS PRESSED DOWN

    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    if (ptrComponent != NULL) {
        // Set the WiringScene Operation Mode to Add Components
        m_CurrentWiringScene->SetOperationMode(SceneOperationMode_enum(SCENEMODE_ADDCOMPONENT));

        // Now Set the component from SSTInfo into the Scene to use when the user places it
        m_CurrentWiringScene->SetUserChosenSSTInfoDataComponent(ptrComponent);

        // Since we are in MODE_ADDCOMPONENT, turn off the ANY of the other Pointer
        // Toolbar Buttons (MODE_ADDWIRE, MODE_ADDTEXT, or MODE_SELECTMOVEITEM)
        TurnOffAllPointerToolButtons();
    } else {
        // No component was selected go back to the SelectMoveItem mode and set the button
        m_CurrentWiringScene->SetOperationMode(SceneOperationMode_enum(SCENEMODE_SELECTMOVEITEM));
        TurnOnAPointerToolButton(SCENEMODE_SELECTMOVEITEM);
    }
}

void MainWindow::ToolButtonGroupActionPointerGroupClicked(int id)
{
    // THIS HANDLER IS CALLED WHEN ONE OF THE 3 BUTTONS OF THE
    // ACTION POINTER GROUP (TOOBAR) IS CLICKED

    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // Turn off any selected ToolBox component
    m_CompToolBox->UnselectAllToolboxComponentItems();

    // Get the id of the button we press (this will be one of the WiringScene Modes)
    // MODE_ADDWIRE, MODE_ADDTEXT, or MODE_SELECTMOVEITEM

    // Set the WiringScene Operation Mode to the new ID
    m_CurrentWiringScene->SetOperationMode(SceneOperationMode_enum(id));
    TurnOnAPointerToolButton(id);
}

void MainWindow::HandleSceneEventComponentAdded(GraphicItemComponent* Item)
{
    // Handler called when a Component Item is added to the scene
    Q_UNUSED(Item)

    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();
}

void MainWindow::HandleSceneEventTextAdded(GraphicItemText* Item)
{
    // Handler called when a Text Item is added to the scene
    Q_UNUSED(Item)

    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // Check to see if we need to return to the Select Tool after adding the text
    if (ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingText() == true) {
        ClickPointToolButton(int(SCENEMODE_SELECTMOVEITEM));
    }
}

void MainWindow::HandleSceneEventWireAddedInitialPlacement(GraphicItemWire* Item)
{
    // Handler called when a Wire Item is added to the scene
    // and the start point has been set
    Q_UNUSED(Item)

    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();
}

void MainWindow::HandleSceneEventWireAddedFinalPlacement(GraphicItemWire* Item)
{
    // Handler called when a Wire Item is added to the scene
    // and the end point has been set
    Q_UNUSED(Item)

    // Check to see if we need to return to the Select Tool after adding the wire
    if (ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingWire() == true) {
        ClickPointToolButton(int(SCENEMODE_SELECTMOVEITEM));
    }
}

void MainWindow::HandleSceneEventSelectionChanged()
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // Update the Menus and Toolbars
    UpdateMenuAndToolbars();
}

void MainWindow::HandleSceneEventGraphicItemSelected(QGraphicsItem* Item)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // NOTE: Item may be NULL, to represent that no item is selected

    // The scene has some item selected, we first just want to clear the properties window
    m_PropWin->ClearProperiesWindow();

    // Turn off component menu items initially
    m_SelectedComponent = NULL;
    m_SelectedText = NULL;

    // Update the Menus and Toolbars
    UpdateMenuAndToolbars();

    // Decide if an Item was selected (Item is not NULL)
    if (Item != NULL) {
        // Is the Item a GraphicItemComponent?  (If it is not, then SelectedComponent is set to NULL)
        m_SelectedComponent = qgraphicsitem_cast<GraphicItemComponent*>(Item);
        if (m_SelectedComponent != NULL) {
            // Update the Menus and Toolbars
            UpdateMenuAndToolbars();
            return;
        }

        // Check is see if the item is a GraphicItemText
        m_SelectedText = qgraphicsitem_cast<GraphicItemText*>(Item);
        if (m_SelectedText != NULL) {
            // Set the Font user controls to the appropriate settings
            SetFontControls(m_SelectedText->font());
        } else {
            // Reset the Font to the Default
            SetFontControls(m_DefaultFont);
        }
    } else {
        // Reset the Font to the Default
        SetFontControls(m_DefaultFont);
    }
}

void MainWindow::HandleSceneEventGraphicItemSelectedProperties(ItemProperties* Properties)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // The scene has identifed an object with properties has been selected, show those properties
    if (Properties != NULL) {
        m_PropWin->SetGraphicItemProperties(Properties);
    } else {
        // If Null clear the properties window
        m_PropWin->ClearProperiesWindow();
    }
}

void MainWindow::HandleSceneEventSetProjectDirty()
{
    // The scene needs to set the project dirty
    SetProjectDirty();
}

void MainWindow::HandleSceneEventRefreshPropertiesWindowProperty(QString PropertyName, QString NewPropertyValue)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // Refresh the Properties in the properties window
    m_PropWin->RefreshProperiesWindowProperty(PropertyName, NewPropertyValue);
}

void MainWindow::HandleSceneEventRefreshPropertiesWindow(ItemProperties* Properties)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    // Refresh all the Properties in the properties window
    m_PropWin->SetGraphicItemProperties(Properties);
}

void MainWindow::HandleSceneEventDragAndDropFinished()
{
    // The scene just finished a Drag & Drop event
    // Lets automatically choose the Select/Move tool by "Pretend" clicking on it.
    // This will also automatically uncheck the Component in the Toolbox.
    // to prevent "accidental" placement of more components
    ClickPointToolButton(int(SCENEMODE_SELECTMOVEITEM));
}

void MainWindow::MenuActionHandlerCut()
{
    // Copy all the objects
    MenuActionHandlerCopy();

    // And delete all the selected objects
    m_CurrentWiringScene->DeleteAllSelectedItems();
}

void MainWindow::MenuActionHandlerCopy()
{
    GraphicItemBase*            BaseItem = NULL;
    QByteArray*                 ItemBuffer;
    QDataStream*                DataStream;
    GraphicItemComponent*       Component;
    GraphicItemWire*            Wire;
    GraphicItemWireLineSegment* WireSeg;
    GraphicItemText*            Text;

    ClearCopyPasteBuffer();

    foreach (QGraphicsItem* SelectedItem, m_CurrentWiringScene->GetAllSelectedGraphicalItems()) {
        BaseItem = NULL;
        Wire = NULL;
        WireSeg = NULL;

        // Convert the SelectedItem into one of the 3 types of GraphicItemXXX classes
        // Note: The GraphicItemXXX variable will be null if type is incorrect
        if (BaseItem == NULL) {
            // LOOKING FOR COMPONENT OBJECTS
            Component = qgraphicsitem_cast<GraphicItemComponent*>(SelectedItem);
            BaseItem = qgraphicsitem_cast<GraphicItemComponent*>(SelectedItem);
        }

        if (BaseItem == NULL) {
            // User can only select WireSegments (not actual GraphicItemWires)
            WireSeg = qgraphicsitem_cast<GraphicItemWireLineSegment*>(SelectedItem);
            // If it is a wire segment, get its parent wire
            // There are muliple segments to a wire, so the wire may be redundant,
            // We handle this below
            if (WireSeg != NULL) {
                // LOOKING FOR WIRE OBJECTS
                Wire = (GraphicItemWire*)WireSeg->parentItem();
                BaseItem = (GraphicItemBase*)Wire;
            }
        }

        if (BaseItem == NULL) {
            // LOOKING FOR TEXT OBJECTS
            Text = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
            BaseItem = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
        }

        // Now Take the Item and perform its save command to a memory buffer,
        // Then add that buffer to an array of buffers for that Object type
        if (BaseItem != NULL) {
            // Create a Binary Data Array
            ItemBuffer = new QByteArray();

            // Build a DataStream on top of the ByteArray
            DataStream = new QDataStream(ItemBuffer, QIODevice::WriteOnly);

            // Now have the Item Save itself to the ByteArray (via the Datastream)
            BaseItem->SaveData(*DataStream);

            // Delete the datastream as it is no longer needed
            delete DataStream;

            if (BaseItem->GetItemType() == ITEMTYPE_COMPONENT) {
                // Save buffer to the array for Component buffers
                m_CopyPasteBufferListComps.append(ItemBuffer);
            }

            if (BaseItem->GetItemType() == ITEMTYPE_WIRE) {
                // Check to see if we are processing a redundant wire
                if (m_CopyPasteWireBasesList.contains(BaseItem) == false) {
                    // Not Redundant; add its databuffer and the wire to the lists
                    m_CopyPasteBufferListWires.append(ItemBuffer);
                    m_CopyPasteWireBasesList.append(BaseItem);
                } else {
                    // This wire was redundant, throw away the created buffer
                    delete ItemBuffer;
                }
            }

            if (BaseItem->GetItemType() == ITEMTYPE_TEXT) {
                // Save buffer to the array for Text buffers
                m_CopyPasteBufferListTexts.append(ItemBuffer);
            }
        }
    }
}

void MainWindow::MenuActionHandlerPaste()
{
    int                   x;
    int                   count;
    QByteArray*           ItemBuffer;
    QDataStream*          DataStream;

    // Select Nothing
    m_CurrentWiringScene->SetNothingSelected();

    // Add Paste all Component Items
    count = m_CopyPasteBufferListComps.count();
    for (x = 0; x < count; x++) {

        // Create a Binary Data Array
        ItemBuffer = new QByteArray();

        // Build a DataStream on top of the ByteArray
        DataStream = new QDataStream(m_CopyPasteBufferListComps.at(x), QIODevice::ReadOnly);

        // Tell the Scene to Paste the Item
        m_CurrentWiringScene->PasteNewComponentItem(*DataStream, m_PasteOffset);

        // Delete the datastream as it is no longer needed
        delete DataStream;
    }

    // Add Paste all Wire Items
    count = m_CopyPasteBufferListWires.count();
    for (x = 0; x < count; x++) {

        // Create a Binary Data Array
        ItemBuffer = new QByteArray();

        // Build a DataStream on top of the ByteArray
        DataStream = new QDataStream(m_CopyPasteBufferListWires.at(x), QIODevice::ReadOnly);

        // Tell the Scene to Paste the Item
        m_CurrentWiringScene->PasteNewWireItem(*DataStream, m_PasteOffset);

        // Delete the datastream as it is no longer needed
        delete DataStream;
    }

    // Add Paste all Text Items
    count = m_CopyPasteBufferListTexts.count();
    for (x = 0; x < count; x++) {

        // Create a Binary Data Array
        ItemBuffer = new QByteArray();

        // Build a DataStream on top of the ByteArray
        DataStream = new QDataStream(m_CopyPasteBufferListTexts.at(x), QIODevice::ReadOnly);

        // Tell the Scene to Paste the Item
        m_CurrentWiringScene->PasteNewTextItem(*DataStream, m_PasteOffset);

        // Delete the datastream as it is no longer needed
        delete DataStream;
    }

    // Increase the Paste Offset
    m_PasteOffset += DEFAULT_PASTE_OFFSET;
}

void MainWindow::MenuActionHandlerNewProject()
{
    // Check to see if the existing project is dirty before loading a new one
    if (IsProjectDirty() == true) {
        QString errorString = "Changes have been made to the project\n Are you sure you want to loose your changes and create a new project?";
        if (QMessageBox::question(NULL, "Loose Changes?", errorString) == QMessageBox::No) {
            return;
        }
    }

    // Select and delete all objects
    MenuActionHandlerSelectAll();
    if (m_CurrentWiringScene->GetNumSelectedGraphicalItems() > 0) {
        MenuActionHandlerDeleteItem();
    }

    // Reset the Current Project Filename
    SetMainWindowTitle(UNTITLED);
    m_LoadedProjectDataFilePathName = "";

    SetProjectDirty(false);

    // Update the Menus and Toolbars
    UpdateMenuAndToolbars();
}

void MainWindow::MenuActionHandlerLoadDesign()
{
    QString ProjectFilePathName;
    QDir    DirInfo;
    QString StartingDir;
    QString Filter;
    QString TempPath;

    // Check to see if the existing project is dirty before loading a new one
    if (IsProjectDirty() == true) {
        QString errorString = "Changes have been made to the project\n Are you sure you want to loose your changes and load a new project?";
        if (QMessageBox::question(NULL, "Loose Changes?", errorString) == QMessageBox::No) {
            return;
        }
    }

    // Look at the Location for the SSTInfo XML data to see if its good
    TempPath = QFileInfo(m_LastSavedProjectDataFilePathName).dir().absolutePath();
    DirInfo.setPath(TempPath);

    // Check to see that the Starting Directory Exists
    if (DirInfo.exists() == false) {
        // Starting Dir does not exist for some reason, so lets
        // fallback to the users home directory
        DirInfo.setPath(QDir::homePath());

        // Now lets check again, if the directory is still bad, then tell the user something is wrong
        if (DirInfo.exists() == false) {
            QString errorString = "ERROR: SSTWorkbench cannot find a starting directory, try deleting the ~/.SSTWorkbench file";
            QMessageBox::critical(NULL, "Cannot Identify Starting Directory", errorString);
            return;
        }
    }

    // Get the Starting Directory
    StartingDir = DirInfo.absolutePath();

    // Execute the File Open Dialog
    Filter = QString("SSTWorkbench Project Files (") + PROJECTFILEEXTENSIONFILTER + ")";
    ProjectFilePathName = QFileDialog::getOpenFileName(this, tr("Open SSTWorkbench Project"), StartingDir, Filter);

    // Process the file
    if (ProjectFilePathName.isEmpty() == false) {
        // User selected a file, so now load the project data from it
        if (LoadProjectData(ProjectFilePathName) == true) {
            SetProjectDirty(false);

            // Update the Menus and Toolbars
            UpdateMenuAndToolbars();

            m_LoadedProjectDataFilePathName = ProjectFilePathName;
            m_LastSavedProjectDataFilePathName = ProjectFilePathName;
        } else {
            QString errorString = QString("ERROR: SSTWorkbench Cannot Load Project File %1").arg(ProjectFilePathName);
            QMessageBox::critical(NULL, "Cannot Load Project File", errorString);
            return;
        }
    }
}

void MainWindow::MenuActionHandlerSaveDesign()
{
    // See if the user has set a Project Save Filename
    if (m_LoadedProjectDataFilePathName.isEmpty() == true) {
        MenuActionHandlerSaveAs();
    } else {
        // User wants to save
        if (SaveProjectData(m_LoadedProjectDataFilePathName) != true) {
            QString errorString = QString("ERROR: SSTWorkbench Cannot Save Project File %1").arg(m_LoadedProjectDataFilePathName);
            QMessageBox::critical(NULL, "Cannot Save Project File", errorString);
        }
    }
}

void MainWindow::MenuActionHandlerSaveAs()
{
    QString ProjectFilePathName;
    QString StartingDir;
    QDir    DirInfo;
    QString Filter;
    QString TempPath;

    // Look at the Location for the Project data to see if its good
    TempPath = QFileInfo(m_LastSavedProjectDataFilePathName).dir().absolutePath();
    DirInfo.setPath(TempPath);

    // Check to see that the Starting Directory Exists
    if (DirInfo.exists() == false) {
        // Starting Dir does not exist for some reason, so lets
        // fallback to the users home directory
        DirInfo.setPath(QDir::homePath());

        // Now lets check again, if the directory is still bad, then tell the user something is wrong
        if (DirInfo.exists() == false) {
            QString errorString = "ERROR: SSTWorkbench cannot find a starting directory, try deleting the ~/.SSTWorkbench file";
            QMessageBox::critical(NULL, "Cannot Identify Starting Directory", errorString);
            return;
        }
    }

    // Get the Starting Directory
    StartingDir = DirInfo.absolutePath();

    // Execute the File Save As Dialog
    Filter = QString("SSTWorkbench Project Files (") + PROJECTFILEEXTENSIONFILTER + ")";
    ProjectFilePathName = QFileDialog::getSaveFileName(this, tr("Save SSTWorkbench Project As"), StartingDir, Filter, 0, QFileDialog::DontUseNativeDialog);

    // Process the file
    if (ProjectFilePathName.isEmpty() == false) {

        // Check to see if the ProjectFileName has a proper extension
        if (ProjectFilePathName.endsWith(PROJECTFILEEXTENSION) == false) {
            // Append the extension
            ProjectFilePathName += PROJECTFILEEXTENSION;
        }

        // User selected a file, so now save the project data to it
        if (SaveProjectData(ProjectFilePathName) == true) {
            m_LoadedProjectDataFilePathName = ProjectFilePathName;
            m_LastSavedProjectDataFilePathName = ProjectFilePathName;
        } else {
            QString errorString = QString("ERROR: SSTWorkbench Cannot Save Project File %1").arg(ProjectFilePathName);
            QMessageBox::critical(NULL, "Cannot Save Project File", errorString);
            return;
        }
    }
}

void MainWindow::MenuActionHandlerImportSSTInfo()
{
    QString              XMLFileName;
    QDir                 DirInfo;
    QString              StartingDir;
    SSTInfoXMLFileParser XMLFileParser;
    QString              TempPath;

    // Check to see if the existing project has existing SSTInfoData
    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        QString errorString = "Are you sure you want to loose your existing SSTInfo Data and load a new one?";
        if (QMessageBox::question(NULL, "Change SSTInfo Data?", errorString) == QMessageBox::No) {
            return;
        }
    }

    // Look at the Location for the SSTInfo XML data to see if its good
    TempPath = QFileInfo(m_LastSavedSSTInfoXMLDataFilePathName).dir().absolutePath();
    DirInfo.setPath(TempPath);

    // Check to see that the Starting Directory Exists
    if (DirInfo.exists() == false) {
        // Starting Dir does not exist for some reason, so lets
        // fallback to the users home directory
        DirInfo.setPath(QDir::homePath());

        // Now lets check again, if the directory is still bad, then tell the user something is wrong
        if (DirInfo.exists() == false) {
            QString errorString = "ERROR: SSTWorkbench cannot find a starting directory, try deleting the ~/.SSTWorkbench file";
            QMessageBox::critical(NULL, "Cannot Identify Starting Directory", errorString);
            return;
        }
    }

    // Get the Starting Directory
    StartingDir = DirInfo.absolutePath();

    // Execute the File Open Dialog
    XMLFileName = QFileDialog::getOpenFileName(this, tr("Open SSTInfo XML Data"), StartingDir, tr("XML Files (*.xml)"));

    // Process the file
    if (XMLFileName.isEmpty() == false) {
        // User selected a file, so now Import the SSTInfo from it, and set the member variable that tracks it
        XMLFileParser.SetXMLInputFile(XMLFileName);
        if (XMLFileParser.ParseXMLFile() == true) {
            // Get the SSTInfoData structure created by the XMLFileParser
            m_CompToolBox->LoadSSTInfo(XMLFileParser.GetParsedSSTInfoData(), true);
            SetProjectDirty();

            // Update the Menus and Toolbars
            UpdateMenuAndToolbars();

            // Save the File Name and Path settings
            m_LastSavedSSTInfoXMLDataFilePathName = XMLFileName;
        }
    }
}

void MainWindow::MenuActionHandlerExportSSTInputDeck()
{
    QString ProjectFilePathName;
    QString StartingDir;
    QDir    DirInfo;
    QString Filter;
    QString TempPath;

    // Look at the Location for the Python data to see if its good
    TempPath = QFileInfo(m_LastExportedPythonFilePathName).dir().absolutePath();
    DirInfo.setPath(TempPath);

    // Check to see that the Starting Directory Exists
    if (DirInfo.exists() == false) {
        // Starting Dir does not exist for some reason, so lets
        // fallback to the users home directory
        DirInfo.setPath(QDir::homePath());

        // Now lets check again, if the directory is still bad, then tell the user something is wrong
        if (DirInfo.exists() == false) {
            QString errorString = "ERROR: SSTWorkbench cannot find a starting directory, try deleting the ~/.SSTWorkbench file";
            QMessageBox::critical(NULL, "Cannot Identify Starting Directory", errorString);
            return;
        }
    }

    // Get the Starting Directory
    StartingDir = DirInfo.absolutePath();

    // Execute the File Save As Dialog
    Filter = QString("SST Input Python Files (") + PYTHONEXPORTFILEEXTENSIONFILTER + ")";
    ProjectFilePathName = QFileDialog::getSaveFileName(this, tr("Export SST Input Python File to"), StartingDir, Filter, 0, QFileDialog::DontUseNativeDialog);

    // Process the file
    if (ProjectFilePathName.isEmpty() == false) {

        // Check to see if the ProjectFileName has a proper extension
        if (ProjectFilePathName.endsWith(PYTHONEXPORTFILEEXTENSION) == false) {
            // Append the extension
            ProjectFilePathName += PYTHONEXPORTFILEEXTENSION;
        }

        // Create the Python Exporter
        PythonExporter PythonExport(m_CurrentWiringScene, ProjectFilePathName);

        // User selected a file, so now save the project data to it
        PythonExport.PerformExportOfPythonFile();

        // Save the File Name and Path settings
        m_LastExportedPythonFilePathName = ProjectFilePathName;
    }
}

void MainWindow::MenuActionHandlerPreferences()
{
    int nRtn;

   // Create the Dialog
    m_PreferencesDialog = new DialogPreferences(this);

    m_PreferencesDialog->SetFlagReturnToSelectAfterWirePlaced(ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingWire());
    m_PreferencesDialog->SetFlagReturnToSelectAfterTextPlaced(ApplicationPreferences::GetAppPref_ReturnToSelectToolAfterPlacingText());
    m_PreferencesDialog->SetFlagAutomaticallyDeleteShortWires(ApplicationPreferences::GetAppPref_AutoDeleteTooShortWires());
    m_PreferencesDialog->SetFlagDisplayGridEnabled(ApplicationPreferences::GetAppPref_DisplayGridEnabled());
    m_PreferencesDialog->SetFlagSnapToGridEnabled(ApplicationPreferences::GetAppPref_SnapToGridEnabled());
    m_PreferencesDialog->SetGridSize(ApplicationPreferences::GetAppPref_SnapToGridSize());

    // Run the dialog box (Modal)
    nRtn = m_PreferencesDialog->exec();

    if (nRtn == QDialog::Accepted) {
        ApplicationPreferences::SetAppPref_ReturnToSelectToolAfterPlacingWire(m_PreferencesDialog->GetFlagReturnToSelectAfterWirePlaced());
        ApplicationPreferences::SetAppPref_ReturnToSelectToolAfterPlacingText(m_PreferencesDialog->GetFlagReturnToSelectAfterTextPlaced());
        ApplicationPreferences::SetAppPref_AutoDeleteTooShortWires(m_PreferencesDialog->GetFlagAutomaticallyDeleteShortWires());
        ApplicationPreferences::SetAppPref_DisplayGridEnabled(m_PreferencesDialog->GetFlagDisplayGridEnabled());
        ApplicationPreferences::SetAppPref_SnapToGridEnabled(m_PreferencesDialog->GetFlagSnapToGridEnabled());
        ApplicationPreferences::SetAppPref_SnapToGridSize(m_PreferencesDialog->GetGridSize());

        emit PreferenceChangedAutoDeleteTooShortWire(ApplicationPreferences::GetAppPref_AutoDeleteTooShortWires());
    }

    // Delete the Dialog
    delete m_PreferencesDialog;
}

