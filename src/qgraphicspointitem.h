// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef QGRAPHICSPOINTITEM
#define QGRAPHICSPOINTITEM

#include "scenemanager.h"
#include <QGraphicsItem>

class QGraphicsPointItem : public QGraphicsItem
{
    Q_GRAPHICSITEM( SceneManager::GraphicsPoint )

public:   
    QGraphicsPointItem( const QColor& aCol );

    QColor color() const { return mColor; }
    //void setColor( const QColor& aCol );

protected:
    QColor mColor;

    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aStyleOpt, QWidget* aWidget ) Q_DECL_OVERRIDE;

};

#endif // QGRAPHICSPOINTITEM

