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

#include "WindowComponentToolbox.h"

////////////////////////////////////////////////////////////

ComponentTreeWidget::ComponentTreeWidget(QWidget* parent /*=0*/)
    : QTreeWidget(parent)
{
    // Init Variables
    m_DraggingComponentType = COMP_UNCATEGORIZED;
    m_DraggingComponent = NULL;
    m_DraggingItem = NULL;
    m_DragStartPosition = QPointF(0,0);
}

void ComponentTreeWidget::mousePressEvent(QMouseEvent* event)
{
    // Save the mouse position for checking if
    // we are gonna do a Drag & Drop event
    if (event->button() == Qt::LeftButton) {
        m_DragStartPosition = event->pos();
    }

    // Get the Item that we are moving
    m_DraggingItem = itemAt(event->pos());
    if (m_DraggingItem != NULL) {
        m_DraggingComponent = VPtr<SSTInfoDataComponent>::asPtr(m_DraggingItem->data(0, Qt::UserRole));

        // Is this a component Item?
        if (m_DraggingComponent != NULL) {
            m_DraggingComponentType = m_DraggingComponent->GetComponentType();
        }
    }

    // Call the parent handler
    QTreeWidget::mousePressEvent(event);
}

void ComponentTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
    enum Qt::DropAction DropExecRtn;

    // Check to see if the left button is NOT down
    if (!(event->buttons() & Qt::LeftButton)) {
        QTreeWidget::mouseMoveEvent(event);
        return;
    }

    // Have we moved the mouse enough to start Drag & Drop processing
    if ((event->pos() - m_DragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        QTreeWidget::mouseMoveEvent(event);
        return;
    }

    if (m_DraggingComponent != NULL) {
        // Well the mouse was moved enough, so lets start a Drag & Drop function
        // Create drag and mime data objects & setup them up
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        mimeData->setText(DRAGDROP_COMPONENTNAME);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(GraphicItemComponent::GetComponentButtonIconImageName(m_DraggingComponentType)).scaled(25,25));

        // Perform the Drag & Drop
        DropExecRtn = drag->exec();
    }

    // Call the parent handler
    QTreeWidget::mouseMoveEvent(event);

    // Since we moved the mouse off the item and called drag, the item would
    // show a selected state, turn off selection
    if ((m_DraggingItem != NULL) && (DropExecRtn != Qt::IgnoreAction)) {
        m_DraggingItem->setSelected(false);
    }
}

void ComponentTreeWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // We let go of the mouse, reset any drag/drop settings
    m_DraggingComponentType = COMP_UNCATEGORIZED;
    m_DraggingComponent = NULL;
    m_DraggingItem = NULL;
    m_DragStartPosition = QPointF(0,0);

    QTreeWidget::mouseReleaseEvent(event);
}

////////////////////////////////////////////////////////////

