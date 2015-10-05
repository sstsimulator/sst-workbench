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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GlobalIncludes.h"
#include <QtPrintSupport>

#include "WiringScene.h"
#include "WiringView.h"
#include "WindowItemProperties.h"
#include "WindowComponentToolbox.h"

#include "GraphicItemComponent.h"
#include "GraphicItemText.h"
#include "GraphicItemWire.h"
#include "DialogPortsConfig.h"
#include "DialogManageModules.h"
#include "DialogPreferences.h"
#include "SSTInfoXMLFileParser.h"
#include "PythonExporter.h"

////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor / Destructor
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    // Project Dirty flag
    void SetProjectDirty(bool DirtyFlag = true);
    bool IsProjectDirty();

    // Set the Wiring Window Tab Title
    void SetMainWindowTitle(QString Title);
    void SetMainWindowTitle(QFileInfo FileInfo);

    // Set the Font
    void SetFontControls(const QFont& font);

    // Control the Scene display
    void SetCurrentViewScaleByPercent(double ScalePercent);
    void UpdateCurrentViewScaleDisplay();

    // Handling of Persistant Storage
    void SavePersistentStorage();
    void RestorePersistentStorage();

    // Get access to the Persistent Settings object
    QSettings* GetPersistentSettings() {return m_PersistentSettings;}

    // Handling of Project Data (Serialization)
    bool SaveProjectData(QString ProjectFilePathName);
    bool LoadProjectData(QString ProjectFilePathName);

    // Build up the Menus and Toolbar buttons the user can press for Actions
    void CreateActions();
    void CreateMenus();
    void CreateToolbars();
    void CreateStatusbar();

    // Support routines to create color menus and buttons for the Menus and Toolbars
    QIcon  CreateColorIcon(QColor color);
    QMenu* CreateColorMenu(const char* slot, QColor defaultColor, bool ComponentColors = false);
    QIcon  CreateColorToolButtonIcon(const QString& image, QColor color);

    // Control of Moving Ports
    void EnableMovingPorts(bool Enable);
    bool IsMovingPortsEnabled() {return (m_ComponentMovingPorts != NULL);}

    // Update Menu and Toolbar Controls
    void UpdateMenuAndToolbars();

    // Copy Paste Support
    void ClearCopyPasteBuffer();

    // Print Support
    void PrintStandardSetup(QPrinter& Printer, QPainter& Painter);
    void PrintWiringWindow(QPrinter& Printer, QPainter& Painter, int& PageNum);
    void PrintComponentParameters(QPrinter& Printer, QPainter& Painter, int& PageNum);
    void PrintStartNewPage(QPrinter& Printer, QPainter& Painter, int& PageNum);
    void PrintPageHeader(QPrinter& Printer, QPainter& Painter, int PageNum);

    // PointerToolsButtonGroup Support
    void TurnOffAllPointerToolButtons();
    void TurnOnAPointerToolButton(int id);
    void ClickPointToolButton(int id);

    // Tab Page Handling
    int GetPageIndexFromName(QString PageName);
    bool IsPageNameRedundant(QString PageName);
    QString GetPageNameFromUser(bool NewTabFlag, QString DefaultText = "");  // NewTabFlag = 1 for New Tab, 0 for Renaming Tab
    void UpdatePageTabUIInformation();
    int CreateNewSceneViewAndPutIntoLists(); // returns index of lists
    void DeletePageSceneView(int PageIndex);

signals:
    void PerformPreprocessingCommonActions();
    void PreferenceChangedAutoDeleteTooShortWire(bool NewValue);

