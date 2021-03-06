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

#ifndef GRAPHICITEMTEXT_H
#define GRAPHICITEMTEXT_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"

////////////////////////////////////////////////////////////

class GraphicItemText : public QGraphicsTextItem, public GraphicItemBase
{
    Q_OBJECT

public:
    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_TEXT };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemText(const QFont& InitTextFont, const QColor& InitTextColor, const QPointF& startPos, QGraphicsItem* parent = 0);
    GraphicItemText(QDataStream& DataStreamIn, QGraphicsItem* parent = 0);  // Only used for serialization
    virtual ~GraphicItemText();

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

signals:
    // Signals to the Scene
    void ItemTextLostFocus(GraphicItemText* item);
    void ItemTextSetProjectDirty();

private:
    void CommonSetup();

private slots:
    void HandleDocumentContentsChanged();

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void focusOutEvent(QFocusEvent* event);

private:
    qreal m_MoveDelayDistance;
};

#endif // GRAPHICITEMTEXT_H

