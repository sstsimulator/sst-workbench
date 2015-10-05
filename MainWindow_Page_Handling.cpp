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
//       The routines here are for the MainWindow Page processing.
//       The files are separated to reduce single file code bloat.
////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// NOTE: The Page Names list contains the list of available
//       page names (tabs) that can be opened/closed, renamed, and deleted
//       The indexes in the list match the corresponding Graphic Scene/View
//       Tab index does not represent a specific page because some tabs may
//       not be visible, but all pages are always available to be displayed
//       via a tab.
/////////////////////////////////////////////////////////////////////////////

int MainWindow::GetPageIndexFromName(QString PageName)
{
    return m_PageNamesList.indexOf(PageName);
}

QString MainWindow::GetPageNameFromUser(bool NewTabFlag, QString DefaultText /*=""*/ ) {
    bool    BtnOK;
    QString TabName;
    QString Title;
    QString MainText;

    Title = "New Page";
    MainText = "Name of new Page:";
    if (NewTabFlag == false) {
        Title = "Rename Page";
        MainText = "Rename Page:";
    }

    TabName = QInputDialog::getText(this, Title, MainText, QLineEdit::Normal,  DefaultText, &BtnOK);

    return TabName;
}

bool MainWindow::IsPageNameRedundant(QString PageName)
{
    // Check for redundant Page Names
    if (m_PageNamesList.count(PageName) >= 1) {
        return true;
    }
    return false;
}

void MainWindow::UpdatePageTabUIInformation()
{
    int      x;
    int      y;
    QAction* NewAction;

    // Allow tabs to be closeable if count > 1
    m_TabWiringWindow->setTabsClosable(m_TabWiringWindow->count() > 1);
    m_PageClose->setEnabled(m_TabWiringWindow->count() > 1);
    m_PageDelete->setEnabled(m_TabWiringWindow->count() > 1);

    // Populate the PageOpen menu with the list of available pages
    m_PageOpenMenu->clear();  // Clear out all sub-menu items
    for (x = 0; x < m_PageNamesList.count(); x++) {
        NewAction = m_PageOpenMenu->addAction(m_PageNamesList.at(x), this, SLOT(MenuActionPageOpen()));
        NewAction->setEnabled(true);

        // See if this page has a displayed tab
        for (y = 0; y < m_TabWiringWindow->count(); y++) {
            if (m_TabWiringWindow->tabText(y) == m_PageNamesList.at(x)) {
                // The tab is open, so put a checkmark by the Coresponding menu item
                NewAction->setEnabled(false);
                NewAction->setCheckable(true);
                NewAction->setChecked(true);
            }
        }
    }
}

int MainWindow::CreateNewSceneViewAndPutIntoLists()
{
    // Build the Wiring Scene and set its initial size (of the scene window, not the viewport)
    WiringScene* NewWiringScene = new WiringScene(m_GenericItemMenu, m_UndoStack, this);
    NewWiringScene->setSceneRect(QRectF(0, 0, 5000, 5000));

    // Add the new Scene to the ScenesList
    m_WiringScenesList.append(NewWiringScene);

    // Build the view object for the wiring scene
    WiringView* NewWiringView = new WiringView(NewWiringScene);

    // Add the new View to the ViewsList
    m_WiringViewsList.append(NewWiringView);

    // Connect Common signals to all scenes
    connect(this, SIGNAL(PreferenceChangedAutoDeleteTooShortWire(bool)),  NewWiringScene, SLOT(HandlePreferenceChangeAutoDeleteTooShortWire(bool)));

    return m_WiringViewsList.count() - 1;
}

void MainWindow::DeletePageSceneView(int PageIndex)
{
    // Remove the PageName, View and Scene from the lists
    WiringScene* RemovedScene = m_WiringScenesList.at(PageIndex);
    WiringView*  RemovedView  = m_WiringViewsList.at(PageIndex);
    delete RemovedScene;
    delete RemovedView;
    m_WiringScenesList.removeAt(PageIndex);
    m_WiringViewsList.removeAt(PageIndex);
    m_PageNamesList.removeAt(PageIndex);
}

