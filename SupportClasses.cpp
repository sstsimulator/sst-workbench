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

#include "SupportClasses.h"

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

