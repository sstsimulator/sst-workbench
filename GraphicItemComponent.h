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

#ifndef GRAPHICITEMCOMPONENT_H
#define GRAPHICITEMCOMPONENT_H

#include "GlobalIncludes.h"

#include "GraphicItemBase.h"
#include "GraphicItemPort.h"
#include "SSTInfoData.h"
#include "PortInfoData.h"

///////////////////////////////////////////////////////////////////////////////

class GraphicItemComponent : public QObject, public QGraphicsRectItem, public GraphicItemBase
{
    Q_OBJECT

public:
    // Required Virtual function for any QGraphicItem to provide a type value for the qgraphicsitem_cast function
    enum { Type = UserType + ITEMTYPE_COMPONENT };
    int type() const {return Type;}

    // Constructor / Destructor
    GraphicItemComponent(int ComponentIndex, SSTInfoDataComponent* SSTInfoComponent, QMenu* ItemMenu, QColor& ComponentFillColor, const QPointF& startPos, QGraphicsItem* parent = 0);
    GraphicItemComponent(QDataStream& DataStreamIn, QMenu* ItemMenu, QGraphicsItem* parent = 0);  // Only used for serialization
    virtual ~GraphicItemComponent();

    // Control of Component Index
    int  GetComponentIndex() {return m_ComponentIndex;}
    void SetComponentIndex(int NewIndex);

    // Serialization
    void SaveData(QDataStream& DataStreamOut);

    // Get the name of the Image for the Component Toolbox button (STATIC FUNCTION)
    static QString GetComponentButtonIconImageName(SSTInfoDataComponent::ComponentType componentType);

    // Update Visual Layout of the Component and its Ports
    // This will add/remove graphical dynamic ports as necessary
    void UpdateVisualLayoutOfComponent();

    // Return the Component Information
    QString GetParentElementName() {return m_ParentElementName;}
    QString GetComponentName() {return m_ComponentName;}
    QString GetComponentTypeName() {return m_ComponentTypeName;}
    QString GetComponentDisplayName() {return m_ComponentDisplayName;}
    SSTInfoDataComponent::ComponentType GetComponentType() const { return m_ComponentType; }
    int GetNumAllowedInstances() const {return m_NumAllowedInstances;}

    // Disconnect wires from any connected ports
    void DisconnectPortsFromAllWires();

    // Info on Ports
    int GetNumGraphicalPortsOnComponent();
    bool ComponentContainsDynamicPorts();
    bool AreAnyComponentPortsConnectedToWires();
    QList<PortInfoData*>& GetPortInfoDataArray() {return m_PortInfoDataArray;}
    QList<GraphicItemPort*>& GetGraphicalPortArray() {return m_GraphicalPortArray;}

    void SetComponentFillColor(const QColor& color);

    bool GetMovingPortsMode() {return m_MovingPortsMode;}
    void SetMovingPortsMode(bool Flag);

    // Module Support
    QStringList GetModulesList() {return m_ComponentModuleNames;}
    void        SetModulesList(QStringList ModulesList) {m_ComponentModuleNames = ModulesList;}
    void        AddModuleParameter(QString ParamName, QString DefaultValue, QString Desc);

signals:
    void ItemComponentSetProjectDirty();
    void ItemComponentRefreshPropertiesWindowProperty(QString, QString);
    void ItemComponentRefreshPropertiesWindow(ItemProperties*);

private slots:
    void HandleItemPortModedPosition(GraphicItemPort* Port);

private:
    void ReorderPortSequence(GraphicItemPort* CurrentPort, PortInfoData::ComponentSide CurrentSide, bool SwapSides, int CurrentSeq, int NewSeq);
    void CreateComponentDisplayName();
    void CreateInitiaLVisualLayoutOfComponent();
    void SetComponentBoxDimensions(int NumPortsLeftSide, int NumPortsRightSide);
    void ReconfigureAllGraphicalPorts();

    // Initialization
    void CommonInit();
    void CommonSetup();

private:
    // Mouse Handling
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

    // Something Changed on the Component
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    void HighlightSelected(QPainter* painter, const QStyleOptionGraphicsItem* option);

    // Handle a property change
    void PropertyChanged(QString& PropName, QString& NewPropValue);
    void DynamicPropertiesChanged(ItemProperties* ptrExistingProperties);

private:
    SSTInfoDataComponent::ComponentType m_ComponentType;
    int                                 m_ComponentIndex;

    QString                             m_ParentElementName;
    QString                             m_ComponentUserName;
    QString                             m_ComponentName;
    QString                             m_ComponentDesc;
    QString                             m_ComponentTypeName;
    QString                             m_ComponentDisplayName;
    QString                             m_ComponentDisplayTypeName;
    QString                             m_ComponentUniqueName;

    int                                 m_NumAllowedInstances;

    QList<PortInfoData*>                m_PortInfoDataArray;            // Contains Expaned Information on Ports (Above what SSTInfo gives us)

    QList<GraphicItemPort*>             m_GraphicalPortArray;           // Array of all Graphical Ports
    QList<GraphicItemPort*>             m_GraphicalPortArrayLeftSide;   // Array of Graphical Ports on the Left Side
    QList<GraphicItemPort*>             m_GraphicalPortArrayRightSide;  // Array of Graphical Ports on the Right Side

    QStringList                         m_ComponentModuleNames;             // String Array of Module Names loaded by this Component
    QList<QGraphicsRectItem*>           m_ComponentModuleRectList;
    QList<QGraphicsSimpleTextItem*>     m_ComponentModuleTextList;

    QGraphicsSimpleTextItem*            m_ComponentGraphicDisplayName;
    QGraphicsSimpleTextItem*            m_ComponentGraphicDisplayTypeName;

    QMenu*                              m_ItemMenu;

    QColor                              m_ComponentFillColor;
    QRectF                              m_ComponentRect;
    int                                 m_NumPortsLeftSide;
    int                                 m_NumPortsRightSide;
    int                                 m_ComponentSideHeight;
    qreal                               m_MoveDelayDistance;

    bool                                m_MovingPortsMode;
};

#endif // GRAPHICITEMCOMPONENT_H
