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

#include "WindowComponentToolbox.h"

////////////////////////////////////////////////////////////

ComponentToolButton::ComponentToolButton(SSTInfoDataComponent::ComponentType ComponentType, QWidget* parent /*=0*/)
    : QToolButton(parent)
{
    // Init Variables
    m_ComponentType = ComponentType;
    m_DragStartPosition = QPointF(0,0);
}

void ComponentToolButton::mousePressEvent(QMouseEvent* event)
{
    // Save the mouse position for checking if
    // we are gonna do a Drag & Drop event
    if (event->button() == Qt::LeftButton) {
        m_DragStartPosition = event->pos();
    }

    // Call the parent handler
    QToolButton::mousePressEvent(event);
}

void ComponentToolButton::mouseMoveEvent(QMouseEvent* event)
{
    // Check to see if the left buttone is NOT down
    if (!(event->buttons() & Qt::LeftButton)) {
        QToolButton::mouseMoveEvent(event);
        return;
    }

    // Have we moved the mouse enough to start Drag & Drop processing
    if ((event->pos() - m_DragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        QToolButton::mouseMoveEvent(event);
        return;
    }

    // Well the mouse was moved enough, so lets start a Drag & Drop function
    // Create drag and mime data objects & setup them up
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(DRAGDROP_COMPONENTNAME);
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(GraphicItemComponent::GetComponentButtonIconImageName(m_ComponentType)));

    // Perform the Drag & Drop
    drag->exec();

    // Call the parent handler
    QToolButton::mouseMoveEvent(event);

    // Since we moved the mouse off the button and called drag, the button would
    // show a down state and not recover.  So turn off the down state.
    setDown(false);
}

////////////////////////////////////////////////////////////

ComponentToolBox::ComponentToolBox(QWidget* parent/*=0*/)
    : QToolBox(parent)
{
}

void ComponentToolBox::resizeEvent(QResizeEvent* event)
{
    QToolBox::resizeEvent(event);

    // Fire the event telling the Main window that the Toolbox has been resized
    emit ComponentToolboxResized();
}

////////////////////////////////////////////////////////////

WindowComponentToolBox::WindowComponentToolBox(QWidget* parent /*=0*/)
    : QFrame(parent)
{
    // Build the sub widgets inside of this frame widget

    // Create the Toolbox Widget
    m_ToolboxWidget = new ComponentToolBox();
    m_ToolboxWidget->setMinimumWidth(10);

    // Create the radio Buttons for sorting
    m_RadioSortByType = new QRadioButton("Type");
    m_RadioSortByElement = new QRadioButton("Element");

    // Initially sort by Element
    m_RadioSortByElement->setChecked(true);
    m_SortByFlag = SORTBY_ELEMENT;

    // Setup signal/handlers to handle when the Radio Buttons are clicked
    connect(m_RadioSortByType, SIGNAL(clicked(bool)), this, SLOT(HandleSortByButtonClicked(bool)));
    connect(m_RadioSortByElement, SIGNAL(clicked(bool)), this, SLOT(HandleSortByButtonClicked(bool)));

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

    // Build a scroll area for the Toolbox Widget
    QScrollArea* pComponentScrollArea = new QScrollArea();
    pComponentScrollArea->setWidget(m_ToolboxWidget);
    pComponentScrollArea->setWidgetResizable(true);
    pComponentScrollArea->setFrameShape(QFrame::WinPanel);
    pComponentScrollArea->setFrameShadow(QFrame::Sunken);
    pComponentScrollArea->setMinimumHeight(200);

    // Create a layout for this Frame window and add the
    // Sort Group Box and Toolbox Widgets
    QVBoxLayout* pMainLayout = new QVBoxLayout();
    pMainLayout->addWidget(pComponentScrollArea);
    pMainLayout->addWidget(m_SortByGroupBox);

    // Set the layout to this window
    setLayout(pMainLayout);

    // Initialize the SSTInfoData Pointer
    m_SSTInfoData = NULL;
    m_CurrentButtonGroup = NULL;

    // Setup a signal/handler to notify the main window that a component has been selected
    connect(this, SIGNAL(ComponentToolboxButtonPressed(SSTInfoDataComponent*)), this->parent(), SLOT(UserActionComponentToolboxButtonPressed(SSTInfoDataComponent*)));

    // Setup the Resize Component ToolBox handler
    connect(m_ToolboxWidget, SIGNAL(ComponentToolboxResized()), this, SLOT(HandleComponentToolboxResized()));
}