void MainWindow::MenuActionPageOpen()
{
    int      CurrentPageIndex;
    int      NewTabIndex;
    QAction* pAction;
    QString  PageName;

    // Find the Calling Action and get its Text Name and if it is already open
    pAction = qobject_cast<QAction*>(sender());
    PageName = pAction->text();

    // We need to create a new tab with the name of the page with the correct view
    // Get the Page Index
    CurrentPageIndex = GetPageIndexFromName(PageName);

    // Create the Tab that will house a single QGraphics View object at end (user can change tab position if desired)
    NewTabIndex = m_TabWiringWindow->insertTab(999, m_WiringViewsList.at(CurrentPageIndex), PageName);

    // Select the new tab
    m_TabWiringWindow->setCurrentIndex(NewTabIndex);

    // Update all the UI information for pages and tabs
    UpdatePageTabUIInformation();
}

void MainWindow::MenuActionPageClose(int TabIndex /*=-1*/)
{
    int     CurrentTabIndex;

    // If no TabIndex is not filled, then use the current page
    if (TabIndex == -1) {
        // Get the general informaton on the current tab and its Page Index
        CurrentTabIndex = m_TabWiringWindow->currentIndex();
    } else {
        CurrentTabIndex = TabIndex;
    }

    m_TabWiringWindow->removeTab(CurrentTabIndex);

    // Update all the UI information for pages and tabs
    UpdatePageTabUIInformation();
}

void MainWindow::MenuActionPageRename(QString ProvidedPageName /*=""*/)
{
    int     CurrentTabIndex;
    int     CurrentPageIndex;
    QString CurrentTabText;
    QString NewPageName;

    // Get the general informaton on the current tab and its Page Index
    CurrentTabIndex = m_TabWiringWindow->currentIndex();
    CurrentTabText = m_TabWiringWindow->tabText(CurrentTabIndex);
    CurrentPageIndex = GetPageIndexFromName(CurrentTabText);

    if (ProvidedPageName.isEmpty() == true) {
        // Get the new page name from the user
        NewPageName = GetPageNameFromUser(false, CurrentTabText);
    } else {
        NewPageName =  ProvidedPageName;
    }

    // if the page name did not change, do nothing
    if (NewPageName == CurrentTabText) {
        return;
    }

    // Check the new Page Name to see if it is redundant
    if (IsPageNameRedundant(NewPageName) == true) {
        QMessageBox::warning(this, "Duplicate Page Name", QString("Cannot rename page \"%1\" to \"%2\"; a page with that name already exists.").arg(CurrentTabText).arg(NewPageName));
        return;
    }

    // if NewPageName is empty, don't do anything
    if (NewPageName.isEmpty() == true) {
        return;
    }

    m_TabWiringWindow->setTabText(CurrentTabIndex, NewPageName);
    m_PageNamesList[CurrentPageIndex] = NewPageName;

    // Update all the UI information for pages and tabs
    UpdatePageTabUIInformation();
}

void MainWindow::MenuActionPageNew(QString ProvidedPageName /*=""*/)
{
    QString NewPageName;
    int     NewTabIndex;
    int     NewPageIndex;

    // Do we need to get a name from the user
    if (ProvidedPageName.isEmpty() == true) {
        // Get the new page name
        NewPageName = GetPageNameFromUser(true);

        // if empty, don't do anything
        if (NewPageName.isEmpty() == true) {
            return;
        }
    } else {
        NewPageName = ProvidedPageName;
    }

    // Check the new Page Name
    if (IsPageNameRedundant(NewPageName) == true) {
        QMessageBox::warning(this, "Duplicate Page Name", QString("Cannot create page \"%1\"; a page with that name already exists.").arg(NewPageName));
        return;
    }

    // Build the new Wiring Scene and Views and put thiem into the appropriate lists
    NewPageIndex = CreateNewSceneViewAndPutIntoLists();

    // Add the NewPage to the list of Page names
    m_PageNamesList.append(NewPageName);

    // Set the current Scene and View to the new selected tab
    m_CurrentWiringScene = m_WiringScenesList.at(NewPageIndex);
    m_CurrentWiringView  = m_WiringViewsList.at(NewPageIndex);

    // Create the Tab that will house a single QGraphics View object
    NewTabIndex = m_TabWiringWindow->addTab(m_WiringViewsList.at(NewPageIndex), NewPageName);

    // Select the new tab
    m_TabWiringWindow->setCurrentIndex(NewTabIndex);

    // Update all the UI information for pages and tabs
    UpdatePageTabUIInformation();
}

