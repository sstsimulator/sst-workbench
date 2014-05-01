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

#include "MainWindow.h"
#include <QtPrintSupport>

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

    // Create the Undo Window (Useful for debugging the Undo Stack
//  m_UndoViewWindow = new QUndoView(m_UndoStack);
//  m_UndoViewWindow->setWindowTitle(tr("Command List"));
//  m_UndoViewWindow->show();
//  m_UndoViewWindow->setAttribute(Qt::WA_QuitOnClose, false);

    // Set the Applications default font that Text uses in the scene
    m_DefaultFont = QGuiApplication::font();
    SetFontControls(m_DefaultFont);

    // Build the Wiring Scene and set its initial size (of the scene window, not the viewport)
    m_WiringScene = new WiringScene(m_GenericItemMenu, m_UndoStack, this);
    m_WiringScene->setSceneRect(QRectF(0, 0, 5000, 5000));

    // Build the view object for the wiring scene
    m_WiringView = new QGraphicsView(m_WiringScene);

    // Create the Tabbed Widget Window that will house multiple QGraphics View windows
    m_TabWiringWindow = new QTabWidget(this);
    m_TabWiringWindow->addTab(m_WiringView, "RENAME_ME");
    SetMainTabTitle(UNTITLED);

    // Connect Scene Event Signals to Main Window Handlers
    connect(m_WiringScene, SIGNAL(SceneEventComponentAdded(GraphicItemComponent*)),             this, SLOT(HandleSceneEventComponentAdded(GraphicItemComponent*)));
    connect(m_WiringScene, SIGNAL(SceneEventTextAdded(GraphicItemText*)),                       this, SLOT(HandleSceneEventTextAdded(GraphicItemText*)));
    connect(m_WiringScene, SIGNAL(SceneEventWireAddedInitialPlacement(GraphicItemWire*)),       this, SLOT(HandleSceneEventWireAddedInitialPlacement(GraphicItemWire*)));
    connect(m_WiringScene, SIGNAL(SceneEventWireAddedFinalPlacement(GraphicItemWire*)),         this, SLOT(HandleSceneEventWireAddedFinalPlacement(GraphicItemWire*)));
    connect(m_WiringScene, SIGNAL(selectionChanged()),                                          this, SLOT(HandleSceneEventSelectionChanged()));
    connect(m_WiringScene, SIGNAL(SceneEventGraphicItemSelected(QGraphicsItem*)),               this, SLOT(HandleSceneEventGraphicItemSelected(QGraphicsItem*)));
    connect(m_WiringScene, SIGNAL(SceneEventGraphicItemSelectedProperties(ItemProperties*)),    this, SLOT(HandleSceneEventGraphicItemSelectedProperties(ItemProperties*)));
    connect(m_WiringScene, SIGNAL(SceneEventSetProjectDirty()),                                 this, SLOT(HandleSceneEventSetProjectDirty()));
    connect(m_WiringScene, SIGNAL(SceneEventRefreshPropertiesWindowProperty(QString, QString)), this, SLOT(HandleSceneEventRefreshPropertiesWindowProperty(QString, QString)));
    connect(m_WiringScene, SIGNAL(SceneEventRefreshPropertiesWindow(ItemProperties*)),    this, SLOT(HandleSceneEventRefreshPropertiesWindow(ItemProperties*)));
    connect(m_WiringScene, SIGNAL(SceneEventDragAndDropFinished()),                             this, SLOT(HandleSceneEventDragAndDropFinished()));
    connect(m_UndoStack,   SIGNAL(cleanChanged(bool)),                                          this, SLOT(HandleUndoStackCleanChanged(bool)));

    // Create the Components Right Side Window
    m_CompToolBox = new WindowComponentToolBox(this);

    // Create the Properties Left Side Window
    m_PropWin = new WindowItemProperties(this);

    // Layout the 3 Main Windows (Components, Scene, Properties) from left to right
    m_MainSplitterWidget = new QSplitter;
    m_MainSplitterWidget->addWidget(m_CompToolBox);
    m_MainSplitterWidget->addWidget(m_TabWiringWindow);
    m_MainSplitterWidget->addWidget(m_PropWin);

    // Set the Main widget as the Main window.
    setCentralWidget(m_MainSplitterWidget);

    // Set the window title
    setWindowTitle(tr("SST Workbench"));

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
    m_WiringView->setFocus();

    UserActionHandlerNewProject();
}

MainWindow::~MainWindow()
{
    if (m_PersistentSettings != NULL) {
        delete m_PersistentSettings;
    }
}

void MainWindow::SetMainTabTitle(QString Title)
{
    // Set the Tab of thw Wiring Window
    QString NewTitle = QString(MAINSCREENTITLE) + Title;
    m_TabWiringWindow->setTabText(0, NewTitle);
}

void MainWindow::SetMainTabTitle(QFileInfo FileInfo)
{
    SetMainTabTitle(FileInfo.baseName());
}

bool MainWindow::IsSceneEmpty()
{
    return m_WiringScene->items().isEmpty();
}

bool MainWindow::AreSceneComponentsSelected()
{
    // Check to see if the scene is empty
    if (IsSceneEmpty() == true) {
        return false;
    }

    // The scene is not empty, so see if there are any selected items
    QGraphicsItem* selectedItem = m_WiringScene->selectedItems().first();
    return (selectedItem != NULL);
}

void MainWindow::SetFontControls(const QFont& font)
{
    m_FontSelectCombo->setCurrentFont(font);
    m_FontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    m_BoldAction->setChecked(font.weight() == QFont::Bold);
    m_ItalicAction->setChecked(font.italic());
    m_UnderlineAction->setChecked(font.underline());
}

void MainWindow::SetSceneScaleByPercent(double ScalePercent)
{
    double NewScale;
    double NewScalePercent = ScalePercent;

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

    // Change the Percent into a double
    NewScale = NewScalePercent / 100;

    // Get the old matrix and then scale it to the new size
    QMatrix oldMatrix = m_WiringView->matrix();
    m_WiringView->resetMatrix();
    m_WiringView->translate(oldMatrix.dx(), oldMatrix.dy());
    m_WiringView->scale(NewScale, NewScale);

    // Update the Display
    UpdateSceneScaleDisplay();
}

