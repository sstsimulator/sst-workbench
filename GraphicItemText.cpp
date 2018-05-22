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

#include "GraphicItemText.h"

////////////////////////////////////////////////////////////

GraphicItemText::GraphicItemText(const QFont& InitTextFont, const QColor& InitTextColor, const QPointF& startPos, QGraphicsItem *parent /*=0*/)
    : QGraphicsTextItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_TEXT)
{
    // Set the Font and Color
    setFont(InitTextFont);
    setDefaultTextColor(InitTextColor);

    // Set the initial position
    setPos(startPos);

    // Set the Z height (Text is always starts on top )
    setZValue(TEXT_ZVALUE);

    // Perform General Setup
    CommonSetup();
}

GraphicItemText::GraphicItemText(QDataStream& DataStreamIn, QGraphicsItem *parent /*=0*/)
    : QGraphicsTextItem(parent), GraphicItemBase(GraphicItemBase::ITEMTYPE_TEXT)
{
    QPointF NewPos;
    qreal   NewZValue;
    QString NewText;
    QColor  NewColor;
    QFont   NewFont;

    // Serialization: Load General GraphicItem Data
    DataStreamIn >> NewPos;
    DataStreamIn >> NewZValue;

    // Load Text data
    DataStreamIn >> NewText;
    DataStreamIn >> NewColor;
    DataStreamIn >> NewFont;

    // Set all the text info
    setPos(NewPos);
    setZValue(NewZValue);
    setPlainText(NewText);
    setDefaultTextColor(NewColor);
    setFont(NewFont);

    // Perform General Setup
    CommonSetup();
}

GraphicItemText::~GraphicItemText()
{
}

void GraphicItemText::SaveData(QDataStream& DataStreamOut)
{
    // Serialization: Save General GraphicItem Data
    DataStreamOut << pos();
    DataStreamOut << zValue();

    // Save Text data
    DataStreamOut << toPlainText();
    DataStreamOut << defaultTextColor();
    DataStreamOut << font();
}


void GraphicItemText::CommonSetup()
{
    // Setup the handler for when the Text Contents change
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(HandleDocumentContentsChanged()));

    // Set Graphic parameters
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    // Make the text editable
    setTextInteractionFlags(Qt::TextEditorInteraction);

    // Set the Initial Position Move Delay
    m_MoveDelayDistance = MOUSEMOVE_DELAYPIXELS;
}

void GraphicItemText::HandleDocumentContentsChanged()
{
    // Signal that the Text Contents changed
    emit ItemTextSetProjectDirty();
}

QVariant GraphicItemText::itemChange(GraphicsItemChange change, const QVariant& value)
{
    QPointF NewPos;
    QPointF OldPos;
    bool    SelectedState;

    // Did this get called to to a Selection Change?
    if (change == ItemSelectedChange && scene()) {
        // value is the selected state
        SelectedState = value.toBool();
    }

    // Check to see if the Item has moved enough
    if (change == QGraphicsItem::ItemPositionChange) {
        NewPos = value.toPointF();
        OldPos = pos();
        if ((OldPos - NewPos).manhattanLength() < m_MoveDelayDistance) {
            // The mouse has not moved enough, keep the old position
            return QVariant(OldPos);
        } else {
            // Clear the delay distance until the mouse is released
            m_MoveDelayDistance = 0;
        }
    }

    // If the position has moved, then mark the Project as dirty
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        emit ItemTextSetProjectDirty();
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphicItemText::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Reset the Delay Distance
    m_MoveDelayDistance = MOUSEMOVE_DELAYPIXELS;
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicItemText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    // User wants to edit the text item
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus();
    }

    // Call the base class handler
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void GraphicItemText::focusOutEvent(QFocusEvent* event)
{
    // Clear the cursor
    QTextCursor Cursor = textCursor();
    Cursor.clearSelection();
    setTextCursor(Cursor);

    // Stop the text interaction with the object
    setTextInteractionFlags(Qt::NoTextInteraction);

    // Signal that we lost focus to anyone that cares
    emit ItemTextLostFocus(this);

    // Call the base class handler
    QGraphicsTextItem::focusOutEvent(event);
}