WindowComponentToolBox::~WindowComponentToolBox()
{
    if (m_SSTInfoData != NULL) {
        delete m_SSTInfoData;
        m_SSTInfoData = NULL;
    }
}

void WindowComponentToolBox::UncheckAllCurrentGroupButtons()
{
    // Make sure there is a CurrentButtonGroup Selected
    if (GetCurrentButtonGroup() != NULL) {
        // Get the list of Component Toolbox Buttons available
        QList<QAbstractButton*> ComponentToolboxButtons = GetCurrentButtonGroup()->buttons();

        // Before updating the CurrentButtonGroup
        // Turn off the checked setting for all other buttons
        foreach (QAbstractButton* button, ComponentToolboxButtons) {
            button->setChecked(false);
        }
    }

    // Now Send a signal to the Main Window telling it that we have no button selected,
    // and the scene should do nothing if it is clicked
    emit ComponentToolboxButtonPressed(NULL);
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

    //Re-Populate the ToolBox using the new sort
    PopulateToolBox();
}

void WindowComponentToolBox::ClearToolBox()
{
    int           x;
    QWidget*      ptrWidget;
    QButtonGroup* ptrButtonGroup;
    int           NumberOfTabs;

    // Disconnect any handler if the Button Group Changes
    disconnect(m_ToolboxWidget, SIGNAL(currentChanged(int)), 0, 0);

    // Get the number of button groups (tabs)
    NumberOfTabs = m_ToolboxWidget->count();

    // Remove all widgets from the ToolboxWidget and all button groups
    // from the ButtonGroupList
    for (x = 0; x < NumberOfTabs; x++) {
        ptrWidget = m_ToolboxWidget->widget(0);
        ptrButtonGroup = m_ButtonGroupList.at(0);

        m_ToolboxWidget->removeItem(0);
        m_ButtonGroupList.removeAt(0);

        delete ptrWidget;
        delete ptrButtonGroup;
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

    // Add Components to the Element Library plus some Parameters and Ports (setup for only a since allowed instance)
    NewComp = new SSTInfoDataComponent(NewElem->GetElementName(), "SST Startup Configuration", SSTInfoDataComponent::COMP_SSTSTARTUPCONFIGURATION, 1);
    NewComp->SetComponentDesc("SST Startup Configuration");
    NewElem->AddComponent(NewComp);

    // Set the Parameters for the SST Startup Configuration
    NewComp->AddParam("timebase", "1ps", "Sets the base time step of the simulation.");
    NewComp->AddParam("stopAt", "", "Set time at which simulation will end execution.");
    NewComp->AddParam("partitioner", "", "Partitioner to be used <self | simple |rrobin | linear | lib.partitioner_name> (Option ignored for serial jobs).");
    NewComp->AddParam("dump-partition", "", "Dump component partition to this file (default is not to dump information).");
    NewComp->AddParam("output-config", "", "Dump the SST component and link configuration graph to this file (as a Python file), empty string (default) is not to dump anything.");
    NewComp->AddParam("output-dot", "", "Dump the SST component and link graph to this file in DOT-format, empty string (default) is not to dump anything.");
    NewComp->AddParam("output-directory", "", "Controls where SST will place output files including debug output and simulation statistics, default is for SST to create a unique directory.");
}

QWidget* WindowComponentToolBox::CreateItemComponentButtonWidget(QButtonGroup* ButtonGroup, const QString& text, SSTInfoDataComponent* Component)
{
    // THIS ROUTINE WILL CREATE AN ACTUAL TOOLBUTTON AND ADD IT TO
    // THE BUTTON GROUP.  IT WILL ALSO CREATE A SEPARATE VISUAL
    // REPRESENTATION OF THE BUTTON (WITH ICON AND TEXT) AS A WIDGET
    // FOR THE GRAPHICAL LAYOUT.

    // Get the icon for the ComponentItem (Static Function)
    QIcon icon(GraphicItemComponent::GetComponentButtonIconImageName(Component->GetComponentType()));

    // Now build a button for the button group
    ComponentToolButton* NewButton = new ComponentToolButton(Component->GetComponentType());
    NewButton->setIcon(icon);
    NewButton->setIconSize(QSize(25, 25));
    NewButton->setCheckable(true);

    // Save a Pointer to the Component for as part of this button so we can get it later
    NewButton->setProperty("PTRCOMPONENT", VPtr<SSTInfoDataComponent>::asQVariant(Component));
    // NOTE: To get the data back out from this button use:
    // SSTInfoDataComponent* ptrComponent = VPtr<SSTInfoDataComponent>::asPtr(button->property("PTRCOMPONENT"));

    // Add it to the ButtonGroup
    ButtonGroup->addButton(NewButton);

    // Now create a layout with the Button and the text together
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(NewButton, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    // Set the spacing between the Button and text
    // and set the surrounding margins
    layout->setContentsMargins(2,2,2,2);
    layout->setSpacing(2);

    // Create the return widget and set it to our layout
    QWidget* RtnWidget = new QWidget;
    RtnWidget->setLayout(layout);

    // Prevent this new Button Widget from being resized by the layout engine
    RtnWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

    return RtnWidget;
}

QWidget* WindowComponentToolBox::CreateButtonGroupWidgetByElement(SSTInfoDataElement* ElementData)
{
    int                   x;
    SSTInfoDataComponent* Component;
    QString               ComponentName;
    QWidget*              NewButtonWidget;
    int                   x_Pos = 0;
    int                   y_Pos = 0;

    // THIS ROUTINE WILL BUILD A BUTTON GROUP FOR THE ELEMENT AND A SEPERATE BUTTON WILL BE
    // CREATED FOR EACH COMPONENT IN THE ELEMENT AND ADDED TO THE BUTTON GROUP.

    // Build a Group for the Component Buttons, This allows us handle them as a collection
    // The button group is a virtual grouping of buttons.
    QButtonGroup* NewButtonGroup = new QButtonGroup(this);
    NewButtonGroup->setExclusive(false);  // Allow buttons to not be mutually exclusive, mut. exclusion Done in handlers

    // Setup a signal that notifies when a button from the this group is pressed down
    connect(NewButtonGroup, SIGNAL(buttonPressed(int)), this, SLOT(HandleComponentToolboxButtonGroupPressed(int)));
    connect(NewButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(HandleComponentToolboxButtonGroupToggled(int, bool)));

    // Create the layout for the Buttons in the group
    // The layout is the visual representation of the buttons in the button group
    QGridLayout* GroupLayout = new QGridLayout;

    // Now for Each Component in the element Add a button for it
    for (x = 0; x < ElementData->GetNumOfComponents(); x++)
    {
        // Get the Component From the Element
        Component = ElementData->GetComponent(x);

        // Build a Name for the button
        ComponentName = Component->GetComponentName();

        // Now create a widget that represents the Component (actual button + text) and add the actual button to the button group
        NewButtonWidget = CreateItemComponentButtonWidget(NewButtonGroup, ComponentName, Component);

        // Add the NewButtonWidget to the Layout
        GroupLayout->addWidget(NewButtonWidget, x_Pos, y_Pos++);

        // Check to see if we need to reset the x_Pos or y_Pos
        if (y_Pos >= TOOLBOX_INITNUMBUTTONSACROSS) {
            x_Pos++;    // Increment the x_Pos
            y_Pos = 0;  // Reset the y_Pos
        }
    }

    // Add the NewButtonGroup to our list list of button groups so we can access them easily
    m_ButtonGroupList.append(NewButtonGroup);

    // Set layout Parameters
    // Always make last unused row stretch to fill the area
    GroupLayout->setRowStretch(x_Pos + 1, 10);

    // Create the return widget and set it to our layout
    QWidget* RtnWidget = new QWidget();
    RtnWidget->setLayout(GroupLayout);
//  RtnWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

    return RtnWidget;
}

QWidget* WindowComponentToolBox::CreateButtonGroupWidgetByComponentType(SSTInfoDataComponent::ComponentType Type)
{
    int                   x;
    int                   y;
    SSTInfoDataElement*   ElementData;
    SSTInfoDataComponent* Component;
    QString               ComponentName;
    QWidget*              NewButtonWidget;
    int                   x_Pos = 0;
    int                   y_Pos = 0;

    // THIS ROUTINE WILL BUILD A BUTTON GROUP FOR THE ELEMENT AND A SEPERATE BUTTON WILL BE
    // CREATED FOR EACH TYPE OF COMPONENT IN ALL THE ELEMENTS AND ADDED TO THE BUTTON GROUP.

    // Build a Group for the Component Buttons, This allows us handle them as a collection
    // The button group is a virtual grouping of buttons.
    QButtonGroup* NewButtonGroup = new QButtonGroup(this);
    NewButtonGroup->setExclusive(false);  // Allow buttons to not be mutually exclusive, mut. exclusion Done in handlers

    // Setup a signal that notifies when a button from the this group is pressed down
    connect(NewButtonGroup, SIGNAL(buttonPressed(int)), this, SLOT(HandleComponentToolboxButtonGroupPressed(int)));
    connect(NewButtonGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(HandleComponentToolboxButtonGroupToggled(int, bool)));

    // Create the layout for the Buttons in the group
    // The layout is the visual representation of the buttons in the button group
    QGridLayout* GroupLayout = new QGridLayout;

    // For each element in the SSTInfoData object, look at each component
    for (x = 0; x < m_SSTInfoData->GetNumOfElements(); x++) {
        // Get the Element Data
        ElementData = m_SSTInfoData->GetElement(x);

        // For each component in this element, check to see if it is of the correct type,
        for (y = 0; y < ElementData->GetNumOfComponents(); y++) {

            // Get the Component From the Element
            Component = ElementData->GetComponent(y);

            // Is the component type correct, if it is add it to the group
            if (Component->GetComponentType() == Type) {

                // Build a Name for the button
                ComponentName = "[" + ElementData->GetElementName() + "]\n"+  Component->GetComponentName();

                // Now create a widget that represents the Component (actual button + text) and add the actual button to the button group
                NewButtonWidget = CreateItemComponentButtonWidget(NewButtonGroup, ComponentName, Component);

                // Add the NewButtonWidget to the Layout
                GroupLayout->addWidget(NewButtonWidget, x_Pos, y_Pos++);

                // Check to see if we need to reset the x_Pos or y_Pos
                if (y_Pos >= TOOLBOX_INITNUMBUTTONSACROSS) {
                    x_Pos++;    // Increment the x_Pos
                    y_Pos = 0;  // Reset the y_Pos
                }
            }
        }
    }

    // Add the NewButtonGroup to our list list of button groups so we can access them easily
    m_ButtonGroupList.append(NewButtonGroup);

    // Set layout Parameters
    // Always make last unused row stretch to fill the area
    GroupLayout->setRowStretch(x_Pos + 1, 10);

    // Create the return widget and set it to our layout
    QWidget* RtnWidget = new QWidget();
    RtnWidget->setLayout(GroupLayout);

    return RtnWidget;
}

void WindowComponentToolBox::PopulateToolBox()
{
    SSTInfoDataElement* ElementData;
    QWidget*            CreatedWidget;
    int                 SmallestButtonGroupWidth = 10000;

    // Check first to see if we have any data
    if (m_SSTInfoData == NULL) {
        return;
    }

    // Disconnect any handler if the Button Group Changes
    disconnect(m_ToolboxWidget, SIGNAL(currentChanged(int)), 0, 0);

    if (m_SortByFlag == SORTBY_ELEMENT) {
        // SORT BY ELEMENT
        // For each element in the SSTInfoData object, build a widget
        for (int x = 0; x < m_SSTInfoData->GetNumOfElements(); x++) {
            // Get the Element Data
            ElementData = m_SSTInfoData->GetElement(x);

            // Only add Element data that has components
            if (ElementData->GetNumOfComponents() > 0) {

                // Create the Widget containing the Button Group
                CreatedWidget = CreateButtonGroupWidgetByElement(ElementData);

                // Figure out if this button group is the narrowest
                if (SmallestButtonGroupWidth > CreatedWidget->sizeHint().width()) {
                    SmallestButtonGroupWidth = CreatedWidget->sizeHint().width();
                }

                // Add the Button Group to the Toolbox widget
                m_ToolboxWidget->addItem(CreatedWidget, ElementData->GetElementName());
            }
        }
    } else {
        // SORT BY TYPE
        // For each type of SSTInfoData object, build a widget
        SSTInfoDataComponent::ComponentType ThisComponentType;
        for (int x = 0; x < NUMCOMPONENTTYPES; x++) {
            ThisComponentType = (SSTInfoDataComponent::ComponentType)x;

            // Create the Widget containing the Button Group
            CreatedWidget = CreateButtonGroupWidgetByComponentType(ThisComponentType);

            // Figure out if this button group is the narrowest
            if (SmallestButtonGroupWidth > CreatedWidget->sizeHint().width()) {
                SmallestButtonGroupWidth = CreatedWidget->sizeHint().width();
            }

            // Add the Button Group to the Toolbox widget
            m_ToolboxWidget->addItem(CreatedWidget, SSTInfoDataComponent::GetComponentTypeName(ThisComponentType));
        }
    }

    // Setup the size of the Toolbox
    m_ToolboxWidget->setMinimumWidth(SmallestButtonGroupWidth);
//  m_ToolboxWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));

    // Initialize the CurrentButtonGroup to the first one
    m_CurrentButtonGroup = m_ButtonGroupList.at(0);

    // Setup a handler if a Button Group Changes (User selects a different tab)
    connect(m_ToolboxWidget, SIGNAL(currentChanged(int)), this, SLOT(HandleChangedButtonGroup(int)));
}

void WindowComponentToolBox::HandleComponentToolboxButtonGroupPressed(int id)
{
    // THIS HANDLER IS CALLED WHEN A BUTTON IN THE COMPONENT TOOLBOX IS PRESSED DOWN

    // Get the list of Component Toolbox Buttons available
    QList<QAbstractButton*> ComponentToolboxButtons = GetCurrentButtonGroup()->buttons();

    // Get the Actual button that was pressed
    QToolButton* ClickedButton = (QToolButton*)(GetCurrentButtonGroup()->button(id));

    // Turn off the checked setting for all other buttons
    foreach (QAbstractButton* button, ComponentToolboxButtons) {
        if (GetCurrentButtonGroup()->button(id) != button) {
            button->setChecked(false);
        }
    }

    // Now Get the pointer to the SSTInfoDataComponent from the Button
    QVariant fred = ClickedButton->property("PTRCOMPONENT");
    SSTInfoDataComponent* ptrComponent = VPtr<SSTInfoDataComponent>::asPtr(ClickedButton->property("PTRCOMPONENT"));

    // Now Send a signal to the Main Window telling it what component was clicked
    emit ComponentToolboxButtonPressed(ptrComponent);
}

void WindowComponentToolBox::HandleComponentToolboxButtonGroupToggled(int id, bool checked)
{
    Q_UNUSED(id)

    // THIS HANDLER IS CALLED WHEN A BUTTON IN THE COMPONENT TOOLBOX IS TOGGLED (after it is pressed down)

    // Check to see if the Clicked Button is checked or not
    if (checked == false) {
        // Send a signal to the Main Window telling it that we have no button selected,
        // and the scene should do nothing if it is clicked
        emit ComponentToolboxButtonPressed(NULL);
    }
}

void WindowComponentToolBox::HandleChangedButtonGroup(int index)
{
    // THIS HANDLER IS CALLED WHEN A TAB OF THE ToolboxWidget IS CHANGED

    // UnCheck all the buttons in the current group
    UncheckAllCurrentGroupButtons();

    if (index >= 0) {
        // Update the CurrentButtonGroup to the new index
        m_CurrentButtonGroup = m_ButtonGroupList.at(index);
    } else {
        m_CurrentButtonGroup = NULL;
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

    //Re-Populate the ToolBox using the new sort
    PopulateToolBox();
}

void WindowComponentToolBox::HandleComponentToolboxResized()
{
    int          x;
    int          Row, Col;
    int          ToolBoxWidth;
    int          ButtonWidth;
    int          NumButtonsAcross;
    QGridLayout* OldGridLayout;
    QGridLayout* NewGridLayout;
    QWidget*     ptrButtonGroupWidget;
    QWidget*     ptrButtonWidget;
    QLayoutItem* ptrLayoutItem;
    int          x_Pos;
    int          y_Pos;
    int          NumRows;
    int          NumCols;
    QString      TabName;

    // Get the new width of the toolbox
    ToolBoxWidth = m_ToolboxWidget->width();

    // Reseet the layout of the buttons inside of each tab of the ToolboxWidget,
    for (x = 0; x < m_ToolboxWidget->count(); x++) {
        // Get the Widget for this tab
        ptrButtonGroupWidget = m_ToolboxWidget->widget(x);

        // Get the Grid Layout for this Widget
        OldGridLayout = (QGridLayout*)ptrButtonGroupWidget->layout();

        // Create a New Layout
        NewGridLayout = new QGridLayout();

        TabName = m_ToolboxWidget->itemText(x);
        NumRows = OldGridLayout->rowCount();
        NumCols = OldGridLayout->columnCount();
        x_Pos = 0;
        y_Pos = 0;

        // Get each button
        for (Row = 0; Row < OldGridLayout->rowCount(); Row++) {
            for (Col = 0; Col < OldGridLayout->columnCount(); Col++) {
                ptrLayoutItem = OldGridLayout->itemAtPosition(Row, Col);
                if (ptrLayoutItem != NULL) {
                    ptrButtonWidget = ptrLayoutItem->widget();
                    ButtonWidth = ptrButtonWidget->width();
                    NumButtonsAcross = ToolBoxWidth / ButtonWidth;

                    // Add the NewButtonWidget to the Layout
                    NewGridLayout->addWidget(ptrButtonWidget, x_Pos, y_Pos++);

                    // Check to see if we need to reset the x_Pos or y_Pos
                    if (y_Pos >= NumButtonsAcross) {
                        x_Pos++;    // Increment the x_Pos
                        y_Pos = 0;  // Reset the y_Pos
                    }
                }
            }
        }

        // Set layout Parameters
        // Always make last unused row stretch to fill the area
        NewGridLayout->setRowStretch(x_Pos + 1, 10);

        // Remove the old Layout from ButtonGroup Widget and add the New one
        delete OldGridLayout;
        ptrButtonGroupWidget->setLayout(NewGridLayout);
    }
}

