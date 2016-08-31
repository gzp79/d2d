// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdtext.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include "qgraphicspointtext.h"
#include <QJsonObject>
#include <QJsonValue>

CommandText::CommandText()
    : SceneManager::Command( Type )
{
}

CommandText::CommandText( const QString& aLayer, qreal aX,qreal aY, const QString& aText, quint32 aCol )
    : SceneManager::Command( Type )
    , layer( aLayer )
    , x(aX)
    , y(aY)
    , text( aText )
    , col( aCol )
{
}

CommandText* CommandText::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "text" )
        return NULL;

    QJsonValue vx = aData["x"];
    if( !vx.isDouble() ) { qDebug() << "error in text command, invalid x parameter: " << vx.toString(); return NULL; }
    qreal x = vx.toDouble();

    QJsonValue vy = aData["y"];
    if( !vy.isDouble() ) { qDebug() << "error in text command, invalid y parameter: " << vy.toString(); return NULL; }
    qreal y = vy.toDouble();

    bool ok = true;
    QJsonValue vcol = aData["color"];
    quint32 col = toCol32( vcol, &ok );
    if( !ok ) { qDebug() << "error in text command, invalid color parameter: " << vcol.toString(); return NULL; }

    QString layer = toLayer( aData["layer"] );
    return new CommandText(layer, x,y,aData["text"].toString(),col);
}

void CommandText::save( QTextStream& aStrm, const QGraphicsItem* aItem )
{
    const QGraphicsPointText* text = qgraphicsitem_cast<const QGraphicsPointText*>(aItem);
    Q_ASSERT( text != NULL );
    if( !text )
        return;

    const QGraphicsPointText::DataVector& data = text->getData();

    QByteArray cmd;
    QTextStream ts(&cmd);
    for( QGraphicsPointText::DataVector::const_iterator it = data.begin(); it != data.end(); ++it )
    {
        ts << "{\"command\":\"text\","
            << "\"layer\":\"" << toLayer(aItem) << "\","
            << "\"x\":" << text->x() << ","
            << "\"y\":" << -text->y() << ","
            << "\"text\":\"" << it->text << "\","
            << "\"color\":" << toCol32( it->color )
            << "}\n";
    }
    ts.flush();
    aStrm << cmd;
}

void CommandText::execute( SceneManager& aScene )
{
    QGraphicsPointText* item = aScene.getTextAt( layer, QPointF(x,-y) );
    if( item )
    {        
        item->addText( text, toQColor(col) );
        return;
    }

    item = new QGraphicsPointText( text, toQColor( col ) );
    item->setData( SceneManager::DataTypeKey, QVariant(Type) );
    item->setData( SceneManager::DataBound, QVariant( QPointF(x,-y) ));
    item->setFlag( QGraphicsItem::ItemIsSelectable, true );

    item->setPos( x,-y );    
    item->setZValue( 1 );

    aScene.addItem( item, layer, LayerPartText );
}