void MainWindow::UpdateSceneScaleDisplay()
{
    qreal   NewScaleValueF;
    int     NewScaleValue;
    QString NewScaleText;
    QString CurrentText;
    int     MatchIndex;
    int     SizeCount;

    // Figure out the current scaling and make it a percentage
    NewScaleValueF = m_WiringView->transform().m11() * 100;

    // Round the number to an integer
    NewScaleValue = round(NewScaleValueF);

    // Convert it to a percent string
    NewScaleText = QString("%1%").arg(NewScaleValue);

    // Set the control
    m_SceneScaleCombo->setCurrentText(NewScaleText);

    // This is to get around an QT Issue, when the user enters text into the
    // Combobox, it is added to the list of options, We want to keep our
    // List of options fixed (not increasing), but sill allow the user to enter their own data
    // Therefore we remove the entry from the list (it is always added at the end by default).
    CurrentText = QString("%1").arg(NewScaleValue);   // FIRST TIME WITHOUT A %
    MatchIndex = m_SceneScaleCombo->findText(CurrentText);
    SizeCount = m_SceneScaleCombo->count();
    if (MatchIndex == SizeCount - 1) {
        if (CurrentText != m_LastScaleString) {
            m_SceneScaleCombo->removeItem(MatchIndex);
            m_SceneScaleCombo->setCurrentIndex(-1);
            m_SceneScaleCombo->setEditText(CurrentText);
        }
    }
    CurrentText = QString("%1%").arg(NewScaleValue);   // SECOND TIME WITH A %
    MatchIndex = m_SceneScaleCombo->findText(CurrentText);
    SizeCount = m_SceneScaleCombo->count();
    if (MatchIndex == SizeCount - 1) {
        if (CurrentText != m_LastScaleString) {
            m_SceneScaleCombo->removeItem(MatchIndex);
            m_SceneScaleCombo->setCurrentIndex(-1);
            m_SceneScaleCombo->setEditText(CurrentText);
        }
    }
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
    settings->setValue(PERSISTVALUE_SSTINFOXMLFILEPATHNAME, m_SSTInfoXMLDataFilePathName);
    settings->setValue(PERSISTVALUE_PROJECTFILEPATHNAME, m_LastSavedProjectDataFilePathName);
    settings->setValue(PERSISTVALUE_PYTHONEXPORTFILEPATHNAME, m_LastExportedPythonFilePathName);
    settings->endGroup();

    settings->beginGroup(PERSISTGROUP_PREFERENCES);
    settings->setValue(PERSISTVALUE_PREF_RETURNTOSELAFTERWIRE, m_ReturnToSelectToolAfterPlacingWire);
    settings->setValue(PERSISTVALUE_PREF_RETURNTOSELAFTERTEXT, m_ReturnToSelectToolAfterPlacingText);
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
    m_SSTInfoXMLDataFilePathName = settings->value(PERSISTVALUE_SSTINFOXMLFILEPATHNAME, QDir::homePath()).toString();
    m_LastSavedProjectDataFilePathName = settings->value(PERSISTVALUE_PROJECTFILEPATHNAME, QDir::homePath()).toString();
    m_LastExportedPythonFilePathName = settings->value(PERSISTVALUE_PYTHONEXPORTFILEPATHNAME, QDir::homePath()).toString();
    settings->endGroup();

    settings->beginGroup(PERSISTGROUP_PREFERENCES);
    m_ReturnToSelectToolAfterPlacingWire = settings->value(PERSISTVALUE_PREF_RETURNTOSELAFTERWIRE, true).toBool();
    m_ReturnToSelectToolAfterPlacingText = settings->value(PERSISTVALUE_PREF_RETURNTOSELAFTERTEXT, true).toBool();
    settings->endGroup();
}

bool MainWindow::SaveProjectData(QString ProjectFilePathName)
{
    QByteArray SaveBuffer;
    QByteArray ComputedHash;

    // Create a Data Stream and Connect it to the Buffer
    QDataStream DataStreamOut(&SaveBuffer, QIODevice::WriteOnly);

    // Save Version Informatgion
    DataStreamOut << (quint32)SSTWORKBENCHPROJECTFILEMAGICNUMBER;  // SSTWorkbench Magic Number
    DataStreamOut << (qint32)SSTWORKBENCHPROJECTFILEFORMATVERSION; // SSTWorkbench File Format Version
    DataStreamOut.setVersion(QDataStream::Qt_5_2);                 // QT's Serialization Version (for complex Types)

    // Start Streaming the data structures to the file
    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        m_CompToolBox->GetSSTInfoData()->SaveData(DataStreamOut);
        m_WiringScene->SaveData(DataStreamOut);
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

    // Set the Main Screen Tab Title
    SetMainTabTitle(QFileInfo(ProjectFilePathName));

    SetProjectDirty(false);
    return true;
}

bool MainWindow::LoadProjectData(QString ProjectFilePathName)
{
    QByteArray LoadBuffer;
    QByteArray LoadHash;
    QByteArray ComputeHash;

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
     if (FileVersion < SSTWORKBENCHPROJECTFILEFORMATVERSION) {
         QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File is too OLD; Version is %2 and Expected Version is %3").arg(ProjectFilePathName).arg(FileVersion).arg(SSTWORKBENCHPROJECTFILEFORMATVERSION));
         return false;
     }
     if (FileVersion > SSTWORKBENCHPROJECTFILEFORMATVERSION) {
         QMessageBox::critical(NULL, "Failed to Load Project File", QString("ERROR: Cannot Load Project File = %1; File is too NEW; Version is %2 and Expected Version is %3").arg(ProjectFilePathName).arg(FileVersion).arg(SSTWORKBENCHPROJECTFILEFORMATVERSION));
         return false;
     }

     // Set the Streaming Version based upon our SSTWORKBENCHFILEFORMATVERSION
     DataStreamIn.setVersion(QDataStream::Qt_5_2);

     // Create a new SSTInfoData Structure & Load the data from the file
    SSTInfoData* NewSSTInfoData = new SSTInfoData(DataStreamIn);

    // Add the new SSTInfoData to the Component Toolbax
    m_CompToolBox->LoadSSTInfo(NewSSTInfoData, false);

    // Build the Wiring Scene
    m_WiringScene->LoadData(DataStreamIn);

    // Set nothing selected
    m_WiringScene->SetNothingSelected();

    // Set the Main Screen Tab Title
    SetMainTabTitle(QFileInfo(ProjectFilePathName));

    // Display all GraphicItems
    UserActionSceneScaleZoomAll();

    SetProjectDirty(false);
    return true;
}

bool MainWindow::IsSSTInfoDataLoaded()
{
    return (m_CompToolBox->GetSSTInfoData() != NULL);
}

