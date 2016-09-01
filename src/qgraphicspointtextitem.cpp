// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "qgraphicspointtextitem.h"

#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include "scenemanager.h"

QGraphicsPointTextItem::QGraphicsPointTextItem( const QString& aText, const QColor& aCol )
    : QGraphicsTextItem()
{
    mData.append( Data(aCol, aText) );
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setDefaultTextColor( aCol );
    this->setHtml( genHtml() );

}

void QGraphicsPointTextItem::addText( const QString& aText, const QColor& aCol )
{
    mData.append( Data(aCol, aText) );
    this->setHtml( genHtml() );
    update();
}

void QGraphicsPointTextItem::paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aStyleOpt, QWidget* aWidget )
{
    QGraphicsTextItem::paint( aPainter,aStyleOpt,aWidget );

    static const QColor white( 255, 255, 255, 255 );
    QColor col = mData.empty() ? white : mData[0].color;
    aPainter->setBrush( col );
    aPainter->setPen( col );
    aPainter->drawEllipse( QRectF(-1.5, -1.5, 3, 3) );

    if( isSelected() )
    {
        aPainter->setBrush( QBrush() );
        aPainter->drawEllipse( QRectF(-5, -5, 10, 10) );
    }
}

QString QGraphicsPointTextItem::genHtml() const
{
    QString html = "<html><body>";
    bool first = true;
    for( DataVector::const_iterator it = mData.begin(); it != mData.end(); ++it )
    {
        if( !first )
            html += "<br/>";
        first = false;
        html += "<font color=\"" + it->color.name()+ "\">";
        html += it->text;
        html += "</font>";

    }
    html += "</body></html>";
    return html;
}