WindowComponentToolBox::WindowComponentToolBox(QWidget* parent /*=0*/)
    : QFrame(parent)
{
    // Build the sub widgets inside of this frame widget

    // Create a QTreeWidget to hold components
    m_ToolboxTreeWidget = new ComponentTreeWidget();
    m_ToolboxTreeWidget->setMinimumWidth(10);
    QStringList HeaderList;
    HeaderList.append("Name");
    m_ToolboxTreeWidget->setHeaderLabels(HeaderList);
    m_ToolboxTreeWidget->setColumnCount(1);

    // Create the radio Buttons for sorting
    m_RadioSortByType = new QRadioButton("Type");
    m_RadioSortByElement = new QRadioButton("Element");

    // Initially sort by Element
    m_RadioSortByElement->setChecked(true);
    m_SortByFlag = SORTBY_ELEMENT;

    // Setup signal/handlers to handle when the Radio Buttons are clicked
    connect(m_RadioSortByType, SIGNAL(clicked(bool)), this, SLOT(HandleSortByButtonClicked(bool)));
    connect(m_RadioSortByElement, SIGNAL(clicked(bool)), this, SLOT(HandleSortByButtonClicked(bool)));

    // Setup Tree Widget Handlers
    connect(m_ToolboxTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(HandleComponentTreeItemPressed(QTreeWidgetItem*, int)));
    connect(m_ToolboxTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(HandleComponentTreeItemCollapsed(QTreeWidgetItem*)));

    // Create the Sort By Group Box
    m_SortByGroupBox = new QGroupBox(this);
    m_SortByGroupBox->setTitle("Sort By:");
    m_SortByGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));

    // Build a Horizontal layout for the m_SortByGroupBox and
    // put the Sort Radio Buttons inside of it
    QHBoxLayout* pSortByGroupBoxLayout = new QHBoxLayout();
    pSortByGroupBoxLayout->addWidget(m_RadioSortByElement);
    pSortByGroupBoxLayout->addWidget(m_RadioSortByType);
    m_SortByGroupBox->setLayout(pSortByGroupBoxLayout);

    // Create a layout for this Frame window and add the
    // Sort Group Box and Toolbox Widgets
    QVBoxLayout* pMainLayout = new QVBoxLayout();
    pMainLayout->addWidget(m_ToolboxTreeWidget);
    pMainLayout->addWidget(m_SortByGroupBox);

    // Set the layout to this window
    setLayout(pMainLayout);

    // Initialize the SSTInfoData Pointer
    m_SSTInfoData = NULL;
}

WindowComponentToolBox::~WindowComponentToolBox()
{
    if (m_SSTInfoData != NULL) {
        delete m_SSTInfoData;
        m_SSTInfoData = NULL;
    }
}

void WindowComponentToolBox::UnselectAllToolboxComponentItems()
{
    // Get the List of all selected Items
    QList<QTreeWidgetItem*> SelectedList = m_ToolboxTreeWidget->selectedItems();

    // Check to see if there are any selected items
    for (int x = 0; x < SelectedList.count() ; x++) {
        // Deselect all selected items
        SelectedList.at(x)->setSelected(false);
    }

    // Now Send a signal to the Main Window telling it that we have no item selected,
    // and the scene should do nothing if it is clicked
    emit ComponentToolboxItemPressed(NULL);
}

void WindowComponentToolBox::LoadSSTInfo(SSTInfoData* SSTInfoData, bool AddSSTConfigComponent)
{
    // Remove any pre-existing SSTInfoData
    if (m_SSTInfoData != NULL) {
        delete m_SSTInfoData;
    }

    // Point to the new SSTInfoData
    m_SSTInfoData = SSTInfoData;

    // Add the SST Configuration Component.  This is a special
    // component that SSTWorkbench adds to allow the user to
    // set global values related to the SST run.
    if (AddSSTConfigComponent == true) {
        Add_SSTConfigComponentToSSTInfo();
    }

    // Clear the toolbox
    ClearToolBox();

    // Re-Populate the ToolBox using the new sort
    PopulateToolBox();
}

void WindowComponentToolBox::ClearToolBox()
{
    int               x;
    QTreeWidgetItem*  ptrTreeWidgetItem;
    int               ItemCount;

    // Remove all Top Level Tree Widget Items
    ItemCount = m_ToolboxTreeWidget->topLevelItemCount();
    for (x = 0; x < ItemCount; x++) {
        ptrTreeWidgetItem = m_ToolboxTreeWidget->takeTopLevelItem(0);
        destroy(ptrTreeWidgetItem);
    }
}