void MainWindow::CreateActions()
{
    ////////////////////////////////////////////////////////
    // CREATE THE LIST OF ACTIONS THAT THE  USER CAN TAKE
    ////////////////////////////////////////////////////////

    ///
    m_BringToFrontAction = new QAction(QIcon(":/images/ItemBringToFront.png"), tr("Bring to &Front"), this);
    m_BringToFrontAction->setShortcuts(QKeySequence::Forward);
    m_BringToFrontAction->setStatusTip(tr("Bring Item To Front"));
    connect(m_BringToFrontAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerBringToFront()));

    m_SendToBackAction = new QAction(QIcon(":/images/ItemSendToBack.png"), tr("Send to &Back"), this);
    m_SendToBackAction->setShortcuts(QKeySequence::Back);
    m_SendToBackAction->setStatusTip(tr("Send Item To Back"));
    connect(m_SendToBackAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerSendToBack()));

    m_DeleteAction = new QAction(QIcon(":/images/ItemDelete.png"), tr("&Delete"), this);
    m_DeleteAction->setShortcuts(QKeySequence::Delete);
    m_DeleteAction->setStatusTip(tr("Delete Item"));
    m_DeleteAction->setEnabled(false);
    connect(m_DeleteAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerDeleteItem()));

    ///
    m_UndoAction = m_UndoStack->createUndoAction(this, tr("&Undo"));
    m_UndoAction->setIcon(QIcon(":/images/Undo.png"));
    m_UndoAction->setShortcuts(QKeySequence::Undo);

    m_RedoAction = m_UndoStack->createRedoAction(this, tr("&Redo"));
    m_RedoAction->setIcon(QIcon(":/images/Redo.png"));
    m_RedoAction->setShortcuts(QKeySequence::Redo);

    m_SelectAllAction = new QAction(tr("Select &All"), this);
    m_SelectAllAction->setShortcuts(QKeySequence::SelectAll);
    m_SelectAllAction->setStatusTip(tr("Select All Items"));
    connect(m_SelectAllAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerSelectAll()));

    ///
    m_CutAction = new QAction(tr("Cut"), this);
    m_CutAction->setShortcuts(QKeySequence::Cut);
    m_CutAction->setStatusTip(tr("Cut All Selected Items"));
    connect(m_CutAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerCut()));

    m_CopyAction = new QAction(tr("Copy"), this);
    m_CopyAction->setShortcuts(QKeySequence::Copy);
    m_CopyAction->setStatusTip(tr("Copy All Selected Items"));
    connect(m_CopyAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerCopy()));

    m_PasteAction = new QAction(tr("Paste"), this);
    m_PasteAction->setShortcuts(QKeySequence::Paste);
    m_PasteAction->setStatusTip(tr("Paste Items"));
    connect(m_PasteAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerPaste()));

    ///
    m_WorkBenchNewProjectAction = new QAction(QIcon(":/images/ProjectNew.png"), tr("&New Project..."), this);
    m_WorkBenchNewProjectAction->setShortcuts(QKeySequence::New);
    m_WorkBenchNewProjectAction->setStatusTip(tr("New SST Workbench Project"));
    connect(m_WorkBenchNewProjectAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerNewProject()));

    m_WorkBenchLoadDesignAction = new QAction(QIcon(":/images/ProjectOpen.png"), tr("&Open Project..."), this);
    m_WorkBenchLoadDesignAction->setShortcuts(QKeySequence::Open);
    m_WorkBenchLoadDesignAction->setStatusTip(tr("Load SST Workbench Project"));
    connect(m_WorkBenchLoadDesignAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerLoadDesign()));

    m_WorkBenchSaveDesignAction = new QAction(QIcon(":/images/ProjectSave.png"), tr("&Save Project"), this);
    m_WorkBenchSaveDesignAction->setShortcuts(QKeySequence::Save);
    m_WorkBenchSaveDesignAction->setStatusTip(tr("Save SST Workbench Project"));
    m_WorkBenchSaveDesignAction->setEnabled(false);
    connect(m_WorkBenchSaveDesignAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerSaveDesign()));

    m_WorkBenchSaveAsAction = new QAction(QIcon(":/images/ProjectSave.png"), tr("Save Project As..."), this);
    m_WorkBenchSaveAsAction->setShortcuts(QKeySequence::SaveAs);
    m_WorkBenchSaveAsAction->setStatusTip(tr("Save SST Workbench Project As"));
    m_WorkBenchSaveAsAction->setEnabled(false);
    connect(m_WorkBenchSaveAsAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerSaveAs()));

    m_ImportSSTInfoAction = new QAction(QIcon(":/images/ImportXML.png"), tr("Im&port SSTInfo Data..."), this);
    m_ImportSSTInfoAction->setShortcut(tr("Ctrl+F"));
    m_ImportSSTInfoAction->setStatusTip(tr("Import SSTInfo Data File"));
    connect(m_ImportSSTInfoAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerImportSSTInfo()));

    m_ExportSSTInputDeckAction = new QAction(QIcon(":/images/SST_Single_S_Logo.png"), tr("E&xport SST Input Deck..."), this);
    m_ExportSSTInputDeckAction->setShortcut(tr("Ctrl+E"));
    m_ExportSSTInputDeckAction->setStatusTip(tr("Export Project To An SST Python Input Deck"));
    m_ExportSSTInputDeckAction->setEnabled(false);
    connect(m_ExportSSTInputDeckAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerExportSSTInputDeck()));

    m_ExitAction = new QAction(tr("E&xit"), this);
    m_ExitAction->setShortcuts(QKeySequence::Quit);
    m_ExitAction->setStatusTip(tr("Quit SST Workbench"));
    connect(m_ExitAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerExit()));

    m_PreferencesAction = new QAction(tr("P&references"), this);
    m_PreferencesAction->setShortcuts(QKeySequence::Preferences);
    m_PreferencesAction->setStatusTip(tr("Preferences"));
    connect(m_PreferencesAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerPreferences()));

    m_AboutAction = new QAction(tr("A&bout SST Workbench"), this);
    m_AboutAction->setStatusTip(tr("About SST Workbench"));
    connect(m_AboutAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerAbout()));

    m_PrintAction = new QAction(tr("&Print..."), this);
    m_PrintAction->setShortcuts(QKeySequence::Print);
    m_PrintAction->setStatusTip(tr("Print Current View"));
    connect(m_PrintAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerPrint()));

    ///
    m_BoldAction = new QAction(tr("Bold"), this);
    m_BoldAction->setCheckable(true);
    QPixmap pixmap(":/images/TextBold.png");
    m_BoldAction->setIcon(QIcon(pixmap));
    m_BoldAction->setShortcuts(QKeySequence::Bold);
    m_BoldAction->setStatusTip(tr("Bold"));
    connect(m_BoldAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerFontChange()));

    m_ItalicAction = new QAction(QIcon(":/images/TextItalic.png"), tr("Italic"), this);
    m_ItalicAction->setCheckable(true);
    m_ItalicAction->setShortcuts(QKeySequence::Italic);
    m_ItalicAction->setStatusTip(tr("Italic"));
    connect(m_ItalicAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerFontChange()));

    m_UnderlineAction = new QAction(QIcon(":/images/TextUnderline.png"), tr("Underline"), this);
    m_UnderlineAction->setCheckable(true);
    m_UnderlineAction->setShortcuts(QKeySequence::Underline);
    m_UnderlineAction->setStatusTip(tr("Underline"));
    connect(m_UnderlineAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerFontChange()));

    ///
    // Build the Component Menu Actions
    m_MovePortsAction = new QAction("&Move Port Positions", this);
    m_MovePortsAction->setShortcut(tr("Ctrl+M"));
    m_MovePortsAction->setStatusTip(tr("Move Component Ports"));
    connect(m_MovePortsAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerMovePorts()));

    m_SetDynamicPortsAction = new QAction("Set Dynamic Por&ts...", this);
    m_SetDynamicPortsAction->setShortcut(tr("Ctrl+T"));
    m_SetDynamicPortsAction->setStatusTip(tr("Set Component Dynamic Ports"));
    m_SetDynamicPortsAction->setVisible(false);
    connect(m_SetDynamicPortsAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerSetDynamicPorts()));

    ///
    // Actions for displaying the Toolbars
    m_ShowToolsToolbarAction = new QAction("Tools", this);
    m_ShowToolsToolbarAction->setStatusTip(tr("Enable/Disable Tools Toolbar"));
    m_ShowToolsToolbarAction->setCheckable(true);
    m_ShowToolsToolbarAction->setChecked(true);
    connect(m_ShowToolsToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    m_ShowItemToolbarAction = new QAction("Item Control", this);
    m_ShowItemToolbarAction->setStatusTip(tr("Enable/Disable Item Control Toolbar"));
    m_ShowItemToolbarAction->setCheckable(true);
    m_ShowItemToolbarAction->setChecked(true);
    connect(m_ShowItemToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    m_ShowFontControlToolbarAction = new QAction("Font Control", this);
    m_ShowFontControlToolbarAction->setStatusTip(tr("Enable/Disable Font Control Toolbar"));
    m_ShowFontControlToolbarAction->setCheckable(true);
    m_ShowFontControlToolbarAction->setChecked(true);
    connect(m_ShowFontControlToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    m_ShowViewControlToolbarAction = new QAction("View Control", this);
    m_ShowViewControlToolbarAction->setStatusTip(tr("Enable/Disable View Control Toolbar"));
    m_ShowViewControlToolbarAction->setCheckable(true);
    m_ShowViewControlToolbarAction->setChecked(true);
    connect(m_ShowViewControlToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    m_ShowFileActionsToolbarAction = new QAction("File Actions", this);
    m_ShowFileActionsToolbarAction->setStatusTip(tr("Enable/Disable File Actions Toolbar"));
    m_ShowFileActionsToolbarAction->setCheckable(true);
    m_ShowFileActionsToolbarAction->setChecked(true);
    connect(m_ShowFileActionsToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    m_ShowEditToolbarAction = new QAction("Edit", this);
    m_ShowEditToolbarAction->setStatusTip(tr("Enable/Disable Edit Toolbar"));
    m_ShowEditToolbarAction->setCheckable(true);
    m_ShowEditToolbarAction->setChecked(true);
    connect(m_ShowEditToolbarAction, SIGNAL(triggered()), this, SLOT(UserActionHandlerShowToolbars()));

    ///
    // Zoom In
    m_SceneScaleZoomInAction = new QAction(QIcon(":/images/ZoomIn.png"), tr("Zoom In"), this);
    m_SceneScaleZoomInAction->setShortcuts(QKeySequence::ZoomIn);
    m_SceneScaleZoomInAction->setToolTip(tr("Zoom In"));
    m_SceneScaleZoomInAction->setStatusTip(tr("Zoom In"));
    connect(m_SceneScaleZoomInAction, SIGNAL(triggered()), this, SLOT(UserActionSceneScaleZoomIn()));

    // Zoom Out
    m_SceneScaleZoomOutAction = new QAction(QIcon(":/images/ZoomOut.png"), tr("Zoom Out"), this);
    m_SceneScaleZoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    m_SceneScaleZoomOutAction->setToolTip(tr("Zoom Out"));
    m_SceneScaleZoomOutAction->setStatusTip(tr("Zoom Out"));
    connect(m_SceneScaleZoomOutAction, SIGNAL(triggered()), this, SLOT(UserActionSceneScaleZoomOut()));

    // Zoom All
    m_SceneScaleZoomAllAction = new QAction(QIcon(":/images/ZoomAll.png"), tr("Zoom Al&l"), this);
    m_SceneScaleZoomAllAction->setShortcut(tr("Ctrl+L"));
    m_SceneScaleZoomAllAction->setToolTip(tr("Zoom All"));
    m_SceneScaleZoomAllAction->setStatusTip(tr("Zoom All"));
    connect(m_SceneScaleZoomAllAction, SIGNAL(triggered()), this, SLOT(UserActionSceneScaleZoomAll()));
}

void MainWindow::CreateMenus()
{
    //////////////////////////////////////////
    // BUILD THE MENU AND TIE TO THE ACTIONS
    //////////////////////////////////////////

    // File Menu
    m_FileMenu = menuBar()->addMenu(tr("&File"));
    m_FileMenu->addAction(m_WorkBenchNewProjectAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_WorkBenchLoadDesignAction);
    m_FileMenu->addAction(m_WorkBenchSaveDesignAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_WorkBenchSaveAsAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_ImportSSTInfoAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_ExportSSTInputDeckAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_PrintAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_PreferencesAction);
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(m_ExitAction);


    // Edit Menu
    m_EditMenu = menuBar()->addMenu(tr("&Edit"));
    m_EditMenu->addAction(m_UndoAction);
    m_EditMenu->addAction(m_RedoAction);
    m_EditMenu->addSeparator();
    m_EditMenu->addAction(m_SelectAllAction);
    m_EditMenu->addSeparator();
    m_EditMenu->addAction(m_CutAction);
    m_EditMenu->addAction(m_CopyAction);
    m_EditMenu->addAction(m_PasteAction);

    // Item Menu
    m_GenericItemMenu = menuBar()->addMenu(tr("&Item"));
    m_GenericItemMenu->addAction(m_DeleteAction);
    m_GenericItemMenu->addSeparator();
    m_GenericItemMenu->addAction(m_BringToFrontAction);
    m_GenericItemMenu->addAction(m_SendToBackAction);
    m_GenericItemMenu->addSeparator();
    m_GenericItemMenu->addAction(m_MovePortsAction);
    m_GenericItemMenu->addSeparator();
    m_GenericItemMenu->addAction(m_SetDynamicPortsAction);

    // View Menu
    m_ViewMenu = menuBar()->addMenu(tr("&View"));
    m_ViewMenu->addAction(m_SceneScaleZoomAllAction);
    m_ViewMenu->addSeparator();
    m_ViewMenu->addAction(m_SceneScaleZoomInAction);
    m_ViewMenu->addAction(m_SceneScaleZoomOutAction);

    // Toolbar Menu
    m_ToolbarMenu = menuBar()->addMenu(tr("&Toolbars"));
    m_ToolbarMenu->addAction(m_ShowFileActionsToolbarAction);
    m_ToolbarMenu->addAction(m_ShowEditToolbarAction);
    m_ToolbarMenu->addAction(m_ShowToolsToolbarAction);
    m_ToolbarMenu->addAction(m_ShowItemToolbarAction);
    m_ToolbarMenu->addAction(m_ShowFontControlToolbarAction);
    m_ToolbarMenu->addAction(m_ShowViewControlToolbarAction);

    // Help Menu
    m_AboutMenu = menuBar()->addMenu(tr("&Help"));
    m_AboutMenu->addAction(m_AboutAction);
}

void MainWindow::CreateToolbars()
{
    //////////////////////////////////////////
    // BUILD THE CONTROLS USED BY THE TOOLBARS
    // AND TIE THE BUTTONS TO THE ACTIONS
    //////////////////////////////////////////

    // Build the Font Selector Control
    m_FontSelectCombo = new QFontComboBox();
    m_FontSelectCombo->setToolTip(tr("Select Text Font"));
    m_FontSelectCombo->setStatusTip(tr("Select Text Font"));
    connect(m_FontSelectCombo, SIGNAL(activated(QString)), this, SLOT(UserActionFontSelectChanged(QString)));

    // Build the Font Size Control
    m_FontSizeCombo = new QComboBox;
    m_FontSizeCombo->setEditable(true);
    m_FontSizeCombo->setToolTip(tr("Set Text Size"));
    m_FontSizeCombo->setStatusTip(tr("Set Text Size"));
    for (int i = 4; i <= 32; i = i + 2) {
        m_FontSizeCombo->addItem(QString().setNum(i));
    }
    m_LastFontSizeString = m_FontSizeCombo->itemText(m_FontSizeCombo->count() - 1);
    QIntValidator* FontValidator = new QIntValidator(2, 64, this);  // Allow user to enter different number not on the select list
    m_FontSizeCombo->setValidator(FontValidator);
    m_FontSizeCombo->setInsertPolicy(QComboBox::InsertAtBottom);
    connect(m_FontSizeCombo, SIGNAL(activated(QString)), this, SLOT(UserActionFontSizeChanged(QString)));

    // Font Color Button
    m_FontColorToolButton = new QToolButton;
    m_FontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_FontColorToolButton->setMenu(CreateColorMenu(SLOT(UserActionTextColorChanged()), Qt::black));
    m_FontColorToolButton->setToolTip(tr("Set Text Color"));
    m_FontColorToolButton->setStatusTip(tr("Set Text Color"));
    m_TextColorAction = m_FontColorToolButton->menu()->defaultAction();
    m_FontColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/PointerText.png", Qt::black));
    // m_FontColorToolButton->setAutoFillBackground(true);
    connect(m_FontColorToolButton, SIGNAL(clicked()), this, SLOT(UserActionTextColorButtonTriggered()));

    // ComponentFill Color Button
    m_ComponentFillColorToolButton = new QToolButton;
    m_ComponentFillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_ComponentFillColorToolButton->setMenu(CreateColorMenu(SLOT(UserActionComponentFillColorChanged()), Qt::white, true));
    m_ComponentFillColorToolButton->setToolTip(tr("Set Component Color"));
    m_ComponentFillColorToolButton->setStatusTip(tr("Set Component Color"));
    m_ComponentFillColorAction = m_ComponentFillColorToolButton->menu()->defaultAction();
    m_ComponentFillColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/ItemFloodfill.png", Qt::white));
    connect(m_ComponentFillColorToolButton, SIGNAL(clicked()), this, SLOT(UserActionComponentFillColorButtonTriggered()));

    // Pointer SelectMove Button
    QToolButton* PointerSelectMoveButton = new QToolButton;
    PointerSelectMoveButton->setToolTip(tr("Select / Move Item"));
    PointerSelectMoveButton->setStatusTip(tr("Select / Move Item"));
    PointerSelectMoveButton->setCheckable(true);
    PointerSelectMoveButton->setChecked(true);
    PointerSelectMoveButton->setIcon(QIcon(":/images/PointerSelectMove.png"));

    // Pointer Add a Wire Button
    QToolButton* PointerAddWireButton = new QToolButton;
    PointerAddWireButton->setToolTip(tr("Add A Wire"));
    PointerAddWireButton->setStatusTip(tr("Add A Wire"));
    PointerAddWireButton->setCheckable(true);
    PointerAddWireButton->setIcon(QIcon(":/images/PointerLine.png"));

    // Pointer Add Text Button
    QToolButton* PointerAddTextButton = new QToolButton;
    PointerAddTextButton->setToolTip(tr("Add Text"));
    PointerAddTextButton->setStatusTip(tr("Add Text"));
    PointerAddTextButton->setCheckable(true);
    PointerAddTextButton->setIcon(QIcon(":/images/PointerText.png"));

    // Create a group for the pointer and line buttons (makes them mutually exclusive)
    m_PointerTypeGroup = new QButtonGroup(this);
    m_PointerTypeGroup->setExclusive(false);  // Allow buttons to not be mutually exclusive, mut. exclusion Done in handlers
    m_PointerTypeGroup->addButton(PointerSelectMoveButton, int(WiringScene::MODE_SELECTMOVEITEM));
    m_PointerTypeGroup->addButton(PointerAddWireButton, int(WiringScene::MODE_ADDWIRE));
    m_PointerTypeGroup->addButton(PointerAddTextButton, int(WiringScene::MODE_ADDTEXT));
    // If a buton of this group is clicked, then call the function
    connect(m_PointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(UserActionPointerGroupClicked(int)));

    // The Scaling combo box for the Scene scale
    m_SceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("25%") << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%") << tr("200%");
    m_SceneScaleCombo->addItems(scales);
    m_LastScaleString = m_SceneScaleCombo->itemText(m_SceneScaleCombo->count() - 1);
    m_SceneScaleCombo->setEditable(true);
    m_SceneScaleCombo->setCurrentIndex(3);
    m_SceneScaleCombo->setToolTip(tr("Set Scale"));
    m_SceneScaleCombo->setStatusTip(tr("Set Scale"));
    // Setup a validator to allow user to enter different number not on the select list
    QRegExp re("[1-5]{1,1}[0-9]{0,2}%{0,1}");  // First digit must be 1-5 and is required, followed by 0 or more digits of 0-9 followed by an optional %
    QRegExpValidator* ScaleValidator = new QRegExpValidator(re, this);  // Allow user to enter different number not on the select list
    m_SceneScaleCombo->setValidator(ScaleValidator);
    m_SceneScaleCombo->setInsertPolicy(QComboBox::InsertAtBottom);
    connect(m_SceneScaleCombo, SIGNAL(activated(QString)), this, SLOT(UserActionSceneScaleChanged(QString)));

    /////////////////////////
    // BUILD THE TOOLBARS
    /////////////////////////

    // File Actions
    m_FileActionsToolbar = addToolBar(tr("File Actions"));
    m_FileActionsToolbar->setObjectName("File Actions");
    m_FileActionsToolbar->addAction(m_WorkBenchLoadDesignAction);
    m_FileActionsToolbar->addAction(m_WorkBenchSaveDesignAction);
    m_FileActionsToolbar->addAction(m_ImportSSTInfoAction);
    m_FileActionsToolbar->addAction(m_ExportSSTInputDeckAction);

    // Edit Toolbar
    m_EditToolbar = addToolBar(tr("Edit"));
    m_EditToolbar->setObjectName("Edit");
    m_EditToolbar->addAction(m_UndoAction);
    m_EditToolbar->addAction(m_RedoAction);

    // Tools Toolbar
    m_ToolsToolbar = addToolBar(tr("Tools"));
    m_ToolsToolbar->setObjectName("Tools");
    m_ToolsToolbar->addWidget(PointerSelectMoveButton);
    m_ToolsToolbar->addWidget(PointerAddWireButton);
    m_ToolsToolbar->addWidget(PointerAddTextButton);

    // Edit Toolbar
    m_ItemToolBar = addToolBar(tr("Item"));
    m_ItemToolBar->setObjectName("Item");
    m_ItemToolBar->addAction(m_DeleteAction);
    m_ItemToolBar->addAction(m_BringToFrontAction);
    m_ItemToolBar->addAction(m_SendToBackAction);
    m_ItemToolBar->addWidget(m_ComponentFillColorToolButton);

    // Text ToolBar
    m_FontControlToolBar = addToolBar(tr("Font Control"));
    m_FontControlToolBar->setObjectName("Font Control");
    m_FontControlToolBar->addWidget(m_FontSelectCombo);
    m_FontControlToolBar->addWidget(m_FontSizeCombo);
    m_FontControlToolBar->addAction(m_BoldAction);
    m_FontControlToolBar->addAction(m_ItalicAction);
    m_FontControlToolBar->addAction(m_UnderlineAction);
    m_FontControlToolBar->addWidget(m_FontColorToolButton);

    // View Control Toolbar
    m_ViewControlToolbar = addToolBar(tr("View Control"));
    m_ViewControlToolbar->setObjectName("View Control");
    m_ViewControlToolbar->addWidget(m_SceneScaleCombo);
    m_ViewControlToolbar->addAction(m_SceneScaleZoomAllAction);
    m_FileActionsToolbar->addSeparator();
    m_ViewControlToolbar->addAction(m_SceneScaleZoomInAction);
    m_ViewControlToolbar->addAction(m_SceneScaleZoomOutAction);

    // Turn off the right click on the toolbar area to prevent the toolbar menu from popping up
    setContextMenuPolicy(Qt::NoContextMenu);
}

void MainWindow::CreateStatusbar()
{
    // Create/Display the Status Bar
  statusBar()->showMessage(tr(""));
}

QIcon MainWindow::CreateColorIcon(QColor color)
{
    // Create the Pixmap for the icon
    QPixmap pixmap(20, 20);

    // Create a painter for the Pixmap
    QPainter painter(&pixmap);

    // Draw an outside box
    painter.setPen(QPen());
    painter.drawRect(0, 0, 20, 20);

    // Now Fill it
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(1, 1, 18, 18), color);

    // Return the icon
    return QIcon(pixmap);
}

QMenu* MainWindow::CreateColorMenu(const char* slot, QColor defaultColor, bool ComponentColors /*=false*/)
{
    // Build list of Colors and a list of their names
    QList<QColor> colors;
    QStringList   colorNames;
    if (ComponentColors == true) {
        // Colors for Components
        colors << Qt::green << Qt::red << Qt::cyan << Qt::yellow  << Qt::gray << Qt::white;
        colorNames << tr("green") << tr("red") << tr("cyan") << tr("yellow") << tr("gray") << tr("white");
    } else {
        // Colors for Text
        colors << Qt::black << Qt::green << Qt::red << Qt::blue;
        colorNames << tr("black") << tr("green") << tr("red") << tr("blue");
    }

    // Now Build the Color Menu
    QMenu* colorMenu = new QMenu(this);

    // For each color set it up
    for (int i = 0; i < colors.count(); ++i) {
        QAction* action = new QAction(colorNames.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(CreateColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        // If this color is the default color, make it the default action
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}

QIcon MainWindow::CreateColorToolButtonIcon(const QString& imageFile, QColor color)
{
    // This builds the small color icon under the color picker toolbar buttons
    QPixmap pixmap(50, 80);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QPixmap image(imageFile);

    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
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

void MainWindow::UserActionComponentToolboxButtonPressed(SSTInfoDataComponent* ptrComponent)
{
    // THIS HANDLER IS CALLED WHEN A BUTTON IN THE COMPONENT TOOLBOX (LEFT WINDOW) IS PRESSED DOWN

    // Disable Moving Ports
    EnableMovingPorts(false);

    if (ptrComponent != NULL) {
        // Set the WiringScene Operation Mode to Add Components
        m_WiringScene->SetOperationMode(WiringScene::OperationMode(WiringScene::MODE_ADDCOMPONENT));

        // Now Set the component from SSTInfo into the Scene to use when the user places it
        m_WiringScene->SetUserChosenSSTInfoDataComponent(ptrComponent);

        // Since we are in MODE_ADDCOMPONENT, turn off the ANY of the other Pointer
        // Toolbar Buttons (MODE_ADDWIRE, MODE_ADDTEXT, or MODE_SELECTMOVEITEM)
        QList<QAbstractButton*> PointerToolbarButtons = m_PointerTypeGroup->buttons();

        // Turn off the checked setting for all other buttons
        foreach (QAbstractButton* button, PointerToolbarButtons) {
            button->setChecked(false);
        }
    } else {
        // The ComponentToolbox is changed and we need to do nothing if the scene is clicked
        m_WiringScene->SetOperationMode(WiringScene::OperationMode(WiringScene::MODE_DONOTHING));
    }
}

void MainWindow::UserActionPointerGroupClicked(int id)
{
    // THIS HANDLER IS CALLED WHEN ONE OF THE 3 BUTTONS OF THE
    // ACTION POINTER GROUP (TOOBAR) IS CLICKED

    // Disable Moving Ports
    EnableMovingPorts(false);

    // Turn off any checked ToolBox component
    m_CompToolBox->UncheckAllCurrentGroupButtons();

    // Get the id of the button we press (this will be one of the WiringScene Modes)
    // MODE_ADDWIRE, MODE_ADDTEXT, or MODE_SELECTMOVEITEM
    int NewMode = id;

    // Set the WiringScene Operation Mode to the new mode
    m_WiringScene->SetOperationMode(WiringScene::OperationMode(NewMode));

    // Get a pointer to the list of the toolbar buttons
    QList<QAbstractButton*> PointerToolbarButtons = m_PointerTypeGroup->buttons();

    // Make sure the buttons are Mutually exclusive
    // Turn off the checked setting for all other buttons, but make sure the selected button is checked
    foreach (QAbstractButton* button, PointerToolbarButtons) {
        if (m_PointerTypeGroup->button(id) != button) {
            button->setChecked(false);
        } else {
            button->setChecked(true);
        }
    }
}

void MainWindow::HandleSceneEventComponentAdded(GraphicItemComponent* Item)
{
    // Handler called when a Component Item is added to the scene
    Q_UNUSED(Item)

    // Disable Moving Ports
    EnableMovingPorts(false);
}

void MainWindow::HandleSceneEventTextAdded(GraphicItemText* Item)
{
    // Handler called when a Text Item is added to the scene
    Q_UNUSED(Item)

    // Disable Moving Ports
    EnableMovingPorts(false);

    // Check to see if we need to return to the Select Tool after adding the text
    if (m_ReturnToSelectToolAfterPlacingText == true) {
        emit m_PointerTypeGroup->buttonClicked(int(WiringScene::MODE_SELECTMOVEITEM));
    }

}

void MainWindow::HandleSceneEventWireAddedInitialPlacement(GraphicItemWire* Item)
{
    // Handler called when a Wire Item is added to the scene
    // and the start point has been set
    Q_UNUSED(Item)

    // Disable Moving Ports
    EnableMovingPorts(false);
}

void MainWindow::HandleSceneEventWireAddedFinalPlacement(GraphicItemWire* Item)
{
    // Handler called when a Wire Item is added to the scene
    // and the end point has been set
    Q_UNUSED(Item)

    // Check to see if we need to return to the Select Tool after adding the wire
    if (m_ReturnToSelectToolAfterPlacingWire == true) {
        emit m_PointerTypeGroup->buttonClicked(int(WiringScene::MODE_SELECTMOVEITEM));
    }
}

void MainWindow::HandleSceneEventSelectionChanged()
{
    // Enable/Disable the Delete Actions (Menu and Toolbar)
    m_DeleteAction->setEnabled(m_WiringScene->selectedItems().count() > 0);
}

void MainWindow::HandleSceneEventGraphicItemSelected(QGraphicsItem* Item)
{
    // NOTE: Item may be NULL, to represent that no item is selected

    // The scene has some item selected, we first just want to clear the properties window
    m_PropWin->ClearProperiesWindow();

    // Turn off component menu items initially
    m_MovePortsAction->setVisible(false);
    m_SetDynamicPortsAction->setVisible(false);
    m_SelectedComponent = NULL;
    m_SelectedText = NULL;

    // Decide if an Item was selected (Item is not NULL)
    if (Item != NULL) {
        // Is the Item a GraphicItemComponent?  (If it is not, then SelectedComponent is set to NULL)
        m_SelectedComponent = qgraphicsitem_cast<GraphicItemComponent*>(Item);
        if (m_SelectedComponent != NULL) {
            m_MovePortsAction->setVisible((m_SelectedComponent->GetNumGraphicalPortsOnComponent() > 0) && (IsMovingPortsEnabled() == false));
            m_SetDynamicPortsAction->setVisible(m_SelectedComponent->ComponentContainsDynamicPorts());
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
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Refresh the Properties in the properties window
    m_PropWin->RefreshProperiesWindowProperty(PropertyName, NewPropertyValue);
}

void MainWindow::HandleSceneEventRefreshPropertiesWindow(ItemProperties* Properties)
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Refresh all the Properties in the properties window
    m_PropWin->SetGraphicItemProperties(Properties);
}

void MainWindow::HandleSceneEventDragAndDropFinished()
{
    // The scene just finished a Drag & Drop event
    // Lets automatically choose the Select/Move tool by "Pretend" clicking on it.
    // This will also automatically uncheck the Component in the Toolbox.
    // to prevent "accidental" placement of more components
    emit m_PointerTypeGroup->buttonClicked(int(WiringScene::MODE_SELECTMOVEITEM));
}

void MainWindow::HandleUndoStackCleanChanged(bool NewState)
{
    Q_UNUSED(NewState)

    UpdateDisplayBasedUponDirtyStatus();
}

void MainWindow::UserActionHandlerBringToFront()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Check to see if the scene is empty
    if (IsSceneEmpty() == true) {
        return;
    }

    // Get the first selected item
    QGraphicsItem* selectedItem = m_WiringScene->selectedItems().first();

    // Get the items colliding with the selected item
    QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

    // Figure out the largest zvalue we need to change the selected item
    qreal zValue = 0;
    foreach (QGraphicsItem* item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == GraphicItemComponent::Type) {
            zValue = item->zValue() + 0.1;
        }
    }

    // Set the new zvalue for the selected item
    selectedItem->setZValue(zValue);
    SetProjectDirty();
}

void MainWindow::UserActionHandlerSendToBack()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Check to see if the scene is empty
    if (IsSceneEmpty() == true) {
        return;
    }

    // Get the first selected item
    QGraphicsItem* selectedItem = m_WiringScene->selectedItems().first();

    // Get the items colliding with the selected item
    QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

    // Figure out the largest zvalue we need to change the selected item
    qreal zValue = 0;
    foreach (QGraphicsItem* item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == GraphicItemComponent::Type) {
            zValue = item->zValue() - 0.1;
        }
    }

    // Set the new zvalue for the selected item
    selectedItem->setZValue(zValue);
    SetProjectDirty();
}

void MainWindow::UserActionHandlerDeleteItem()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    m_WiringScene->DeleteAllSelectedItems();

    // Set the selected objects to NULL
    m_SelectedComponent = NULL;
    m_SelectedText = NULL;
}

void MainWindow::UserActionHandlerSelectAll()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Select all Graphic items
    foreach (QGraphicsItem* item, m_WiringScene->items(Qt::DescendingOrder)) {
        item->setSelected(true);
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

void MainWindow::UserActionHandlerCut()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Copy all the objects
    UserActionHandlerCopy();

    // And delete all the selected objects
    m_WiringScene->DeleteAllSelectedItems();
}

void MainWindow::UserActionHandlerCopy()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    GraphicItemBase*            BaseItem = NULL;
    QByteArray*                 ItemBuffer;
    QDataStream*                DataStream;
    GraphicItemComponent*       Component;
    GraphicItemWire*            Wire;
    GraphicItemWireLineSegment* WireSeg;
    GraphicItemText*            Text;

    ClearCopyPasteBuffer();

    foreach (QGraphicsItem* SelectedItem, m_WiringScene->selectedItems()) {
        BaseItem = NULL;
        Wire = NULL;
        WireSeg = NULL;

        // Convert the SelectedItem into one of the 3 types of GraphicItemXXX classes
        // Note: The GraphicItemXXX variable will be null if type is incorrect
        if (BaseItem == NULL) {
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
                Wire = (GraphicItemWire*)WireSeg->parentItem();
                BaseItem = (GraphicItemBase*)Wire;
            }
        }
        if (BaseItem == NULL) {
            Text = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
            BaseItem = qgraphicsitem_cast<GraphicItemText*>(SelectedItem);
        }

        if (BaseItem != NULL) {
            // Create a Binary Data Array
            ItemBuffer = new QByteArray();

            // Build a DataStream on top of the ByteArray
            DataStream = new QDataStream(ItemBuffer, QIODevice::WriteOnly);

            // Now have the Item Save itself to the ByteArray (via the Datastream)
            BaseItem->SaveData(*DataStream);

            // Delete the datastream as it is no longer needed
            delete DataStream;

            if (BaseItem->GetItemType() == GraphicItemBase::ITEMTYPE_COMPONENT) {
                m_CopyPasteBufferListComps.append(ItemBuffer);
            }
            if (BaseItem->GetItemType() == GraphicItemBase::ITEMTYPE_WIRE) {
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
            if (BaseItem->GetItemType() == GraphicItemBase::ITEMTYPE_TEXT) {
                m_CopyPasteBufferListTexts.append(ItemBuffer);
            }
        }
    }
}

void MainWindow::UserActionHandlerPaste()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    int                   x;
    int                   count;
    QByteArray*           ItemBuffer;
    QDataStream*          DataStream;

    // Select Nothing
    m_WiringScene->SetNothingSelected();

    // Add Paste all Component Items
    count = m_CopyPasteBufferListComps.count();
    for (x = 0; x < count; x++) {

        // Create a Binary Data Array
        ItemBuffer = new QByteArray();

        // Build a DataStream on top of the ByteArray
        DataStream = new QDataStream(m_CopyPasteBufferListComps.at(x), QIODevice::ReadOnly);

        // Tell the Scene to Paste the Item
        m_WiringScene->PasteNewComponentItem(*DataStream, m_PasteOffset);

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
        m_WiringScene->PasteNewWireItem(*DataStream, m_PasteOffset);

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
        m_WiringScene->PasteNewTextItem(*DataStream, m_PasteOffset);

        // Delete the datastream as it is no longer needed
        delete DataStream;
    }

    // Increase the Paste Offset
    m_PasteOffset += DEFAULT_PASTE_OFFSET;
}

void MainWindow::UserActionHandlerNewProject()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Check to see if the existing project is dirty before loading a new one
    if (IsProjectDirty() == true) {
        QString errorString = "Changes have been made to the project\n Are you sure you want to loose your changes and create a new project?";
        if (QMessageBox::question(NULL, "Loose Changes?", errorString) == QMessageBox::No) {
            return;
        }
    }

    // Select and delete all objects
    UserActionHandlerSelectAll();
    UserActionHandlerDeleteItem();

    // Do any startup work for the new project

    SetProjectDirty(false);
    m_ExportSSTInputDeckAction->setEnabled(true);
    m_WorkBenchSaveAsAction->setEnabled(false);
}