private slots:
    // Global Handler for all Actions
    void HandlePreprocessingCommonActionsTriggered();

    // Handlers for Tabbed Window Events
    void HandleTabWindowEventTabChanged(int);
    void HandleTabWindowEventTabCloseRequested(int);

    // Handlers User Actions From Component, Properties or SceneView Windows
    void ToolboxWindowActionComponentItemPressed(SSTInfoDataComponent* ptrComponent);
    void ToolButtonGroupActionPointerGroupClicked(int id);

    // Handlers for events from the Graphics Scene
    void HandleSceneEventComponentAdded(GraphicItemComponent*);
    void HandleSceneEventTextAdded(GraphicItemText*);
    void HandleSceneEventWireAddedInitialPlacement(GraphicItemWire*);
    void HandleSceneEventWireAddedFinalPlacement(GraphicItemWire*);
    void HandleSceneEventSelectionChanged();
    void HandleSceneEventGraphicItemSelected(QGraphicsItem*);
    void HandleSceneEventGraphicItemSelectedProperties(ItemProperties*);
    void HandleSceneEventSetProjectDirty();
    void HandleSceneEventRefreshPropertiesWindowProperty(QString, QString);
    void HandleSceneEventRefreshPropertiesWindow(ItemProperties*);
    void HandleSceneEventDragAndDropFinished();
    void HandleUndoStackCleanChanged(bool NewState);

    // Handlers for User Actions From Menus and/or Toolbars
    void MenuActionHandlerBringToFront();
    void MenuActionHandlerSendToBack();
    void MenuActionHandlerDeleteItem();

    // Select All
    void MenuActionHandlerSelectAll();

    // Cut / Copy / Paste
    void MenuActionHandlerCut();
    void MenuActionHandlerCopy();
    void MenuActionHandlerPaste();

    // Misc Handlers
    void MenuActionHandlerNewProject();
    void MenuActionHandlerLoadDesign();
    void MenuActionHandlerSaveDesign();
    void MenuActionHandlerSaveAs();
    void MenuActionHandlerImportSSTInfo();
    void MenuActionHandlerExportSSTInputDeck();
    void MenuActionHandlerPreferences();
    void MenuActionHandlerAbout();
    void MenuActionHandlerPrint();
    void MenuActionHandlerExit();
    void MenuActionHandlerShowToolbars();

    // Font Handling
    void ToolbarActionFontSelectChanged(const QString& font);
    void ToolbarActionFontSizeChanged(const QString& size);
    void ToolbarActionHandlerFontChange();

    // Page (tabs) Handling
    void MenuActionPageOpen();
    void MenuActionPageClose(int TabIndex = -1);
    void MenuActionPageRename(QString ProvidedPageName = "");
    void MenuActionPageNew(QString ProvidedPageName = "");
    void MenuActionPageDelete(int TabIndex = -1);

    // Zoom and Scale Handlers
    void ToolbarActionSceneScaleChanged(const QString& NewScaleText);
    void MenuActionSceneScaleZoomIn();
    void MenuActionSceneScaleZoomOut();
    void MenuActionSceneScaleZoomAll();

    // Component Menu Handlers
    void MenuActionHandlerMovePorts();
    void MenuActionHandlerSetDynamicPorts();
    void MenuActionHandlerManageModules();

    // Handlers for Colors of Text and Component Background
    void ToolbarActionTextColorButtonTriggered();
    void ToolbarActionComponentFillColorButtonTriggered();
    void ToolbarActionTextColorChanged();
    void ToolbarActionComponentFillColorChanged();

private:
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* ev);

private:
    // The Scene, View, Component, Property, Dialogs & Support Windows
    QSplitter*              m_MainSplitterWidget;
    QTabWidget*             m_TabWiringWindow;

    QStringList             m_PageNamesList;
    QList<WiringScene*>     m_WiringScenesList;
    QList<WiringView*>      m_WiringViewsList;
    WiringScene*            m_CurrentWiringScene;
    WiringView*             m_CurrentWiringView;

    WindowComponentToolBox* m_CompToolBox;
    WindowItemProperties*   m_PropWin;
    DialogPortsConfig*      m_ConfigurePortsDialog;
    DialogManageModules*    m_ManageModulesDialog;
    DialogPreferences*      m_PreferencesDialog;

    // Undo / Redo Processing
    QUndoStack*             m_UndoStack;