void MainWindow::MenuActionPageDelete(int TabIndex /*=-1*/)
{
    int     CurrentTabIndex;
    int     CurrentPageIndex;
    QString CurrentTabText;
    QString NewPageName;

    // If no TabIndex is not filled, then use the current page
    if (TabIndex == -1) {
        // Get the general informaton on the current tab and its Page Index
        CurrentTabIndex = m_TabWiringWindow->currentIndex();
    } else {
        CurrentTabIndex = TabIndex;
    }

    // Get the general informaton on the current tab and its Page Index
    CurrentTabText = m_TabWiringWindow->tabText(CurrentTabIndex);
    CurrentPageIndex = GetPageIndexFromName(CurrentTabText);

    // Ask the user if they want to if the call came from a menu item
    if (TabIndex == -1) {
        if (QMessageBox::question(NULL, "Are you Sure?", "Are you sure you want to delete this page?  This can lead to unconnected wires (between pages) and cannot be undone") == QMessageBox::No) {
            return;
        }
    }

    // Close this tab
    MenuActionPageClose(CurrentTabIndex);

    DeletePageSceneView(CurrentPageIndex);

    // Select the CurrentTabIndex - 1 as the new selected tab, this will cause a new current scene and views to be set
    m_TabWiringWindow->setCurrentIndex(CurrentTabIndex - 1);

    // Update all the UI information for pages and tabs
    UpdatePageTabUIInformation();
}

void MainWindow::HandleTabWindowEventTabChanged(int TabIndex)
{
    Q_UNUSED(TabIndex)

    int     CurrentTabIndex;
    int     CurrentPageIndex;
    QString CurrentTabText;

    // Shutdown all selected items on this current scene
    m_CurrentWiringScene->SetNothingSelected();

    // Disconnect all signals from the old scene
    disconnect(m_CurrentWiringScene);

    // Get the general informaton on the current tab and its Page Index
    CurrentTabIndex = m_TabWiringWindow->currentIndex();
    CurrentTabText = m_TabWiringWindow->tabText(CurrentTabIndex);
    CurrentPageIndex = GetPageIndexFromName(CurrentTabText);

    // Set the current Scene and View to the new selected tab
    m_CurrentWiringScene = m_WiringScenesList.at(CurrentPageIndex);
    m_CurrentWiringView  = m_WiringViewsList.at(CurrentPageIndex);

    // Connect Scene Event Signals to Main Window Handlers
    connect(m_CurrentWiringScene, SIGNAL(SceneEventComponentAdded(GraphicItemComponent*)),             this, SLOT(HandleSceneEventComponentAdded(GraphicItemComponent*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventTextAdded(GraphicItemText*)),                       this, SLOT(HandleSceneEventTextAdded(GraphicItemText*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventWireAddedInitialPlacement(GraphicItemWire*)),       this, SLOT(HandleSceneEventWireAddedInitialPlacement(GraphicItemWire*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventWireAddedFinalPlacement(GraphicItemWire*)),         this, SLOT(HandleSceneEventWireAddedFinalPlacement(GraphicItemWire*)));
    connect(m_CurrentWiringScene, SIGNAL(selectionChanged()),                                          this, SLOT(HandleSceneEventSelectionChanged()));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventGraphicItemSelected(QGraphicsItem*)),               this, SLOT(HandleSceneEventGraphicItemSelected(QGraphicsItem*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventGraphicItemSelectedProperties(ItemProperties*)),    this, SLOT(HandleSceneEventGraphicItemSelectedProperties(ItemProperties*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventSetProjectDirty()),                                 this, SLOT(HandleSceneEventSetProjectDirty()));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventRefreshPropertiesWindowProperty(QString, QString)), this, SLOT(HandleSceneEventRefreshPropertiesWindowProperty(QString, QString)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventRefreshPropertiesWindow(ItemProperties*)),          this, SLOT(HandleSceneEventRefreshPropertiesWindow(ItemProperties*)));
    connect(m_CurrentWiringScene, SIGNAL(SceneEventDragAndDropFinished()),                             this, SLOT(HandleSceneEventDragAndDropFinished()));

    // Set the Scale to the current views scale value
    SetCurrentViewScaleByPercent(m_CurrentWiringView->GetCurrentScalePercent());
}


void MainWindow::HandleTabWindowEventTabCloseRequested(int TabIndex)
{
    MenuActionPageClose(TabIndex);
}