void MainWindow::UserActionHandlerLoadDesign()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

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
            m_ExportSSTInputDeckAction->setEnabled(true);
            m_WorkBenchSaveAsAction->setEnabled(true);
            m_LoadedProjectDataFilePathName = ProjectFilePathName;
            m_LastSavedProjectDataFilePathName = ProjectFilePathName;
        } else {
            QString errorString = QString("ERROR: SSTWorkbench Cannot Load Project File %1").arg(ProjectFilePathName);
            QMessageBox::critical(NULL, "Cannot Load Project File", errorString);
            return;
        }
    }
}

void MainWindow::UserActionHandlerSaveDesign()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // See if the user has set a Project Save Filename
    if (m_LoadedProjectDataFilePathName.isEmpty() == true) {
        UserActionHandlerSaveAs();
    } else {
        // User wants to save
        if (SaveProjectData(m_LoadedProjectDataFilePathName) != true) {
            QString errorString = QString("ERROR: SSTWorkbench Cannot Save Project File %1").arg(m_LoadedProjectDataFilePathName);
            QMessageBox::critical(NULL, "Cannot Save Project File", errorString);
        }
    }
}

void MainWindow::UserActionHandlerSaveAs()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

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

void MainWindow::UserActionHandlerImportSSTInfo()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

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
    TempPath = QFileInfo(m_SSTInfoXMLDataFilePathName).dir().absolutePath();
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
            m_ExportSSTInputDeckAction->setEnabled(true);
            m_WorkBenchSaveAsAction->setEnabled(true);

            // Save the File Name and Path settings
            m_SSTInfoXMLDataFilePathName = XMLFileName;
        }
    }
}

