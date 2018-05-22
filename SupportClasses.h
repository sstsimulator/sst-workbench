////////////////////////////////////////////////////////////////////////
// Copyright 2009-2018 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2018, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#ifndef SUPPORTCLASSES_H
#define SUPPORTCLASSES_H

#include "GlobalIncludes.h"

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


#endif // SUPPORTCLASSES_H
