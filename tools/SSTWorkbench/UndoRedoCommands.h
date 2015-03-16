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

#ifndef UNDOREDOCOMMANDS_H
#define UNDOREDOCOMMANDS_H

#include "GlobalIncludes.h"

#include "WiringScene.h"

#define PASTEID 100

class WiringScene;

///////////////////////////////////////////////////////////////////////////////////////
class ComandAddGraphicItemComponent : public QUndoCommand
{
public:
    ComandAddGraphicItemComponent(GraphicItemComponent* Component, WiringScene* Scene,  bool PasteMode = false, QUndoCommand* parent = 0);
    ~ComandAddGraphicItemComponent();

    void undo();
    void redo();

private:
    void UpdateCommandText();

private:
    GraphicItemComponent* m_Component;
    WiringScene*          m_WiringScene;
    bool             m_PasteMode;
};

///////////////////////////////////////////////////////////////////////////////////////
class ComandAddGraphicItemText : public QUndoCommand
{
public:
    ComandAddGraphicItemText(GraphicItemText* Text, WiringScene* Scene,  bool PasteMode = false, QUndoCommand* parent = 0);
    ~ComandAddGraphicItemText();

    void undo();
    void redo();

private:
    void UpdateCommandText();

private:
    GraphicItemText* m_Text;
    WiringScene*     m_WiringScene;
    bool             m_PasteMode;
};

///////////////////////////////////////////////////////////////////////////////////////
class ComandAddGraphicItemWire : public QUndoCommand
{
public:
    ComandAddGraphicItemWire(GraphicItemWire* Wire, WiringScene* Scene,  bool PasteMode = false, QUndoCommand* parent = 0);
    ~ComandAddGraphicItemWire();

    void undo();
    void redo();

private:
    void UpdateCommandText();

private:
    GraphicItemWire* m_Wire;
    WiringScene*     m_WiringScene;
    bool             m_PasteMode;
};

///////////////////////////////////////////////////////////////////////////////////////

class ComandDeleteGraphicItems : public QUndoCommand
{
public:
    ComandDeleteGraphicItems(WiringScene* Scene, QUndoCommand* parent = 0);
    ~ComandDeleteGraphicItems();

    void undo();
    void redo();

private:
    WiringScene*                 m_WiringScene;
    QList<GraphicItemWire*>      m_DeletedWireList;
    QList<GraphicItemComponent*> m_DeletedComponentList;
    QList<GraphicItemText*>      m_DeletedTextList;
    bool                         m_Initialized;
};

#endif // UNDOREDOCOMMANDS_H