void MainWindow::UserActionHandlerExportSSTInputDeck()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

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
        PythonExporter PythonExport(m_WiringScene, ProjectFilePathName);

        // User selected a file, so now save the project data to it
        PythonExport.PerformExportOfPythonFile();

        // Save the File Name and Path settings
        m_LastExportedPythonFilePathName = ProjectFilePathName;
    }
}

void MainWindow::UserActionHandlerPreferences()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    int nRtn;

   // Create the Dialog
    m_PreferencesDialog = new DialogPreferences(this);
    m_PreferencesDialog->SetFlagReturnToSelectAfterWirePlaced(m_ReturnToSelectToolAfterPlacingWire);
    m_PreferencesDialog->SetFlagReturnToSelectAfterTextPlaced(m_ReturnToSelectToolAfterPlacingText);

    // Run the dialog box (Modal)
    nRtn = m_PreferencesDialog->exec();

    if (nRtn == QDialog::Accepted) {
        m_ReturnToSelectToolAfterPlacingWire = m_PreferencesDialog->GetFlagReturnToSelectAfterWirePlaced();
        m_ReturnToSelectToolAfterPlacingText = m_PreferencesDialog->GetFlagReturnToSelectAfterTextPlaced();
    }

    // Delete the Dialog
    delete m_PreferencesDialog;
}