QUndoView*              m_UndoViewWindow;

    // Menus for the Main Window
    QMenu*                  m_FileMenu;
    QMenu*                  m_EditMenu;
    QMenu*                  m_GenericItemMenu;
    QMenu*                  m_ViewMenu;
    QMenu*                  m_PageMenu;
    QMenu*                  m_PageOpenMenu;
    QMenu*                  m_ToolbarMenu;
    QMenu*                  m_AboutMenu;

    // ToolBars for the Main Window
    QToolBar*               m_FileActionsToolbar;
    QToolBar*               m_EditToolbar;
    QToolBar*               m_ToolsToolbar;
    QToolBar*               m_ItemToolBar;
    QToolBar*               m_FontControlToolBar;
    QToolBar*               m_ViewControlToolbar;

    // Controls used by the ToolBars
    QFontComboBox*          m_FontSelectCombo;
    QComboBox*              m_FontSizeCombo;
    QToolButton*            m_FontColorToolButton;
    QToolButton*            m_ComponentFillColorToolButton;
    QButtonGroup*           m_ToolsButtonGroup;
    QComboBox*              m_SceneScaleCombo;

    // All of the Various Actions that the user can take
    QAction*                m_BringToFrontAction;
    QAction*                m_SendToBackAction;
    QAction*                m_DeleteAction;

    QAction*                m_UndoAction;
    QAction*                m_RedoAction;
    QAction*                m_SelectAllAction;

    QAction*                m_CutAction;
    QAction*                m_CopyAction;
    QAction*                m_PasteAction;

    QAction*                m_WorkBenchNewProjectAction;
    QAction*                m_WorkBenchLoadDesignAction;
    QAction*                m_WorkBenchSaveDesignAction;
    QAction*                m_WorkBenchSaveAsAction;
    QAction*                m_ImportSSTInfoAction;
    QAction*                m_ExportSSTInputDeckAction;
    QAction*                m_ExitAction;
    QAction*                m_PreferencesAction;
    QAction*                m_AboutAction;
    QAction*                m_PrintAction;

    QAction*                m_BoldAction;
    QAction*                m_ItalicAction;
    QAction*                m_UnderlineAction;
    QAction*                m_TextColorAction;
    QAction*                m_ComponentFillColorAction;

    QAction*                m_MovePortsAction;
    QAction*                m_SetDynamicPortsAction;
    QAction*                m_ManageModulesAction;

    QAction*                m_ShowFileActionsToolbarAction;
    QAction*                m_ShowEditToolbarAction;
    QAction*                m_ShowToolsToolbarAction;
    QAction*                m_ShowItemToolbarAction;
    QAction*                m_ShowFontControlToolbarAction;
    QAction*                m_ShowViewControlToolbarAction;

    QAction*                m_PageOpen;
    QAction*                m_PageClose;
    QAction*                m_PageRename;
    QAction*                m_PageNew;
    QAction*                m_PageDelete;

    QAction*                m_SceneScaleZoomInAction;
    QAction*                m_SceneScaleZoomOutAction;
    QAction*                m_SceneScaleZoomAllAction;

    // Pointer to the Currently Selected GraphicItems
    GraphicItemComponent*   m_SelectedComponent;
    GraphicItemComponent*   m_ComponentMovingPorts;
    GraphicItemText*        m_SelectedText;

    // Window Persistent Data
    QSettings*              m_PersistentSettings;

    // SSTInfo XML Data File Data (SSTInfo XML Import file)
    QString                 m_LastSavedSSTInfoXMLDataFilePathName;

    // Python Export
    QString                 m_LastExportedPythonFilePathName;

    // Project File Data (Project Open/Save file)
    QString                 m_LastSavedProjectDataFilePathName;
    QString                 m_LoadedProjectDataFilePathName;
    bool                    m_ProjectIsDirty;
    QFont                   m_DefaultFont;
    QString                 m_LastFontSizeString;
    QString                 m_LastScaleString;

    // Copy/Paste Support
    QList<QByteArray*>      m_CopyPasteBufferListComps;
    QList<QByteArray*>      m_CopyPasteBufferListWires;
    QList<QByteArray*>      m_CopyPasteBufferListTexts;
    QList<GraphicItemBase*> m_CopyPasteWireBasesList;
    int                     m_PasteOffset;
};

#endif // MAINWINDOW_H
