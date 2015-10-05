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

#include "WiringView.h"

////////////////////////////////////////////////////////////

WiringView::WiringView(QGraphicsScene* scene, QWidget* parent /*= 0*/) :
    QGraphicsView(scene, parent)
{
    // Init vars
    m_CurrentScalePercent = 100;
}

void WiringView::ScaleViewToNewSize(double NewScalePercent)
{
    QMatrix oldMatrix;
    double  NewScale;

    // Change the Percent into a scaling factor
    NewScale = NewScalePercent / 100;

    // Get the old matrix and then scale it to the new size
    oldMatrix = matrix();
    resetMatrix();
    translate(oldMatrix.dx(), oldMatrix.dy());
    scale(NewScale, NewScale);

    // Save the scale value
    m_CurrentScalePercent = NewScalePercent;
}

qreal WiringView::GetCurrentScaleValue()
{
    return transform().m11() * 100;
}

void WiringView::PrintRenderer(QPainter* painter)
{
    render(painter);
}

void WiringView::FitRectIntoView(QRectF RectToFit)
{
    fitInView(RectToFit, Qt::KeepAspectRatio);
}

qreal WiringView::GetCurrentScalePercent()
{
    return m_CurrentScalePercent;
}

//void WiringView::SetCurrentScalePercent(qreal ScalePercent)
//{
//    m_CurrentScalePercent = ScalePercent;
//}
