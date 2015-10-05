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

#include "SupportClasses.h"

///////////////////////////////////////////////////////////////////////////////
// Initialize static Variables for Support classes
///////////////////////////////////////////////////////////////////////////////

// Init ApplicationPreferences Class static Variables
bool  ApplicationPreferences::m_AppPrefReturnToSelectToolAfterPlacingWire = true;
bool  ApplicationPreferences::m_AppPrefReturnToSelectToolAfterPlacingText = true;
bool  ApplicationPreferences::m_AppPrefAutoDeleteTooShortWires = true;
bool  ApplicationPreferences::m_AppPrefDisplayGridEnabled = false;
bool  ApplicationPreferences::m_AppPrefSnapToGridEnabled = true;
int   ApplicationPreferences::m_AppPrefSnapToGridSize = 20;
bool  ApplicationPreferences::m_AppPrefComponentWidthFullSize = true;

// Init GraphicItemData Class static Variables
int                          GraphicItemData::m_CurrentWireIndex = 0;
QMap<QString, int>           GraphicItemData::m_CurrentComponentByKeyIndex;
QList<GraphicItemWire*>      GraphicItemData::m_GraphicItemWireList;
QList<GraphicItemComponent*> GraphicItemData::m_GraphicItemComponentList;
QList<GraphicItemComponent*> GraphicItemData::m_GraphicItemComponentByTypeList[NUMCOMPONENTTYPES];

///////////////////////////////////////////////////////////////////////////////

SpinBoxEditor::SpinBoxEditor(int min, int max, QObject* parent /*=0*/)
    : QStyledItemDelegate(parent)
{
    // Save off the min and max
    m_min = min;
    m_max = max;
}

// Create Editor when we construct SpinBoxDelegate
QWidget* SpinBoxEditor::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    // TableView need to create an Editor
    // Create Editor when we construct MyDelegate
    // and return the Editor
    QSpinBox* editor = new QSpinBox(parent);
    editor->setMinimum(m_min);
    editor->setMaximum(m_max);
    return editor;
}

// Then, we set the Editor
void SpinBoxEditor::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    // Get the value via index of the Model
    int value = index.model()->data(index, Qt::EditRole).toInt();

    // Put the value into the SpinBox
    QSpinBox* spinbox = static_cast<QSpinBox*>(editor);
    spinbox->setValue(value);
}

// When we modify data, this model reflect the change
void SpinBoxEditor::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    // When we modify data, this model reflect the change
    QSpinBox* spinbox = static_cast<QSpinBox*>(editor);
    spinbox->interpretText();
    int value = spinbox->value();
    model->setData(index, value, Qt::EditRole);
}

// Give the SpinBox the info on size and location
void SpinBoxEditor::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    // Give the SpinBox the info on size and location
    editor->setGeometry(option.rect);
}

///////////////////////////////////////////////////////////////////////////////

QPointF SnapToGrid::CheckSnapToGrid(QPointF StartingPoint)
{
    QPointF                 AdjustedPoint;
    int                     NewX;
    int                     NewY;

    // Check to see if SnapToGrid is enabled
    if (ApplicationPreferences::GetAppPref_SnapToGridEnabled() == true) {
        // Compute the new X and Y positions if we are in Snap to Grid Mode
        NewX = RoundTo((int)StartingPoint.x(), (int) ApplicationPreferences::GetAppPref_SnapToGridSize());
        NewY = RoundTo((int)StartingPoint.y(), (int) ApplicationPreferences::GetAppPref_SnapToGridSize());
        AdjustedPoint = QPointF(NewX, NewY);
        return AdjustedPoint;
    } else {
        return StartingPoint;
    }
}

bool SnapToGrid::IsEnabled()
{
    return ApplicationPreferences::GetAppPref_SnapToGridEnabled();
}

int  SnapToGrid::GetGridSize()
{
    return ApplicationPreferences::GetAppPref_SnapToGridSize();
}

///////////////////////////////////////////////////////////////////////////////

int GraphicItemData::GetNextWireIndex()
{
    return ++m_CurrentWireIndex;
}