void MainWindow::UserActionHandlerAbout()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    QString FinalAboutText;
    QString AboutText = "";
    QString SSTVersion = "";

    AboutText += "<center><h2>SST Workbench</h2>Version %1<br><br>";

    AboutText += "Copyright (c) 2009-2014<br>Sandia Corporation.  ";
    AboutText += "Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, ";
    AboutText += "the U.S. Government retains certain rights in this software.<br><br>";
    AboutText += "Copyright (c) 2009-2014,<br>Sandia Corporation<br>All rights reserved.<br><br>";

    AboutText += "<img src=:/images/SSTLogo.png><br>";

    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        AboutText += "SST Version %2<br>";
        SSTVersion = m_CompToolBox->GetSSTInfoData()->GetSSTInfoVersion();
    }

    AboutText += "<br>This Application is part of the SST Software package. ";
    AboutText += "For license information, see the LICENSE file in the top level directory of the distribution.</center> ";

    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        FinalAboutText = AboutText.arg(COREAPP_VERSION).arg(SSTVersion);
    } else {
        FinalAboutText = AboutText.arg(COREAPP_VERSION);
    }

    QMessageBox::about(this, tr("About SST Workbench"), FinalAboutText);
}

void MainWindow::UserActionHandlerPrint()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    QPrinter printer;
    QString  DocName;

    //  Ask the user what printer to go to
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        // Setup the Document Name

        // Build the painter canvas using the printer info
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);

        // Render the current view to the Painter
        m_WiringView->render(&painter);
    }
}