void WindowComponentToolBox::Add_SSTConfigComponentToSSTInfo()
{
    SSTInfoDataElement* NewElem;
    SSTInfoDataComponent* NewComp;

    // Add an Element Library as a place holder
    NewElem = new SSTInfoDataElement("SST Configuration");
    NewElem->SetElementDesc("Configuration Settings for SST Runtime");
    m_SSTInfoData->AddElement(NewElem);

    // Add Components to the Element Library plus some Properties and Ports (setup for only a since allowed instance)
    NewComp = new SSTInfoDataComponent(NewElem->GetElementName(), "SST Startup Configuration", COMP_SSTSTARTUPCONFIGURATION, 1);
    NewComp->SetComponentDesc("SST Startup Configuration");
    NewElem->AddComponent(NewComp);

    // Set the Properties for the SST Startup Configuration
    // These are identified in function getProgramOptions(...) in SST's core file pymodel.cc
    NewComp->AddParam("timebase", "1ps", "Sets the base time step of the simulation.");
    NewComp->AddParam("stop-at", "", "Set time at which simulation will end execution.");
    NewComp->AddParam("partitioner", "", "Partitioner to be used <self | simple |rrobin | linear | lib.partitioner_name> (Option ignored for serial jobs).");
    NewComp->AddParam("output-partition", "", "Dump component partition to this file (default is not to dump information).");
    NewComp->AddParam("output-config", "", "Dump the SST component and link configuration graph to this file (as a Python file), empty string (default) is not to dump anything.");
    NewComp->AddParam("output-dot", "", "Dump the SST component and link graph to this file in DOT-format, empty string (default) is not to dump anything.");
    NewComp->AddParam("heartbeat-period", "", "Set time for heart beats to be published (these are approximate timings published by the core to update on progress), default is every 10000 simulated seconds");
    NewComp->AddParam("debug-file", "", "File where debug output will go.");
    NewComp->AddParam("verbose", "", "Print information about core runtimes.");
}

