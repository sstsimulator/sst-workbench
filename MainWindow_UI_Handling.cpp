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

////////////////////////////////////////////////////////////
// NOTE: MainWindow Processing routines are located in MainWindow.cpp.
//       The routines here are for the MainWindow UI setup and
//       UI processing.  The files are separated to reduce single
//       file code bloat.
////////////////////////////////////////////////////////////

void MainWindow::SetMainWindowTitle(QString Title)
{
    // Set the name of the Window
    QString MainTitle = QString(COREAPP_MAINWINDOWNAME) + " " + COREAPP_VERSION;

    if (Title != UNTITLED) {
        // Add the project name to the window title
        MainTitle +=  " - " + Title;
    }

    setWindowTitle(MainTitle);
}

void MainWindow::SetMainWindowTitle(QFileInfo FileInfo)
{
    SetMainWindowTitle(FileInfo.baseName());
}

void MainWindow::SetFontControls(const QFont& font)
{
    // Given a font, set the toolbar controls to show the fonts settings
    m_FontSelectCombo->setCurrentFont(font);
    m_FontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    m_BoldAction->setChecked(font.weight() == QFont::Bold);
    m_ItalicAction->setChecked(font.italic());
    m_UnderlineAction->setChecked(font.underline());
}

void MainWindow::UpdateCurrentViewScaleDisplay()
{
    qreal   NewScaleValueF;
    int     NewScaleValue;
    QString NewScaleText;
    QString CurrentText;
    int     MatchIndex;
    int     SizeCount;

    // Figure out the current scaling and make it a percentage
    NewScaleValueF = m_CurrentWiringView->GetCurrentScaleValue();

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
    CurrentText = QString("%1").arg(NewScaleValue);   // FIRST TIME WITHOUT A % at end
    MatchIndex = m_SceneScaleCombo->findText(CurrentText);
    SizeCount = m_SceneScaleCombo->count();
    if (MatchIndex == SizeCount - 1) {
        if (CurrentText != m_LastScaleString) {
            m_SceneScaleCombo->removeItem(MatchIndex);
            m_SceneScaleCombo->setCurrentIndex(-1);
            m_SceneScaleCombo->setEditText(CurrentText);
        }
    }
    CurrentText = QString("%1%").arg(NewScaleValue);   // SECOND TIME WITH A % at end
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

void MainWindow::CreateActions()
{
    ////////////////////////////////////////////////////////
    // CREATE THE LIST OF ACTIONS THAT THE  USER CAN TAKE
    ////////////////////////////////////////////////////////
    // NOTE: The triggered signal for most of the actions is directed to the global handler
    // HandlePreprocessingCommonActionsTriggered() to do some pre-processing before the specific handler for
    // the action is called.  The order that the connect signals for the actions are created is important.

    ///
    m_BringToFrontAction = new QAction(QIcon(":/images/ItemBringToFront.png"), tr("Bring to &Front"), this);
    m_BringToFrontAction->setShortcuts(QKeySequence::Forward);
    m_BringToFrontAction->setStatusTip(tr("Bring Item To Front"));
    connect(m_BringToFrontAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_BringToFrontAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerBringToFront()));

    m_SendToBackAction = new QAction(QIcon(":/images/ItemSendToBack.png"), tr("Send to &Back"), this);
    m_SendToBackAction->setShortcuts(QKeySequence::Back);
    m_SendToBackAction->setStatusTip(tr("Send Item To Back"));
    connect(m_SendToBackAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SendToBackAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerSendToBack()));

    m_DeleteAction = new QAction(QIcon(":/images/ItemDelete.png"), tr("&Delete"), this);
    m_DeleteAction->setShortcuts(QKeySequence::Delete);
    m_DeleteAction->setStatusTip(tr("Delete Item"));
    m_DeleteAction->setEnabled(false);
    connect(m_DeleteAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_DeleteAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerDeleteItem()));

    ///
    m_UndoAction = m_UndoStack->createUndoAction(this, tr("&Undo"));
    m_UndoAction->setIcon(QIcon(":/images/Undo.png"));
    m_UndoAction->setShortcuts(QKeySequence::Undo);
    connect(m_UndoAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));

    m_RedoAction = m_UndoStack->createRedoAction(this, tr("&Redo"));
    m_RedoAction->setIcon(QIcon(":/images/Redo.png"));
    m_RedoAction->setShortcuts(QKeySequence::Redo);
    connect(m_RedoAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));

    m_SelectAllAction = new QAction(tr("Select &All"), this);
    m_SelectAllAction->setShortcuts(QKeySequence::SelectAll);
    m_SelectAllAction->setStatusTip(tr("Select All Items"));
    connect(m_SelectAllAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SelectAllAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerSelectAll()));

    ///
    m_CutAction = new QAction(tr("Cut"), this);
    m_CutAction->setShortcuts(QKeySequence::Cut);
    m_CutAction->setStatusTip(tr("Cut All Selected Items"));
    connect(m_CutAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_CutAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerCut()));

    m_CopyAction = new QAction(tr("Copy"), this);
    m_CopyAction->setShortcuts(QKeySequence::Copy);
    m_CopyAction->setStatusTip(tr("Copy All Selected Items"));
    connect(m_CopyAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_CopyAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerCopy()));

    m_PasteAction = new QAction(tr("Paste"), this);
    m_PasteAction->setShortcuts(QKeySequence::Paste);
    m_PasteAction->setStatusTip(tr("Paste Items"));
    connect(m_PasteAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PasteAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerPaste()));

    ///
    m_WorkBenchNewProjectAction = new QAction(QIcon(":/images/ProjectNew.png"), tr("&New Project..."), this);
    m_WorkBenchNewProjectAction->setShortcuts(QKeySequence::New);
    m_WorkBenchNewProjectAction->setStatusTip(tr("New SST Workbench Project"));
    connect(m_WorkBenchNewProjectAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_WorkBenchNewProjectAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerNewProject()));

    m_WorkBenchLoadDesignAction = new QAction(QIcon(":/images/ProjectOpen.png"), tr("&Open Project..."), this);
    m_WorkBenchLoadDesignAction->setShortcuts(QKeySequence::Open);
    m_WorkBenchLoadDesignAction->setStatusTip(tr("Load SST Workbench Project"));
    connect(m_WorkBenchLoadDesignAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_WorkBenchLoadDesignAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerLoadDesign()));

    m_WorkBenchSaveDesignAction = new QAction(QIcon(":/images/ProjectSave.png"), tr("&Save Project"), this);
    m_WorkBenchSaveDesignAction->setShortcuts(QKeySequence::Save);
    m_WorkBenchSaveDesignAction->setStatusTip(tr("Save SST Workbench Project"));
    m_WorkBenchSaveDesignAction->setEnabled(false);
    connect(m_WorkBenchSaveDesignAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_WorkBenchSaveDesignAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerSaveDesign()));

    m_WorkBenchSaveAsAction = new QAction(QIcon(":/images/ProjectSave.png"), tr("Save Project As..."), this);
    m_WorkBenchSaveAsAction->setShortcuts(QKeySequence::SaveAs);
    m_WorkBenchSaveAsAction->setStatusTip(tr("Save SST Workbench Project As"));
    m_WorkBenchSaveAsAction->setEnabled(false);
    connect(m_WorkBenchSaveAsAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_WorkBenchSaveAsAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerSaveAs()));

    m_ImportSSTInfoAction = new QAction(QIcon(":/images/ImportXML.png"), tr("Im&port SSTInfo Data..."), this);
    m_ImportSSTInfoAction->setShortcut(tr("Ctrl+F"));
    m_ImportSSTInfoAction->setStatusTip(tr("Import SSTInfo Data File"));
    connect(m_ImportSSTInfoAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ImportSSTInfoAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerImportSSTInfo()));

    m_ExportSSTInputDeckAction = new QAction(QIcon(":/images/SST_Single_S_Logo.png"), tr("E&xport SST Input Deck..."), this);
    m_ExportSSTInputDeckAction->setShortcut(tr("Ctrl+E"));
    m_ExportSSTInputDeckAction->setStatusTip(tr("Export Project To An SST Python Input Deck"));
    m_ExportSSTInputDeckAction->setEnabled(false);
    connect(m_ExportSSTInputDeckAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ExportSSTInputDeckAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerExportSSTInputDeck()));

    m_ExitAction = new QAction(tr("E&xit"), this);
    m_ExitAction->setMenuRole(QAction::QuitRole);
    m_ExitAction->setShortcuts(QKeySequence::Quit);
    m_ExitAction->setStatusTip(tr("Quit SST Workbench"));
    connect(m_ExitAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ExitAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerExit()));

    m_PreferencesAction = new QAction(tr("P&references"), this);
    m_PreferencesAction->setMenuRole(QAction::PreferencesRole);
    m_PreferencesAction->setShortcuts(QKeySequence::Preferences);
    m_PreferencesAction->setStatusTip(tr("Preferences"));
    connect(m_PreferencesAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PreferencesAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerPreferences()));

    m_AboutAction = new QAction(tr("A&bout SST Workbench"), this);
    m_AboutAction->setMenuRole(QAction::AboutRole);
    m_AboutAction->setStatusTip(tr("About SST Workbench"));
    connect(m_AboutAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_AboutAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerAbout()));

    m_PrintAction = new QAction(tr("&Print..."), this);
    m_PrintAction->setShortcuts(QKeySequence::Print);
    m_PrintAction->setStatusTip(tr("Print Current View"));
    connect(m_PrintAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PrintAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerPrint()));

    ///
    m_BoldAction = new QAction(tr("Bold"), this);
    m_BoldAction->setCheckable(true);
    QPixmap pixmap(":/images/TextBold.png");
    m_BoldAction->setIcon(QIcon(pixmap));
    m_BoldAction->setShortcuts(QKeySequence::Bold);
    m_BoldAction->setStatusTip(tr("Bold"));
    connect(m_BoldAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_BoldAction, SIGNAL(triggered()), this, SLOT(ToolbarActionHandlerFontChange()));

    m_ItalicAction = new QAction(QIcon(":/images/TextItalic.png"), tr("Italic"), this);
    m_ItalicAction->setCheckable(true);
    m_ItalicAction->setShortcuts(QKeySequence::Italic);
    m_ItalicAction->setStatusTip(tr("Italic"));
    connect(m_ItalicAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ItalicAction, SIGNAL(triggered()), this, SLOT(ToolbarActionHandlerFontChange()));

    m_UnderlineAction = new QAction(QIcon(":/images/TextUnderline.png"), tr("Underline"), this);
    m_UnderlineAction->setCheckable(true);
    m_UnderlineAction->setShortcuts(QKeySequence::Underline);
    m_UnderlineAction->setStatusTip(tr("Underline"));
    connect(m_UnderlineAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_UnderlineAction, SIGNAL(triggered()), this, SLOT(ToolbarActionHandlerFontChange()));

    ///
    // Build the Component Menu Actions
    m_MovePortsAction = new QAction("&Move Port Positions", this);
    m_MovePortsAction->setStatusTip(tr("Move Component Ports"));
    connect(m_MovePortsAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_MovePortsAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerMovePorts()));

    m_SetDynamicPortsAction = new QAction("Set Dynamic Por&ts...", this);
    m_SetDynamicPortsAction->setStatusTip(tr("Set Component Dynamic Ports"));
    m_SetDynamicPortsAction->setVisible(false);
    connect(m_SetDynamicPortsAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SetDynamicPortsAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerSetDynamicPorts()));

    m_ManageModulesAction = new QAction("Mana&ge Component Modules...", this);
    m_ManageModulesAction->setStatusTip(tr("Manage the Components Modules"));
    m_ManageModulesAction->setVisible(false);
    connect(m_ManageModulesAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ManageModulesAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerManageModules()));

    ///
    // Actions for displaying the Toolbars
    m_ShowToolsToolbarAction = new QAction("Tools", this);
    m_ShowToolsToolbarAction->setStatusTip(tr("Enable/Disable Tools Toolbar"));
    m_ShowToolsToolbarAction->setCheckable(true);
    m_ShowToolsToolbarAction->setChecked(true);
    connect(m_ShowToolsToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowToolsToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    m_ShowItemToolbarAction = new QAction("Item Control", this);
    m_ShowItemToolbarAction->setStatusTip(tr("Enable/Disable Item Control Toolbar"));
    m_ShowItemToolbarAction->setCheckable(true);
    m_ShowItemToolbarAction->setChecked(true);
    connect(m_ShowItemToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowItemToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    m_ShowFontControlToolbarAction = new QAction("Font Control", this);
    m_ShowFontControlToolbarAction->setStatusTip(tr("Enable/Disable Font Control Toolbar"));
    m_ShowFontControlToolbarAction->setCheckable(true);
    m_ShowFontControlToolbarAction->setChecked(true);
    connect(m_ShowFontControlToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowFontControlToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    m_ShowViewControlToolbarAction = new QAction("View Control", this);
    m_ShowViewControlToolbarAction->setStatusTip(tr("Enable/Disable View Control Toolbar"));
    m_ShowViewControlToolbarAction->setCheckable(true);
    m_ShowViewControlToolbarAction->setChecked(true);
    connect(m_ShowViewControlToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowViewControlToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    m_ShowFileActionsToolbarAction = new QAction("File Actions", this);
    m_ShowFileActionsToolbarAction->setStatusTip(tr("Enable/Disable File Actions Toolbar"));
    m_ShowFileActionsToolbarAction->setCheckable(true);
    m_ShowFileActionsToolbarAction->setChecked(true);
    connect(m_ShowFileActionsToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowFileActionsToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    m_ShowEditToolbarAction = new QAction("Edit", this);
    m_ShowEditToolbarAction->setStatusTip(tr("Enable/Disable Edit Toolbar"));
    m_ShowEditToolbarAction->setCheckable(true);
    m_ShowEditToolbarAction->setChecked(true);
    connect(m_ShowEditToolbarAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ShowEditToolbarAction, SIGNAL(triggered()), this, SLOT(MenuActionHandlerShowToolbars()));

    ///
    // Page Handling
    m_PageOpen = new QAction(QIcon(":/images/ProjectOpen.png"), tr("Open Page"), this);
//    m_PageOpen->setShortcuts(QKeySequence::ZoomIn);
    m_PageOpen->setToolTip(tr("Open Page"));
    m_PageOpen->setStatusTip(tr("Open Page"));
    connect(m_PageOpen, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PageOpen, SIGNAL(triggered()), this, SLOT(MenuActionPageOpen()));

    m_PageClose = new QAction(QIcon(":/images/ProjectClose.png"), tr("Close Page"), this);
//    m_PageClose->setShortcuts(QKeySequence::ZoomIn);
    m_PageClose->setToolTip(tr("Close Page"));
    m_PageClose->setStatusTip(tr("Close Page"));
    connect(m_PageClose, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PageClose, SIGNAL(triggered()), this, SLOT(MenuActionPageClose()));

    m_PageRename = new QAction(QIcon(":/images/ProjectNew.png"), tr("Rename Page"), this);
//    m_PageRename->setShortcuts(QKeySequence::ZoomIn);
    m_PageRename->setToolTip(tr("Rename Page"));
    m_PageRename->setStatusTip(tr("Rename Page"));
    connect(m_PageRename, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PageRename, SIGNAL(triggered()), this, SLOT(MenuActionPageRename()));

    m_PageNew = new QAction(QIcon(":/images/ProjectNew.png"), tr("New Page"), this);
//    m_PageNew->setShortcuts(QKeySequence::ZoomIn);
    m_PageNew->setToolTip(tr("New Page"));
    m_PageNew->setStatusTip(tr("New Page"));
    connect(m_PageNew, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PageNew, SIGNAL(triggered()), this, SLOT(MenuActionPageNew()));

    m_PageDelete = new QAction(QIcon(":/images/ProjectNew.png"), tr("Delete Page"), this);
//    m_PageDelete->setShortcuts(QKeySequence::ZoomIn);
    m_PageDelete->setToolTip(tr("Delete Page"));
    m_PageDelete->setStatusTip(tr("Delete Page"));
    connect(m_PageDelete, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_PageDelete, SIGNAL(triggered()), this, SLOT(MenuActionPageDelete()));

    ///
    // Zoom In
    m_SceneScaleZoomInAction = new QAction(QIcon(":/images/ZoomIn.png"), tr("Zoom In"), this);
    m_SceneScaleZoomInAction->setShortcuts(QKeySequence::ZoomIn);
    m_SceneScaleZoomInAction->setToolTip(tr("Zoom In"));
    m_SceneScaleZoomInAction->setStatusTip(tr("Zoom In"));
//    connect(m_SceneScaleZoomInAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SceneScaleZoomInAction, SIGNAL(triggered()), this, SLOT(MenuActionSceneScaleZoomIn()));

    // Zoom Out
    m_SceneScaleZoomOutAction = new QAction(QIcon(":/images/ZoomOut.png"), tr("Zoom Out"), this);
    m_SceneScaleZoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    m_SceneScaleZoomOutAction->setToolTip(tr("Zoom Out"));
    m_SceneScaleZoomOutAction->setStatusTip(tr("Zoom Out"));
//    connect(m_SceneScaleZoomOutAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SceneScaleZoomOutAction, SIGNAL(triggered()), this, SLOT(MenuActionSceneScaleZoomOut()));

    // Zoom All
    m_SceneScaleZoomAllAction = new QAction(QIcon(":/images/ZoomAll.png"), tr("Zoom Al&l"), this);
    m_SceneScaleZoomAllAction->setShortcut(tr("Ctrl+L"));
    m_SceneScaleZoomAllAction->setToolTip(tr("Zoom All"));
    m_SceneScaleZoomAllAction->setStatusTip(tr("Zoom All"));
//    connect(m_SceneScaleZoomAllAction, SIGNAL(triggered()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_SceneScaleZoomAllAction, SIGNAL(triggered()), this, SLOT(MenuActionSceneScaleZoomAll()));
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
    m_GenericItemMenu->addSeparator();
    m_GenericItemMenu->addAction(m_ManageModulesAction);

    // Page Menu
    m_PageOpenMenu = new QMenu(tr("Open Page"));
    m_PageOpenMenu->setIcon( QIcon(":/images/ProjectOpen.png"));
    m_PageMenu = menuBar()->addMenu(tr("&Page"));
    m_PageMenu->addMenu(m_PageOpenMenu);
    m_PageMenu->addAction(m_PageClose);
    m_PageMenu->addAction(m_PageRename);
    m_PageMenu->addAction(m_PageNew);
    m_PageMenu->addAction(m_PageDelete);

    // View Menu
    m_ViewMenu = menuBar()->addMenu(tr("&View"));
    m_ViewMenu->addSeparator();
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
    connect(m_FontSelectCombo, SIGNAL(activated(QString)), this, SLOT(ToolbarActionFontSelectChanged(QString)));

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
    connect(m_FontSizeCombo, SIGNAL(activated(QString)), this, SLOT(ToolbarActionFontSizeChanged(QString)));

    // Font Color Button
    m_FontColorToolButton = new QToolButton;
    m_FontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_FontColorToolButton->setMenu(CreateColorMenu(SLOT(ToolbarActionTextColorChanged()), Qt::black));
    m_FontColorToolButton->setToolTip(tr("Set Text Color"));
    m_FontColorToolButton->setStatusTip(tr("Set Text Color"));
    m_TextColorAction = m_FontColorToolButton->menu()->defaultAction();
    m_FontColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/PointerText.png", Qt::black));
    // m_FontColorToolButton->setAutoFillBackground(true);
    connect(m_FontColorToolButton, SIGNAL(clicked()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_FontColorToolButton, SIGNAL(clicked()), this, SLOT(ToolbarActionTextColorButtonTriggered()));

    // ComponentFill Color Button
    m_ComponentFillColorToolButton = new QToolButton;
    m_ComponentFillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_ComponentFillColorToolButton->setMenu(CreateColorMenu(SLOT(ToolbarActionComponentFillColorChanged()), Qt::white, true));
    m_ComponentFillColorToolButton->setToolTip(tr("Set Component Color"));
    m_ComponentFillColorToolButton->setStatusTip(tr("Set Component Color"));
    m_ComponentFillColorAction = m_ComponentFillColorToolButton->menu()->defaultAction();
    m_ComponentFillColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/ItemFloodfill.png", Qt::white));
    connect(m_ComponentFillColorToolButton, SIGNAL(clicked()), this, SLOT(HandlePreprocessingCommonActionsTriggered()));
    connect(m_ComponentFillColorToolButton, SIGNAL(clicked()), this, SLOT(ToolbarActionComponentFillColorButtonTriggered()));

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
    m_ToolsButtonGroup = new QButtonGroup(this);
    m_ToolsButtonGroup->setExclusive(false);  // Allow buttons to not be mutually exclusive, mut. exclusion Done in handlers
    m_ToolsButtonGroup->addButton(PointerSelectMoveButton, int(SCENEMODE_SELECTMOVEITEM));
    m_ToolsButtonGroup->addButton(PointerAddWireButton, int(SCENEMODE_ADDWIRE));
    m_ToolsButtonGroup->addButton(PointerAddTextButton, int(SCENEMODE_ADDTEXT));
    // If a buton of this group is clicked, then call the function
    connect(m_ToolsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(ToolButtonGroupActionPointerGroupClicked(int)));

    // The Scaling combo box for the Scene scale
    m_SceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("25%") << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%") << tr("175%") << tr("200%");
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
    connect(m_SceneScaleCombo, SIGNAL(activated(QString)), this, SLOT(ToolbarActionSceneScaleChanged(QString)));

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
  statusBar()->showMessage("");
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

void MainWindow::UpdateMenuAndToolbars()
{
    // If SSTInfoData is NOT Loaded, we cannot be dirty
    if (m_CompToolBox->IsSSTInfoDataLoaded() == false) {
        m_ProjectIsDirty = false;
    }

    // Enable/Disable the Delete Actions (Menu and Toolbar)
    // Based upon what is selected
    m_DeleteAction->setEnabled(m_CurrentWiringScene->GetNumSelectedGraphicalItems() > 0);

    // Enable/Disable the Export Button if the SSTInfo.XML data has been loaded
    // TODO: CHECK TO SEE IF A COMPONENT IS IN THE WIRING WINDOW
    m_ExportSSTInputDeckAction->setEnabled(m_CompToolBox->IsSSTInfoDataLoaded());

    // Enable / Disable the Save Design Action if the Project is Dirty AND if SSTInfo.XML data has been loaded
    m_WorkBenchSaveDesignAction->setEnabled(IsProjectDirty() && m_CompToolBox->IsSSTInfoDataLoaded());

    // Enable / Disable the Save As Action if the SSTInfo.XML data has been loaded
    m_WorkBenchSaveAsAction->setEnabled(m_CompToolBox->IsSSTInfoDataLoaded());

    // Check to see if we picked a component, and then make visible/invisible the appropriate Item menu entries
    if (m_SelectedComponent != NULL) {
        // Make MovePorts Menu visible if there are ports on the component, and the ports are not moving
        m_MovePortsAction->setVisible((m_SelectedComponent->GetNumGraphicalPortsOnComponent() > 0) && (IsMovingPortsEnabled() == false));

        // Make SetDynamicPorts Menu Visible if the component contains dynamic Ports
        m_SetDynamicPortsAction->setVisible(m_SelectedComponent->ComponentContainsDynamicPorts());

        // Always make ManageModules menu visible
        m_ManageModulesAction->setVisible(true);
    } else {
        // Make all Item Menu entries invisible
        m_MovePortsAction->setVisible(false);
        m_SetDynamicPortsAction->setVisible(false);
        m_ManageModulesAction->setVisible(false);
    }
}

void MainWindow::TurnOffAllPointerToolButtons()
{
    // Turn off all the Pointer Toolbar Buttons (MODE_ADDWIRE, MODE_ADDTEXT, or MODE_SELECTMOVEITEM)
    QList<QAbstractButton*> PointerToolbarButtons = m_ToolsButtonGroup->buttons();

    // Turn off the checked setting for all other buttons
    foreach (QAbstractButton* button, PointerToolbarButtons) {
        button->setChecked(false);
    }

    setCursor(Qt::PointingHandCursor);
}

void MainWindow::TurnOnAPointerToolButton(int id)
{
    // Turn on the selected Pointer Toolbar Button, but turn all the others off
    QList<QAbstractButton*> PointerToolbarButtons = m_ToolsButtonGroup->buttons();

    foreach (QAbstractButton* button, PointerToolbarButtons) {
        if (m_ToolsButtonGroup->button(id) != button) {
            button->setChecked(false);
        } else {
            button->setChecked(true);
        }
    }

    switch (id) {
        case SCENEMODE_SELECTMOVEITEM :
            setCursor(Qt::ArrowCursor);
        break;
        case SCENEMODE_ADDWIRE :
            setCursor(Qt::CrossCursor);
        break;
        case SCENEMODE_ADDTEXT :
            setCursor(Qt::IBeamCursor);
        break;
    }
}

void MainWindow::ClickPointToolButton(int id)
{
    // Pretend click of the appropriate toolbar button
    emit m_ToolsButtonGroup->buttonClicked(int(id));
}

void MainWindow::HandleUndoStackCleanChanged(bool NewState)
{
    Q_UNUSED(NewState)

    // Update the Menus and Toolbars
    UpdateMenuAndToolbars();
}

void MainWindow::MenuActionHandlerBringToFront()
{
    // Check to see if the scene is empty
    if (m_CurrentWiringScene->IsSceneEmpty() == true) {
        return;
    }

    // Get the first selected item
    QGraphicsItem* selectedItem = m_CurrentWiringScene->GetFirstSelectedGraphicalItem();

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

void MainWindow::MenuActionHandlerSendToBack()
{
    // Check to see if the scene is empty
    if (m_CurrentWiringScene->IsSceneEmpty() == true) {
        return;
    }

    // Get the first selected item
    QGraphicsItem* selectedItem = m_CurrentWiringScene->GetFirstSelectedGraphicalItem();

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

void MainWindow::MenuActionHandlerDeleteItem()
{
    m_CurrentWiringScene->DeleteAllSelectedItems();

    // Set the selected objects to NULL
    m_SelectedComponent = NULL;
    m_SelectedText = NULL;
}

void MainWindow::MenuActionHandlerSelectAll()
{
    m_CurrentWiringScene->SelectAllGraphicalItems();
}

/*
void MainWindow::MenuActionHandlerAbout()
{
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
*/
void MainWindow::MenuActionHandlerAbout()
{
    // Disable Moving Ports
    EnableMovingPorts(false);

    QString FinalAboutText;
    QString AboutText = "";
    QString SSTVersion = "";

    AboutText += "<center><h2>SST Workbench</h2>Version %1<br><br>";

    AboutText += "Built With Qt Version %2<br>";
    AboutText += "Using Qt Runtime Libraries %3<br><br>";

    AboutText += "Copyright (c) 2009-2014<br>Sandia Corporation.  ";
    AboutText += "Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, ";
    AboutText += "the U.S. Government retains certain rights in this software.<br><br>";
    AboutText += "Copyright (c) 2009-2014,<br>Sandia Corporation<br>All rights reserved.<br><br>";

    AboutText += "<img src=:/images/SSTLogo.png><br>";

    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        AboutText += "SST Version %4<br>";
        SSTVersion = m_CompToolBox->GetSSTInfoData()->GetSSTInfoVersion();
    }

    AboutText += "<br>This Application is part of the SST Software package. ";
    AboutText += "For license information, see the LICENSE file in the top level directory of the distribution.</center> ";

    if (m_CompToolBox->GetSSTInfoData() != NULL) {
        FinalAboutText = AboutText.arg(COREAPP_VERSION).arg(QT_VERSION_STR).arg(qVersion()).arg(SSTVersion);
    } else {
        FinalAboutText = AboutText.arg(COREAPP_VERSION).arg(QT_VERSION_STR).arg(qVersion());
    }

    QMessageBox::about(this, tr("About SST Workbench"), FinalAboutText);
}

void MainWindow::MenuActionHandlerExit()
{
    close();
}

void MainWindow::MenuActionHandlerShowToolbars()
{
    // Enable / Disable the appropriate Toolbar based upon the menu settings
    m_FileActionsToolbar->setVisible(m_ShowFileActionsToolbarAction->isChecked());
    m_EditToolbar->setVisible(m_ShowEditToolbarAction->isChecked());
    m_ToolsToolbar->setVisible(m_ShowToolsToolbarAction->isChecked());
    m_ItemToolBar->setVisible(m_ShowItemToolbarAction->isChecked());
    m_FontControlToolBar->setVisible(m_ShowFontControlToolbarAction->isChecked());
    m_ViewControlToolbar->setVisible(m_ShowViewControlToolbarAction->isChecked());
}

void MainWindow::ToolbarActionFontSelectChanged(const QString&)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    ToolbarActionHandlerFontChange();
}

void MainWindow::ToolbarActionFontSizeChanged(const QString&)
{
    // Perform any pre-processing common actions
    emit PerformPreprocessingCommonActions();

    ToolbarActionHandlerFontChange();
}

void MainWindow::ToolbarActionHandlerFontChange()
{
    // Build a font based on the user controls
    QFont font = m_FontSelectCombo->currentFont();
    font.setPointSize(m_FontSizeCombo->currentText().toInt());
    font.setWeight(m_BoldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(m_ItalicAction->isChecked());
    font.setUnderline(m_UnderlineAction->isChecked());

    // Tell the scene to change its font
    m_CurrentWiringScene->SetGraphicItemTextFont(font);

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

void MainWindow::ToolbarActionSceneScaleChanged(const QString& NewScaleText)
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = NewScaleText;

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Set the New Scale
    SetCurrentViewScaleByPercent(NewScalePercent);
}

void MainWindow::MenuActionSceneScaleZoomIn()
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = m_SceneScaleCombo->currentText();

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Change the Zoom by a 25% change
    //NewScalePercent *= 1.25;            // Increase by 25%
    NewScalePercent += ZOOM_STEP_SIZE;  // Increase by a fixed step size

    // Set the New Scale
    SetCurrentViewScaleByPercent(NewScalePercent);
}

void MainWindow::MenuActionSceneScaleZoomOut()
{
    QString CurrentScaleText;
    double  NewScalePercent;

    // Get the Current Scale
    CurrentScaleText = m_SceneScaleCombo->currentText();

    // Compute the new scale size by converting the string to a double (Strip off the % sign)
    NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

    // Change the Zoom by a 25% change
//    NewScalePercent /= 1.25;            // Decrease by 25%
    NewScalePercent -= ZOOM_STEP_SIZE;  // Decrease by a fixed step size

    // Set the New Scale
    SetCurrentViewScaleByPercent(NewScalePercent);
}

void MainWindow::MenuActionSceneScaleZoomAll()
{
    QString CurrentScaleText;
    double  NewScalePercent;
    QRectF  ItemsBoundingRect;

    // See if there is anything in the Scene
    if (m_CurrentWiringScene->IsSceneEmpty() == true) {
        // EMPTY SCENE
        // Get the Current Scale
        CurrentScaleText = m_LastScaleString;

        // Compute the new scale size by converting the string to a double (Strip off the % sign)
        NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

        // Set the New Scale
        SetCurrentViewScaleByPercent(NewScalePercent);

    } else {
        // SCENE HAS SOMETHING IN IT
        // Get the rect that contains all items
        ItemsBoundingRect = m_CurrentWiringScene->GetAllGraphicalItemsBoundingRect();

        // Adjust the Rect to add a little buffer
        ItemsBoundingRect.adjust(-20,-20, 20, 20);

        m_CurrentWiringView->FitRectIntoView(ItemsBoundingRect);

        // Update the Display
        UpdateCurrentViewScaleDisplay();

        // Then have it set to the closest step size

        // Get the Current Scale
        CurrentScaleText = m_SceneScaleCombo->currentText();
        // Compute the new scale size by converting the string to a double (Strip off the % sign)
        NewScalePercent = CurrentScaleText.left(CurrentScaleText.indexOf(tr("%"))).toDouble();

        // Force us to zoom out one extra step to make sure we fit
        NewScalePercent -= ZOOM_STEP_SIZE;

        // Set the New Scale
        SetCurrentViewScaleByPercent(NewScalePercent);
    }
}

void MainWindow::MenuActionHandlerMovePorts()
{
    EnableMovingPorts(true);
}

void MainWindow::MenuActionHandlerSetDynamicPorts()
{
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

void MainWindow::MenuActionHandlerManageModules()
{
    // If a port has been selected, and this menu choosen then bring up the
    // dialog box (Menu Items are controlled by the type of component)
    if (m_SelectedComponent != NULL) {
        // Create the Dialog
        m_ManageModulesDialog = new DialogManageModules(m_SelectedComponent, m_CompToolBox->GetSSTInfoData(), this);

        // Run the dialog box (Modal)
        m_ManageModulesDialog->exec();

        SetProjectDirty();

        // Delete the Dialog
        delete m_ManageModulesDialog;
    }
}

void MainWindow::ToolbarActionTextColorButtonTriggered()
{
    // Tell the scene to change its Default Text Color
    m_CurrentWiringScene->SetGraphicItemTextColor(qvariant_cast<QColor>(m_TextColorAction->data()));
}

void MainWindow::ToolbarActionComponentFillColorButtonTriggered()
{
    // Tell the Scene to change its Default Component Fill Color
    m_CurrentWiringScene->SetGraphicItemComponentFillColor(qvariant_cast<QColor>(m_ComponentFillColorAction->data()));
}

void MainWindow::ToolbarActionTextColorChanged()
{
    // Get The Action
    m_TextColorAction = qobject_cast<QAction*>(sender());

    // Set set the Icon for the Color Button with the image and new color
    m_FontColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/PointerText.png",
                                   qvariant_cast<QColor>(m_TextColorAction->data())));

    // Call the Handler just like the button was Toolbar button was clicked
    emit m_FontColorToolButton->clicked();
}

void MainWindow::ToolbarActionComponentFillColorChanged()
{
    // Get The Action
    m_ComponentFillColorAction = qobject_cast<QAction*>(sender());

    // Set set the Icon for the Color Button with the image and new color
    m_ComponentFillColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/ItemFloodfill.png",
                                            qvariant_cast<QColor>(m_ComponentFillColorAction->data())));

    // Call the Handler just like the button was Toolbar button was clicked
    emit m_ComponentFillColorToolButton->clicked();
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