void MainWindow::UserActionHandlerExit()
{
    close();
}

void MainWindow::UserActionHandlerShowToolbars()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Enable / Disable the appropriate Toolbar based upon the menu settings
    m_FileActionsToolbar->setVisible(m_ShowFileActionsToolbarAction->isChecked());
    m_EditToolbar->setVisible(m_ShowEditToolbarAction->isChecked());
    m_ToolsToolbar->setVisible(m_ShowToolsToolbarAction->isChecked());
    m_ItemToolBar->setVisible(m_ShowItemToolbarAction->isChecked());
    m_FontControlToolBar->setVisible(m_ShowFontControlToolbarAction->isChecked());
    m_ViewControlToolbar->setVisible(m_ShowViewControlToolbarAction->isChecked());
}

void MainWindow::UserActionFontSelectChanged(const QString&)
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    UserActionHandlerFontChange();
}

void MainWindow::UserActionFontSizeChanged(const QString&)
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    UserActionHandlerFontChange();
}

void MainWindow::UserActionHandlerFontChange()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Build a font based on the user controls
    QFont font = m_FontSelectCombo->currentFont();
    font.setPointSize(m_FontSizeCombo->currentText().toInt());
    font.setWeight(m_BoldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(m_ItalicAction->isChecked());
    font.setUnderline(m_UnderlineAction->isChecked());

    // Tell the scene to change its font
    m_WiringScene->SetGraphicItemTextFont(font);

    // If there is no selected text, update the default font
    if (m_SelectedText == NULL) {
        m_DefaultFont = font;
    }

    // This is to get around an QT Issue, when the user enters text into the
    // Combobox, it is added to the list of options, We want to keep our
    // List of options fixed (not increasing), but sill allow the user to enter their own data
    // Therefore we remove the entry from the list (it is always added at the end by default).
    QString CurrentText = m_FontSizeCombo->currentText();
    int MatchIndex = m_FontSizeCombo->findText(CurrentText);
    int SizeCount = m_FontSizeCombo->count();
    if (MatchIndex == SizeCount - 1) {
        if (CurrentText != m_LastFontSizeString) {
            m_FontSizeCombo->removeItem(MatchIndex);
            m_FontSizeCombo->setCurrentIndex(-1);
            m_FontSizeCombo->setEditText(CurrentText);
        }
    }
}

