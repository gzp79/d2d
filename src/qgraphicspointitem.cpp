// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "qgraphicspointitem.h"

#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include "scenemanager.h"

QGraphicsPointItem::QGraphicsPointItem( const QColor& aColor )
    : QGraphicsItem()
    , mColor( aColor )
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

QRectF QGraphicsPointItem::boundingRect() const
{
    return QRectF( -5, -5, 10, 10 );
}

void QGraphicsPointItem::paint(QPainter* aPainter, const QStyleOptionGraphicsItem* /*aStyleOpt*/, QWidget* /*aWidget*/ )
{
    aPainter->setBrush( mColor );
    aPainter->setPen( mColor );
    aPainter->drawEllipse( QRectF(-1.5, -1.5, 3, 3) );

    if( isSelected() )
    {
        aPainter->setBrush( QBrush() );
        aPainter->drawEllipse( QRectF(-5, -5, 10, 10) );
    }
}