void WindowComponentToolBox::PopulateToolBox()
{
    SSTInfoDataElement*   ElementData;
    int                   x;
    int                   y;
    int                   z;
    SSTInfoDataComponent* Component;
    QString               ComponentName;
    QIcon                 Icon;

    // Check first to see if we have any data
    if (m_SSTInfoData == NULL) {
        return;
    }

    if (m_SortByFlag == SORTBY_ELEMENT) {
        // SORT BY ELEMENT
        // For each element in the SSTInfoData object, build a widget
        for (x = 0; x < m_SSTInfoData->GetNumOfElements(); x++) {
            // Get the Element Data
            ElementData = m_SSTInfoData->GetElement(x);

            // Only add Element data that has components
            if (ElementData->GetNumOfComponents() > 0) {

                // Add an Element to the Top Level of the Tree Library
                QTreeWidgetItem* NewElementItem = new QTreeWidgetItem(m_ToolboxTreeWidget);
                NewElementItem->setText(0, ElementData->GetElementName());
                NewElementItem->setToolTip(0, ElementData->GetElementDesc());
                NewElementItem->setFlags(NewElementItem->flags() ^ Qt::ItemIsSelectable);
                m_ToolboxTreeWidget->addTopLevelItem(NewElementItem);

                // Now for Each Component Add it to the ElementItem
                for (y = 0; y < ElementData->GetNumOfComponents(); y++)
                {
                    // Get the Component From the Element and its name
                    Component = ElementData->GetComponent(y);
                    ComponentName = Component->GetComponentName();

                    // Create a Tree Widget Item that is a subitem of the parent
                    QTreeWidgetItem* NewComponentItem = new QTreeWidgetItem(NewElementItem);
                    NewComponentItem->setText(0, ComponentName);
                    NewComponentItem->setToolTip(0, Component->GetComponentDesc());
                    Icon = QIcon (GraphicItemComponent::GetComponentButtonIconImageName(Component->GetComponentType()));
                    NewComponentItem->setIcon(0, Icon);

                    // Save a Pointer to the Component for as part of this item so we can get it later
                    NewComponentItem->setData(0, Qt::UserRole, VPtr<SSTInfoDataComponent>::asQVariant(Component));
                    // NOTE: To get the data back out from this button use:
                    //SSTInfoDataComponent* ptrComponent = VPtr<SSTInfoDataComponent>::asPtr(item->data(0, Qt::UserRole));
                }
            }
        }
    } else {
        // SORT BY TYPE
        // For each type of SSTInfoData object, build a widget
        ComponentType_enum ThisComponentType;
        for (x = 0; x < NUMCOMPONENTTYPES; x++) {
            ThisComponentType = (ComponentType_enum)x;

            // Add an Element TYPE to the Top Level of the Tree Library
            QTreeWidgetItem* NewElementItem = new QTreeWidgetItem(m_ToolboxTreeWidget);
            NewElementItem->setText(0, SSTInfoDataComponent::GetComponentTypeName(ThisComponentType));
            NewElementItem->setToolTip(0, SSTInfoDataComponent::GetComponentTypeName(ThisComponentType));
            NewElementItem->setFlags(NewElementItem->flags() ^ Qt::ItemIsSelectable);
            m_ToolboxTreeWidget->addTopLevelItem(NewElementItem);

            // For each element in the SSTInfoData object, look at each component
            for (z = 0; z < m_SSTInfoData->GetNumOfElements(); z++) {
                // Get the Element Data
                ElementData = m_SSTInfoData->GetElement(z);

                // For each component in this element, check to see if it is of the correct type,
                for (y = 0; y < ElementData->GetNumOfComponents(); y++) {

                    // Get the Component From the Element
                    Component = ElementData->GetComponent(y);

                    // Is the component type correct, if it is add it to the group
                    if (Component->GetComponentType() == ThisComponentType) {

                        // Build a Name for the button
                        ComponentName = "[" + ElementData->GetElementName() + "] "+  Component->GetComponentName();

                        // Create a Tree Widget Item that is a subitem of the parent
                        QTreeWidgetItem* NewComponentItem = new QTreeWidgetItem(NewElementItem);
                        NewComponentItem->setText(0, ComponentName);
                        NewComponentItem->setToolTip(0, Component->GetComponentDesc());
                        Icon = QIcon (GraphicItemComponent::GetComponentButtonIconImageName(Component->GetComponentType()));
                        NewComponentItem->setIcon(0, Icon);

                        // Save a Pointer to the Component for as part of this item so we can get it later
                        NewComponentItem->setData(0, Qt::UserRole, VPtr<SSTInfoDataComponent>::asQVariant(Component));
                        // NOTE: To get the data back out from this button use:
                        //SSTInfoDataComponent* ptrComponent = VPtr<SSTInfoDataComponent>::asPtr(item->data(0, Qt::UserRole));
                    }
                }
            }
        }
    }
}

void WindowComponentToolBox::HandleSortByButtonClicked(bool checked)
{
    Q_UNUSED(checked)

    // The user has clicked a Sort By Button, Figure out what
    // Button was pressed and change the sort type.
    if (m_RadioSortByElement->isChecked()) {
        m_SortByFlag = SORTBY_ELEMENT;
    } else {
        m_SortByFlag = SORTBY_TYPE;
    }

    // Clear the toolbox
    ClearToolBox();

    // Re-Populate the ToolBox using the new sort
    PopulateToolBox();
}

void WindowComponentToolBox::HandleComponentTreeItemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    // THIS HANDLER IS CALLED WHEN A Tree Item IN THE COMPONENT TOOLBOX IS CLICKED
    // Toggle selection on/off
    // Now Get the pointer to the SSTInfoDataComponent from the item
    SSTInfoDataComponent* ptrComponent = VPtr<SSTInfoDataComponent>::asPtr(item->data(0, Qt::UserRole));

    // Now Send a signal to the Main Window telling it what component was clicked
    // NOTE: If this is an element entry (top level of tree) then the ptrComponent will be NULL
    emit ComponentToolboxItemPressed(ptrComponent);
}

void WindowComponentToolBox::HandleComponentTreeItemCollapsed(QTreeWidgetItem* item)
{
    Q_UNUSED(item)

    // Unselect all Components when any top level item is collapsedee
    UnselectAllToolboxComponentItems();
}