void MainWindow::UserActionSceneScaleChanged(const QString& NewScaleText)
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = NewScaleText;

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Set the New Scale
    SetSceneScaleByPercent(NewScalePercent);
}

void MainWindow::UserActionSceneScaleZoomIn()
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = m_SceneScaleCombo->currentText();

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Change the Zoom by a 25% change
    NewScalePercent *= 1.25;            // Increase by 25%
    //NewScalePercent += ZOOM_STEP_SIZE;  // Increase by a fixed step size

    // Set the New Scale
    SetSceneScaleByPercent(NewScalePercent);
}

void MainWindow::UserActionSceneScaleZoomOut()
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = m_SceneScaleCombo->currentText();

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Change the Zoom by a 25% change
    NewScalePercent /= 1.25;            // Decrease by 25%
    //NewScalePercent -= ZOOM_STEP_SIZE;  // Decrease by a fixed step size

    // Set the New Scale
    SetSceneScaleByPercent(NewScalePercent);
}

void MainWindow::UserActionSceneScaleZoomAll()
{
    QString CurrentScaleText;
    double  NewScalePercent;
    QRectF  ItemsBoundingRect;

    // See if there is anything in the Scene
    if (IsSceneEmpty() == true) {
        // EMPTY SCENE
        // Get the Current Scale
        CurrentScaleText = m_LastScaleString;

        // Compute the new scale size by converting the string to a double (Strip off the % sign)
        NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

        // Set the New Scale
        SetSceneScaleByPercent(NewScalePercent);

    } else {
        // SCENE HAS SOMETHING IN IT
        // Get the rect that contains all items
        ItemsBoundingRect = m_WiringScene->itemsBoundingRect();

        // Adjust the Rect to add a little buffer
        ItemsBoundingRect.adjust(-20,-20, 20, 20);

        m_WiringView->fitInView(ItemsBoundingRect, Qt::KeepAspectRatio);

        // Update the Display
        UpdateSceneScaleDisplay();

        // Then have it set to the closest step size

        // Get the Current Scale
        CurrentScaleText = m_SceneScaleCombo->currentText();
        // Compute the new scale size by converting the string to a double (Strip off the % sign)
        NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();
        // Set the New Scale
        SetSceneScaleByPercent(NewScalePercent);
    }
}

void MainWindow::UserActionHandlerSetDynamicPorts()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // If a port has been selected, and this menu choosen then bring up the
    // dialog box (Menu Items are controlled by the type of component)
    if (m_SelectedComponent != NULL) {
        // Create the Dialog
        m_ConfigurePortsDialog = new DialogPortsConfig(m_SelectedComponent, this);

        // Run the dialog box (Modal)
        m_ConfigurePortsDialog->exec();

        SetProjectDirty();

        // Delete the Dialog
        delete m_ConfigurePortsDialog;
    }
}

void MainWindow::UserActionHandlerMovePorts()
{
    EnableMovingPorts(true);
}

void MainWindow::UserActionTextColorButtonTriggered()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Tell the scene to change its Default Text Color
    m_WiringScene->SetGraphicItemTextColor(qvariant_cast<QColor>(m_TextColorAction->data()));
}

void MainWindow::UserActionComponentFillColorButtonTriggered()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Tell the Scene to change its Default Component Fill Color
    m_WiringScene->SetGraphicItemComponentFillColor(qvariant_cast<QColor>(m_ComponentFillColorAction->data()));
}

void MainWindow::UserActionTextColorChanged()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Get The Action
    m_TextColorAction = qobject_cast<QAction*>(sender());

    // Set set the Icon for the Color Button with the image and new color
    m_FontColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/PointerText.png",
                                   qvariant_cast<QColor>(m_TextColorAction->data())));

    // Call the Handler just like the button was Toolbar button was clicked
    UserActionTextColorButtonTriggered();
}

void MainWindow::UserActionComponentFillColorChanged()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    // Get The Action
    m_ComponentFillColorAction = qobject_cast<QAction*>(sender());

    // Set set the Icon for the Color Button with the image and new color
    m_ComponentFillColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/ItemFloodfill.png",
                                            qvariant_cast<QColor>(m_ComponentFillColorAction->data())));

    // Call the Handler just like the button was Toolbar button was clicked
    UserActionComponentFillColorButtonTriggered();
}

void MainWindow::SetProjectDirty(bool DirtyFlag /*=true*/)
{
    // Set the Dirty Flag
    m_ProjectIsDirty = DirtyFlag;

    // Also set the undo stack as clean at this point
    if (DirtyFlag == false) {
        m_UndoStack->setClean();
    }

    UpdateDisplayBasedUponDirtyStatus();
}

void MainWindow::UpdateDisplayBasedUponDirtyStatus()
{
    // Check to see if SSTInfoData is Loaded
    if (IsSSTInfoDataLoaded() == true) {
        // We have SSTInfoData, but is the Project Dirty?
        m_WorkBenchSaveDesignAction->setEnabled(IsProjectDirty());
    } else {
        // If SSTInfoData is NOT Loaded, we cannot be dirty
        m_ProjectIsDirty = false;
        m_WorkBenchSaveDesignAction->setEnabled(false);
        m_WorkBenchSaveAsAction->setEnabled(false);
    }
}

bool MainWindow::IsProjectDirty()
{
    return m_ProjectIsDirty || !(m_UndoStack->isClean());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Called when the Window is requested to close
    // We can check to see if we meet the criteria
    // to close the window
    if (IsProjectDirty() == true) {
        if (QMessageBox::question(this, "Project Not Saved...", "The Current Project Has Not Been Saved.\n\n Are you sure you want to Exit") == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    SavePersistentStorage();
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Escape) {
        // Make sure a Component is moving ports
        // Then disable the Component to move ports
        EnableMovingPorts(false);
    }

    // Ignore the call
    ev->ignore();
}
