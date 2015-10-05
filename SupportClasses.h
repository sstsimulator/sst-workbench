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

#ifndef SUPPORTCLASSES_H
#define SUPPORTCLASSES_H

#include "GlobalIncludes.h"

// Forward Declarations
class GraphicItemWire;
class GraphicItemComponent;

//////////////////////////////////////////////////////////////////////////////

// A helper Template class to assist with working with QVarients
template <class T> class VPtr
{
public:
    static T* asPtr(QVariant v)
    {
        return  (T*) v.value<void *>();
    }

    static QVariant asQVariant(T* ptr)
    {
        return qVariantFromValue((void *) ptr);
    }
};

//////////////////////////////////////////////////////////////////////

// A helper template to round to the next step multiple
template<typename T>
T RoundTo( T value, T multiple )
{
    if (multiple == 0) {
        return value;
    }

    return static_cast<T>(round(static_cast<double>(value)/static_cast<double>(multiple))*static_cast<double>(multiple));
}

//////////////////////////////////////////////////////////////////////

// SpinBoxEditor Support Class
class SpinBoxEditor : public QStyledItemDelegate
{
    Q_OBJECT
public:
    // Constructor / Destructor
    explicit SpinBoxEditor(int min, int max, QObject* parent = 0);

private:
    // Create Editor when we construct SpinBoxDelegate
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    // Then, we set the Editor
    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    // When we modify data, this model reflect the change
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    // Give the SpinBox the info on size and location
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    int m_min;
    int m_max;
};

//////////////////////////////////////////////////////////////////////

// Global Preferences Support Class
class ApplicationPreferences
{
public:
    static void SetAppPref_ReturnToSelectToolAfterPlacingWire(bool Flag) {m_AppPrefReturnToSelectToolAfterPlacingWire = Flag;}
    static void SetAppPref_ReturnToSelectToolAfterPlacingText(bool Flag) {m_AppPrefReturnToSelectToolAfterPlacingText = Flag;}
    static void SetAppPref_AutoDeleteTooShortWires(bool Flag) {m_AppPrefAutoDeleteTooShortWires = Flag;}
    static void SetAppPref_DisplayGridEnabled(bool Flag) {m_AppPrefDisplayGridEnabled = Flag;}
    static void SetAppPref_SnapToGridEnabled(bool Flag) {m_AppPrefSnapToGridEnabled = Flag;}
    static void SetAppPref_SnapToGridSize(int Size) {m_AppPrefSnapToGridSize = Size;}

    static bool GetAppPref_ReturnToSelectToolAfterPlacingWire() {return m_AppPrefReturnToSelectToolAfterPlacingWire;}
    static bool GetAppPref_ReturnToSelectToolAfterPlacingText() {return m_AppPrefReturnToSelectToolAfterPlacingText;}
    static bool GetAppPref_AutoDeleteTooShortWires() {return m_AppPrefAutoDeleteTooShortWires;}
    static bool GetAppPref_DisplayGridEnabled() {return m_AppPrefDisplayGridEnabled;}
    static bool GetAppPref_SnapToGridEnabled() {return m_AppPrefSnapToGridEnabled;}
    static int  GetAppPref_SnapToGridSize() {return m_AppPrefSnapToGridSize;}

private:
    // Init Of ApplicationPreferences Static Variables is done in SupportClasses.cpp
    static bool m_AppPrefReturnToSelectToolAfterPlacingWire;
    static bool m_AppPrefReturnToSelectToolAfterPlacingText;
    static bool m_AppPrefAutoDeleteTooShortWires;
    static bool m_AppPrefDisplayGridEnabled;
    static bool m_AppPrefSnapToGridEnabled;
    static int  m_AppPrefSnapToGridSize;
    static bool m_AppPrefComponentWidthFullSize;
};

//////////////////////////////////////////////////////////////////////////////

class SnapToGrid
{
public:
    static QPointF CheckSnapToGrid(QPointF StartingPoint);
    static bool IsEnabled();
    static int  GetGridSize();
};

//////////////////////////////////////////////////////////////////////////////

class GraphicItemData
{
public:
    // Wire Index
    static int GetNextWireIndex();
    static int GetCurrentWireIndex();
    static void SetCurrentWireIndex(int NewIndex);

    // Component Index By Key Type
    static void ResetCurrentComponentIndexStructure();
    static int  GetNextComponentIndexByKey(QString Key);
    static void SetComponentIndexByKey(QMap<QString, int>& SetMap);

    // GraphicItemWire & GraphicItemComponent Lists
    static void ResetGraphicItemWireList();
    static void ResetGraphicItemWComponentList();
    static void ResetGraphicItemWComponentByTypeList();

    static void AddWireToGraphicItemWireList(GraphicItemWire* NewWire);
    static void AddComponentToGraphicItemComponentList(GraphicItemComponent* NewComponent);
    static void AddComponentToGraphicItemComponentByTypeList(GraphicItemComponent* NewComponent, ComponentType_enum CompType);

    static void RemoveWireFromGraphicItemWireList(GraphicItemWire* Wire);
    static void RemoveComponentFromGraphicItemComponentList(GraphicItemComponent* Component);
    static void RemoveComponentFromGraphicItemComponentByTypeList(GraphicItemComponent* Component, ComponentType_enum CompType);

    static QList<GraphicItemWire*>& GetGraphicItemWireList();
    static QList<GraphicItemComponent*>& GetGraphicItemComponentList();
    static QList<GraphicItemComponent*>& GetGraphicItemComponentByTypeList(ComponentType_enum CompType);

    // Serialization
    static void SaveData(QDataStream& DataStreamOut);
    static void LoadData(QDataStream& DataStreamIn, qint32 ProjectFileVersion);

private:
    // Init Of GraphicItemIndex Static Variables is done in SupportClasses.cpp

    // Index of Wires and Components
    static int                          m_CurrentWireIndex;           // Current Index of Wires
    static QMap<QString, int>           m_CurrentComponentByKeyIndex; // Map of Current Indexs of Components by KeyValue <element>.<componentname>

    // Lists to track Graphic items
    static QList<GraphicItemWire*>      m_GraphicItemWireList;          // List of all GraphicItemWires
    static QList<GraphicItemComponent*> m_GraphicItemComponentList;     // List of all GraphicItemComponents
    static QList<GraphicItemComponent*> m_GraphicItemComponentByTypeList[NUMCOMPONENTTYPES]; // List of GraphicItemComponents by ComponentType
};

//////////////////////////////////////////////////////////////////////////////

#endif // SUPPORTCLASSES_H
