// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdrect.h"

#include <QDebug>
#include <QPen>
#include <QBrush>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonValue>

CommandRect::CommandRect()
    : SceneManager::Command( Type )
{
}

CommandRect::CommandRect( const QString& aLayer, qreal aX0,qreal aY0,qreal aX1,qreal aY1, quint32 aCol )
    : SceneManager::Command( Type )
    , layer( aLayer )
    , x0( aX0 )
    , y0( aY0 )
    , x1( aX1 )
    , y1( aY1 )
    , col( aCol )
{
    if( x0 > x1 ) std::swap(x0,x1);
    if( y0 > y1 ) std::swap(y0,y1);
}

CommandRect* CommandRect::parse( const QJsonObject& aData )
{
    if( aData["command"] != "rect" )
        return NULL;

    QJsonValue vx0 = aData["x0"];
    if( !vx0.isDouble() ) { qDebug() << "error in rect command, invalid x0 parameter: " << vx0.toString(); return NULL; }
    qreal x0 = vx0.toDouble();

    QJsonValue vy0 = aData["y0"];
    if( !vy0.isDouble() ) { qDebug() << "error in rect command, invalid y0 parameter: " << vy0.toString(); return NULL; }
    qreal y0 = vy0.toDouble();

    QJsonValue vx1 = aData["x1"];
    if( !vx1.isDouble() ) { qDebug() << "error in rect command, invalid x1 parameter: " << vx1.toString(); return NULL; }
    qreal x1 = vx1.toDouble();

    QJsonValue vy1 = aData["y1"];
    if( !vy1.isDouble() ) { qDebug() << "error in rect command, invalid y1 parameter: " << vy1.toString(); return NULL; }
    qreal y1 = vy1.toDouble();

    bool ok = true;
    QJsonValue vcol = aData["color"];
    quint32 col = toCol32( vcol, &ok );
    if( !ok ) { qDebug() << "error in rect command, invalid color parameter: " << vcol.toString(); return NULL; }

    QString layer = toLayer( aData["layer"] );
    return new CommandRect(layer, x0, y0, x1, y1, col);
}

void CommandRect::save( QTextStream& aStrm, const QGraphicsItem* aItem )
{
    const QGraphicsRectItem* rect = qgraphicsitem_cast<const QGraphicsRectItem*>(aItem);
    Q_ASSERT( rect != NULL );
    if( !rect )
        return;

    QPointF tl = rect->rect().topLeft();
    QPointF br = rect->rect().bottomRight();
    QColor col = rect->pen().color();

    QByteArray cmd;
    QTextStream(&cmd) << "{ \"command\":\"rect\","
        << "\"layer\":\"" << toLayer(aItem) << "\","
        << "\"x0\":" << tl.x() << ","
        << "\"y0\":" << -tl.y() << ","
        << "\"x1\":" << br.x() << ","
        << "\"y1\":" << -br.y() << ","
        << "\"color\":" << toCol32( col )
        << "}";
    aStrm << cmd;
}

void CommandRect::execute( SceneManager& aScene )
{
    qreal x = x0;
    qreal y = y0;
    qreal w = x1 - x0;
    qreal h = y1 - y0;

    QGraphicsRectItem* item = new QGraphicsRectItem( x,-y,w,-h );
    item->setData( SceneManager::DataTypeKey, QVariant(Type) );
    item->setData( SceneManager::DataBound, QVariant( QRectF(QPointF(x0,-y0),QPointF(x1,-y1)) ));

    QPen pen;
    QColor color = toQColor( col );
    pen.setColor( color );
    //pen.setCosmetic( false );
    pen.setWidth(0);
    item->setPen( pen );

    QBrush brush( Qt::SolidPattern );
    brush.setColor( color );
    item->setBrush( brush );

    aScene.addItem( item, layer, LayerPartGraph );
}
