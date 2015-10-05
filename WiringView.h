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

#ifndef WIRINGVIEW_H
#define WIRINGVIEW_H

#include "GlobalIncludes.h"

class WiringView : public QGraphicsView
{
    Q_OBJECT

public:
    WiringView(QGraphicsScene* scene, QWidget* parent = 0);

    void ScaleViewToNewSize(double NewScalePercent);

    qreal GetCurrentScaleValue();

    void PrintRenderer(QPainter* painter);

    void FitRectIntoView(QRectF RectToFit);

    qreal GetCurrentScalePercent();

//    void SetCurrentScalePercent(qreal ScalePercent);

private:
    qreal m_CurrentScalePercent;
};

#endif // WIRINGVIEW_H
