// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdpoint.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QJsonObject>
#include <QJsonValue>

#include "qgraphicspointitem.h"

CommandPoint::CommandPoint()
    : SceneManager::Command( Type )
{
}

CommandPoint::CommandPoint( const QString& aLayer, qreal aX,qreal aY, quint32 aCol )
    : SceneManager::Command( Type )
    , layer( aLayer )
    , x(aX)
    , y(aY)
    , col( aCol )
{
}

CommandPoint* CommandPoint::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "point" )
        return NULL;

    QJsonValue vx = aData["x"];
    if( !vx.isDouble() ) { qDebug() << "error in point command, invalid x parameter: " << vx.toString(); return NULL; }
    qreal x = vx.toDouble();

    QJsonValue vy = aData["y"];
    if( !vy.isDouble() ) { qDebug() << "error in point command, invalid y parameter: " << vy.toString(); return NULL; }
    qreal y = vy.toDouble();

    bool ok = true;
    QJsonValue vcol = aData["color"];
    quint32 col = toCol32( vcol, &ok );
    if( !ok ) { qDebug() << "error in point command, invalid color parameter: " << vcol.toString(); return NULL; }

    QString layer = toLayer( aData["layer"] );
    return new CommandPoint(layer, x,y,col);
}

void CommandPoint::save( QTextStream& aStrm, const QGraphicsItem* aItem )
{
    const QGraphicsPointItem* item = qgraphicsitem_cast<const QGraphicsPointItem*>(aItem);
    Q_ASSERT( item != NULL );
    if( !item )
        return;

    QColor col = item->color();

    QByteArray cmd;
    QTextStream(&cmd) << "{\"command\":\"point\","
        << "\"layer\":\"" << toLayer(aItem) << "\","
        << "\"x\":" << item->x() << ","
        << "\"y\":" << -item->y() << ","
        << "\"color\":" << toCol32( col )
        << "}";
    aStrm << cmd;
}

void CommandPoint::execute( SceneManager& aScene )
{
    QGraphicsPointItem* item = new QGraphicsPointItem( toQColor(col) );
    item->setData( SceneManager::DataTypeKey, QVariant( Type ) );
    item->setData( SceneManager::DataBound, QVariant( QPointF(x,-y) ));
    item->setFlag( QGraphicsItem::ItemIsSelectable, true );

    item->setPos(x,-y);    
    item->setZValue( 1 );    

    aScene.addItem( item, layer, LayerCategoryGraph );
}