int GraphicItemData::GetCurrentWireIndex()
{
    return m_CurrentWireIndex;
}

void GraphicItemData::SetCurrentWireIndex(int NewIndex)
{
    m_CurrentWireIndex = NewIndex;
}

void GraphicItemData::ResetCurrentComponentIndexStructure()
{
    m_CurrentComponentByKeyIndex.clear();
}

int GraphicItemData::GetNextComponentIndexByKey(QString Key)
{
    int                IndexValue;

    // Get the Current index for this key
    IndexValue = m_CurrentComponentByKeyIndex.value(Key);  // If not in map, default will be 0

    // Increment the Index and put it back into the map for the key
    m_CurrentComponentByKeyIndex.insert(Key, ++IndexValue);

    // Return the new value
    return IndexValue;
}

void GraphicItemData::SetComponentIndexByKey(QMap<QString, int>& SetMap)
{
    m_CurrentComponentByKeyIndex = SetMap;
}

void GraphicItemData::ResetGraphicItemWireList()
{
    m_GraphicItemWireList.clear();
}

void GraphicItemData::ResetGraphicItemWComponentList()
{
    m_GraphicItemComponentList.clear();
}

void GraphicItemData::ResetGraphicItemWComponentByTypeList()
{
    for (int x = 0; x < NUMCOMPONENTTYPES; x++) {
        m_GraphicItemComponentByTypeList[x].clear();
    }
}

void GraphicItemData::AddWireToGraphicItemWireList(GraphicItemWire* NewWire)
{
    m_GraphicItemWireList.append(NewWire);
}

void GraphicItemData::AddComponentToGraphicItemComponentList(GraphicItemComponent* NewComponent)
{
    m_GraphicItemComponentList.append(NewComponent);
}

void GraphicItemData::AddComponentToGraphicItemComponentByTypeList(GraphicItemComponent* NewComponent, ComponentType_enum CompType)
{
    m_GraphicItemComponentByTypeList[CompType].append(NewComponent);
}

void GraphicItemData::RemoveWireFromGraphicItemWireList(GraphicItemWire* Wire)
{
    m_GraphicItemWireList.removeOne(Wire);
}

void GraphicItemData::RemoveComponentFromGraphicItemComponentList(GraphicItemComponent* Component)
{
    m_GraphicItemComponentList.removeOne(Component);
}

void GraphicItemData::RemoveComponentFromGraphicItemComponentByTypeList(GraphicItemComponent* Component, ComponentType_enum CompType)
{
    int                FoundIndex;

    if (Component != NULL) {
        // Find the index of the component of that type from the list
        FoundIndex = m_GraphicItemComponentByTypeList[CompType].indexOf(Component);
        if (FoundIndex >= 0) {
            // Remove the item at the found index
            m_GraphicItemComponentByTypeList[CompType].removeAt(FoundIndex);
        }
    }
}

QList<GraphicItemWire*>& GraphicItemData::GetGraphicItemWireList()
{
    return m_GraphicItemWireList;
}

QList<GraphicItemComponent*>& GraphicItemData::GetGraphicItemComponentList()
{
    return m_GraphicItemComponentList;
}

QList<GraphicItemComponent*>& GraphicItemData::GetGraphicItemComponentByTypeList(ComponentType_enum CompType)
{
    return m_GraphicItemComponentByTypeList[CompType];
}

void GraphicItemData::SaveData(QDataStream& DataStreamOut)
{
  DataStreamOut << (qint32)m_CurrentWireIndex;
  DataStreamOut << m_CurrentComponentByKeyIndex;

  // NOTE: GraphicItemWire and GraphicItemComponent lists are
  //       not saved/restored they are runtime only data
}

void GraphicItemData::LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion)
{
    Q_UNUSED(ProjectFileVersion)

    DataStreamIn >> m_CurrentWireIndex;
    DataStreamIn >> m_CurrentComponentByKeyIndex;

    // NOTE: GraphicItemWire and GraphicItemComponent lists are
    //       not saved/restored they are runtime only data
}

