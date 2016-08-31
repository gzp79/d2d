// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdpoly.h"

#include <QJsonArray>
#include <QDebug>
#include <QPen>
#include <QBrush>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonValue>

CommandPoly::CommandPoly()
    : SceneManager::Command( Type )
{
}

CommandPoly::CommandPoly( const QString& aLayer, const QVector<qreal>& aX, const QVector<qreal>& aY, quint32 aCol )
    : SceneManager::Command( Type )
    , layer( aLayer )
    , x( aX )
    , y( aY )
    , col( aCol )
{
}

CommandPoly* CommandPoly::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "poly" )
        return NULL;

    QVector<qreal> x,y;
    QJsonValue vx = aData["x"];
    if( !vx.isArray() ) { qDebug() << "error in poly command, invalid x parameter: " << vx.toString(); return NULL; }
    QJsonArray vxa = vx.toArray();
    if( vxa.size() < 3 ) { qDebug() << "error in poly command, x parameter count is too low at least 3 is required: " << vxa.size(); return NULL; }
    x.resize( vxa.size() );
    for( int i = 0; i < vxa.size(); ++i )
    {
        QJsonValue v = vxa[i];
        if( !v.isDouble() ) { qDebug() << "error in poly command, invalid x parameter value at " << i << ": " << v.toString(); return NULL; }
        x[i] = v.toDouble();
    }

    QJsonValue vy = aData["y"];
    if( !vy.isArray() ) { qDebug() << "error in poly command, invalid y parameter: " << vy.toString(); return NULL; }
    QJsonArray vya = vy.toArray();
    if( vya.size() != x.size() ) { qDebug() << "error in poly command, x,y parameter count mismatch: " << x.size() << "," << vya.size(); return NULL; }
    y.resize( vya.size() );
    for( int i = 0; i < vya.size(); ++i )
    {
        QJsonValue v = vya[i];
        if( !v.isDouble() ) { qDebug() << "error in poly command, invalid y parameter value at " << i << ": " << v.toString(); return NULL; }
        y[i] = v.toDouble();
    }

    bool ok = true;
    QJsonValue vcol = aData["color"];
    quint32 col = toCol32( vcol, &ok );
    if( !ok ) { qDebug() << "error in poly command, invalid color parameter: " << vcol.toString(); return NULL; }

    //qDebug() << "poly (" << x.size() << "):";
    //for( int i = 0; i < x.size(); ++i )
    //{
    //    qDebug() << x[i] << "," << y[i];
    //}

    QString layer = toLayer( aData["layer"] );
    return new CommandPoly(layer, x, y, col);
}

void CommandPoly::save( QTextStream& aStrm, const QGraphicsItem* aItem )
{
    const QGraphicsPolygonItem* poly = qgraphicsitem_cast<const QGraphicsPolygonItem*>(aItem);
    Q_ASSERT( poly != NULL );
    if( !poly )
        return;

    QPolygonF pgon = poly->polygon();
    QColor col = poly->pen().color();

    QByteArray cmd;

    {
        QTextStream ts(&cmd);
        ts << "{\"command\":\"poly\","
           << "\"layer\":\"" << toLayer(aItem) << "\",";

        ts << "\"x\":[";
        for( int i = 0; i < pgon.size(); ++i )
        {
            if( i != 0 )
                ts << ",";
            ts << pgon[i].x();
        }
        ts << "],";

        ts << "\"y\":[";
        for( int i = 0; i < pgon.size(); ++i )
        {
            if( i != 0 )
                ts << ",";
            ts << -pgon[i].y();
        }
        ts << "],";

        ts << "\"color\":" << toCol32( col )
           << "}";

        ts.flush();
    }

    aStrm << cmd;
}

void CommandPoly::execute( SceneManager& aScene )
{        
    QVector<QPointF> pnts;
    pnts.resize(x.size());

    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;

    for( int i = 0; i < x.size(); ++i )
    {
        if( i == 0)
        {
            minx = x[0];
            miny = y[0];
            maxx = x[0];
            maxy = y[0];
        }
        else
        {
            minx = qMin(minx,x[i]);
            miny = qMin(miny,y[i]);
            maxx = qMin(maxx,x[i]);
            maxy = qMin(maxy,y[i]);
        }
        pnts[i] = QPointF(x[i],-y[i]);
    }

    QPolygonF pgon( pnts );
    QGraphicsPolygonItem* item = new QGraphicsPolygonItem( pgon );
    item->setData( SceneManager::DataTypeKey, QVariant(Type) );
    item->setData( SceneManager::DataBound, QVariant( QRectF(QPointF(minx,-miny),QPointF(maxx,-maxy)) ));
    item->setFlag( QGraphicsItem::ItemIsSelectable, true );

    QPen pen;
    QColor color = toQColor( col );
    pen.setColor( color );
    //pen.setCosmetic( false );
    pen.setWidth(0);
    item->setPen( pen );

    QBrush brush( Qt::SolidPattern );
    brush.setColor( color );
    item->setBrush( brush );

    aScene.addItem( item, layer, LayerCategoryGraph );
}
