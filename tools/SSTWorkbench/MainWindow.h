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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GlobalIncludes.h"

#include "WiringScene.h"
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
    void UpdateDisplayBasedUponDirtyStatus();

    // Set the Wiring Window Tab Title
    void SetMainTabTitle(QString Title);
    void SetMainTabTitle(QFileInfo FileInfo);

    // Info on the scene
    bool IsSceneEmpty();
    bool AreSceneComponentsSelected();

    // Set the Font
    void SetFontControls(const QFont& font);

    // Control the Scene display
    void SetSceneScaleByPercent(double ScalePercent);
    void UpdateSceneScaleDisplay();

    // Handling of Persistant Storage
    void SavePersistentStorage();
    void RestorePersistentStorage();

    // Handling of Project Data (Serialization)
    bool SaveProjectData(QString ProjectFilePathName);
    bool LoadProjectData(QString ProjectFilePathName);

    // Get access to the Persistent Settings object
    QSettings* GetPersistentSettings() {return m_PersistentSettings;}

    // Do we have SSTInfoData available
    bool IsSSTInfoDataLoaded();

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

private slots:
    // Handlers User Actions From Component, Properties or SceneView Windows
    void UserActionComponentToolboxButtonPressed(SSTInfoDataComponent* ptrComponent);
    void UserActionPointerGroupClicked(int id);

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

    // Handlers for User Actions From Menus or Toolbars
    void UserActionHandlerBringToFront();
    void UserActionHandlerSendToBack();
    void UserActionHandlerDeleteItem();

    void UserActionHandlerSelectAll();

    void UserActionHandlerCut();
    void UserActionHandlerCopy();
    void UserActionHandlerPaste();

    void UserActionHandlerNewProject();
    void UserActionHandlerLoadDesign();
    void UserActionHandlerSaveDesign();
    void UserActionHandlerSaveAs();
    void UserActionHandlerImportSSTInfo();
    void UserActionHandlerExportSSTInputDeck();
    void UserActionHandlerPreferences();
    void UserActionHandlerAbout();
    void UserActionHandlerPrint();
    void UserActionHandlerExit();
    void UserActionHandlerShowToolbars();

    void UserActionFontSelectChanged(const QString& font);
    void UserActionFontSizeChanged(const QString& size);
    void UserActionHandlerFontChange();

    void UserActionSceneScaleChanged(const QString& NewScaleText);
    void UserActionSceneScaleZoomIn();
    void UserActionSceneScaleZoomOut();
    void UserActionSceneScaleZoomAll();

    void UserActionHandlerMovePorts();
    void UserActionHandlerSetDynamicPorts();
    void UserActionHandlerManageModules();

    void UserActionTextColorButtonTriggered();
    void UserActionComponentFillColorButtonTriggered();
    void UserActionTextColorChanged();
    void UserActionComponentFillColorChanged();

    void ClearCopyPasteBuffer();

private:
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* ev);

private:
    // The Scene, View, Component, Property, Dialogs & Support Windows
    WiringScene*            m_WiringScene;
    QGraphicsView*          m_WiringView;
    WindowComponentToolBox* m_CompToolBox;
    WindowItemProperties*   m_PropWin;
    DialogPortsConfig*      m_ConfigurePortsDialog;
    DialogManageModules*    m_ManageModulesDialog;
    DialogPreferences*      m_PreferencesDialog;
    QSplitter*              m_MainSplitterWidget;
    QTabWidget*             m_TabWiringWindow;

    // Undo / Redo Processing
    QUndoStack*             m_UndoStack;
//  QUndoView*              m_UndoViewWindow;

    // Menus for the Main Window
    QMenu*                  m_FileMenu;
    QMenu*                  m_EditMenu;
    QMenu*                  m_GenericItemMenu;
    QMenu*                  m_ViewMenu;
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
    QButtonGroup*           m_PointerTypeGroup;
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
    QString                 m_SSTInfoXMLDataFilePathName;

    // Project File Data (Project Open/Save file)
    QString                 m_LastSavedProjectDataFilePathName;
    QString                 m_LoadedProjectDataFilePathName;
    bool                    m_ProjectIsDirty;
    QFont                   m_DefaultFont;
    QString                 m_LastFontSizeString;
    QString                 m_LastScaleString;

    // Python Export
    QString                 m_LastExportedPythonFilePathName;

    // Preferences
    bool                    m_ReturnToSelectToolAfterPlacingWire;
    bool                    m_ReturnToSelectToolAfterPlacingText;

    // Copy/Paste Support
    QList<QByteArray*>      m_CopyPasteBufferListComps;
    QList<QByteArray*>      m_CopyPasteBufferListWires;
    QList<QByteArray*>      m_CopyPasteBufferListTexts;
    QList<GraphicItemBase*> m_CopyPasteWireBasesList;
    int                     m_PasteOffset;
};

#endif // MAINWINDOW_H
