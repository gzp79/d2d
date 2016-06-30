// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdpoint.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QJsonObject>
#include <QJsonValue>

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
    if( aData["command"] != "point" )
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
    const QGraphicsEllipseItem* ellipse = qgraphicsitem_cast<const QGraphicsEllipseItem*>(aItem);
    Q_ASSERT( ellipse != NULL );
    if( !ellipse )
        return;

    QColor col = ellipse->brush().color();

    QByteArray cmd;
    QTextStream(&cmd) << "{ \"command\":\"point\","
        << "\"layer\":\"" << toLayer(aItem) << "\","
        << "\"x\":" << ellipse->x() << ","
        << "\"y\":" << -ellipse->y() << ","
        << "\"color\":" << toCol32( col )
        << "}";
    aStrm << cmd;
}

void CommandPoint::execute( SceneManager& aScene )
{
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem( -1.5, -1.5, 3,3 );
    item->setData( SceneManager::DataTypeKey, QVariant( Type ) );
    item->setData( SceneManager::DataBound, QVariant( QPointF(x,-y) ));

    item->setPos(x,-y);
    item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    item->setZValue( 1 );

    QColor color( toQColor(col) );
    item->setPen( Qt::NoPen );
    item->setBrush( color );

    aScene.addItem( item, layer, LayerPartGraph );
}
