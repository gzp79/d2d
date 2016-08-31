// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdline.h"

#include <QDebug>
#include <QPen>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonValue>

CommandLine::CommandLine()
    : SceneManager::Command( Type )
{
}

CommandLine::CommandLine( const QString& aLayer, qreal aX0,qreal aY0,qreal aX1,qreal aY1, quint32 aCol )
    : SceneManager::Command( Type )
    , layer( aLayer )
    , x0( aX0 )
    , y0( aY0 )
    , x1( aX1 )
    , y1( aY1 )
    , col( aCol )
{
}

CommandLine* CommandLine::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "line" )
        return NULL;

    QJsonValue vx0 = aData["x0"];
    if( !vx0.isDouble() ) { qDebug() << "error in line command, invalid x0 parameter: " << vx0.toString(); return NULL; }
    qreal x0 = vx0.toDouble();

    QJsonValue vy0 = aData["y0"];
    if( !vy0.isDouble() ) { qDebug() << "error in line command, invalid y0 parameter: " << vy0.toString(); return NULL; }
    qreal y0 = vy0.toDouble();

    QJsonValue vx1 = aData["x1"];
    if( !vx1.isDouble() ) { qDebug() << "error in line command, invalid x1 parameter: " << vx1.toString(); return NULL; }
    qreal x1 = vx1.toDouble();

    QJsonValue vy1 = aData["y1"];
    if( !vy1.isDouble() ) { qDebug() << "error in line command, invalid y1 parameter: " << vy1.toString(); return NULL; }
    qreal y1 = vy1.toDouble();

    bool ok = true;
    QJsonValue vcol = aData["color"];
    quint32 col = toCol32( vcol, &ok );
    if( !ok ) { qDebug() << "error in line command, invalid color parameter: " << vcol.toString(); return NULL; }

    QString layer = toLayer( aData["layer"] );
    return new CommandLine(layer, x0,y0,x1,y1,col);
}

void CommandLine::save( QTextStream& aStrm, const QGraphicsItem* aItem )
{
    const QGraphicsLineItem* line = qgraphicsitem_cast<const QGraphicsLineItem*>(aItem);
    Q_ASSERT( line != NULL );
    if( !line )
        return;

    QPointF a = line->line().p1();
    QPointF b = line->line().p2();
    QColor col = line->pen().color();


    QByteArray cmd;
    QTextStream(&cmd) << "{\"command\":\"line\","
        << "\"layer\":\"" << toLayer(aItem) << "\","
        << "\"x0\":" << a.x() << ","
        << "\"y0\":" << -a.y() << ","
        << "\"x1\":" << b.x() << ","
        << "\"y1\":" << -b.y() << ","
        << "\"color\":" << toCol32( col )
        << "}";
    aStrm << cmd;
}

void CommandLine::execute( SceneManager& aScene )
{
    QGraphicsLineItem* item = new QGraphicsLineItem( x0,-y0,x1,-y1 );
    item->setData( SceneManager::DataTypeKey, QVariant(Type) );
    item->setData( SceneManager::DataBound, QVariant( QRectF(QPointF(x0,-y0),QPointF(x1,-y1)) ));
    item->setFlag( QGraphicsItem::ItemIsSelectable, true );

    QPen pen;
    pen.setColor( toQColor( col ) );
    pen.setWidth(0);
    item->setPen( pen );

    aScene.addItem( item, layer, LayerCategoryGraph );
}
