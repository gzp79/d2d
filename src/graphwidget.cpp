// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "graphwidget.h"
#include "scenemanager.h"

#include <math.h>

#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QPixmap>
#include <QLinearGradient>
#include <QScrollBar>
#include <QApplication>

GraphWidget::GraphWidget(QWidget* aParent)
    : QGraphicsView(aParent)
    , mSceneManager( NULL )
    , mBackground( NULL )
    , mBackgroundType( BackInvalid )    
    , mAutoFit( false )
    , mHighQuality( false )
{
    //setInteractive(false); // disable selection / item movement, but wheel-scroll is not working
    setInteractive(true);
    setCacheMode(CacheBackground);
    //setViewportUpdateMode(BoundingRectViewportUpdate);
    setViewportUpdateMode(FullViewportUpdate);
    //setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    //setRenderHint(QPainter::HighQualityAntialiasing);

    setDragMode(ScrollHandDrag);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);    
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );    


    mTimer = new QTimer(this);
    mTimer->setInterval(100);
    connect( mTimer, SIGNAL(timeout()), this, SLOT(updateCommands()));
    mTimer->start();    
}

GraphWidget::~GraphWidget()
{
    if( mBackground )
    {
        delete mBackground;
        mBackground = NULL;
    }
}

void GraphWidget::setSceneManager( SceneManager* aSceneManager )
{
    mSceneManager = aSceneManager;
    setScene( mSceneManager );

    setBackground( BackBlack );
}

void GraphWidget::setAutoFit( bool aFit )
{    
    if( mAutoFit == aFit )
        return;

    mAutoFit = aFit;
    if( mAutoFit )
    {
        zoomFit();
    }

    emit onAutoFitChanged(mAutoFit);
}

void GraphWidget::keyPressEvent( QKeyEvent* aEvent )
{
    setAutoFit( false );
    QGraphicsView::keyPressEvent(aEvent);
}

void GraphWidget::enterEvent( QEvent* aEvent )
{
    QGraphicsView::enterEvent( aEvent );
    viewport()->setCursor(Qt::ArrowCursor);
}

void GraphWidget::mousePressEvent( QMouseEvent* aEvent )
{
    if( aEvent->button() == Qt::RightButton )
    {
        QGraphicsItem* itemUnderMouse = itemAt( aEvent->pos() );
        scene()->clearSelection();
        if( itemUnderMouse )
            itemUnderMouse->setSelected(true);
        aEvent->accept();
        return;
    }

    // the same condition as in QGraphicsView as there is no reliable signal for ScrollHandDrag start/stop
    if( dragMode() == QGraphicsView::ScrollHandDrag && aEvent->button() == Qt::LeftButton )
    {
        setAutoFit( false );
    }
    QGraphicsView::mousePressEvent( aEvent );
    viewport()->setCursor(Qt::ArrowCursor);
}

void GraphWidget::mouseReleaseEvent( QMouseEvent* aEvent )
{
    QGraphicsView::mouseReleaseEvent( aEvent );
    viewport()->setCursor( Qt::ArrowCursor );
}

#ifndef QT_NO_WHEELEVENT
void GraphWidget::wheelEvent(QWheelEvent* aEvent)
{
    qreal sc = pow(1.25, aEvent->delta() / 240.);
    zoom(sc);
    setAutoFit( false );
}
#endif

void GraphWidget::zoom(qreal scaleFactor)
{
    scale(scaleFactor, scaleFactor);
}

void GraphWidget::releaseBackground()
{
    if( mBackground )
    {
        delete mBackground;
        mBackground = NULL;
    }
}

void GraphWidget::setHighQuality( bool aEnable )
{
    if( mHighQuality == aEnable )
        return;

    mHighQuality = aEnable;
    setRenderHint(QPainter::Antialiasing, aEnable);
    setRenderHint(QPainter::TextAntialiasing, aEnable);
    setRenderHint(QPainter::HighQualityAntialiasing, aEnable);

    emit onHighQualityChanged(mHighQuality);
}

void GraphWidget::setBackground( int aType )
{
    if( mBackgroundType == aType )
        return;

    mBackgroundType = aType;
    releaseBackground();

    switch( mBackgroundType )
    {
        default:

        case BackBlack:
        {
            mBackgroundColor = QColor(0,0,0,255);
            mBackgroundBorder= QColor(128,128,128,255);
        } break;

        case BackWhite:
        {
            mBackgroundColor = QColor(255,255,255,255);
            mBackgroundBorder= QColor(0,0,0,255);
        } break;

        case BackGlobe:
        {
            mBackgroundColor = QColor(0,0,0,255);
            mBackgroundBorder= QColor(128,128,128,255);

            QApplication::setOverrideCursor(Qt::WaitCursor);
            mBackground = new QPixmap("earth.png");
            QApplication::restoreOverrideCursor();
        } break;

        case BackGradient:
        {
            mBackgroundColor = QColor(0,0,0,255);
            mBackgroundBorder= QColor(0,0,0,255);
        } break;
    }

    invalidateScene( scene()->sceneRect(), QGraphicsScene::BackgroundLayer );
    update();
    emit onBackgroundChanged(mBackgroundType);
}

void GraphWidget::clear()
{
    mSceneManager->clear();
}

void GraphWidget::zoomIn()
{
    zoom(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    zoom(1 / qreal(1.2));
}

void GraphWidget::zoomFit()
{
    fitInView( mSceneManager->itemsBoundingRect(), Qt::KeepAspectRatio );
}

void GraphWidget::updateCommands()
{
    if( mSceneManager )
    {
         if( mSceneManager->processCommands( -1 ) && mAutoFit )
         {
             zoomFit();
         }
    }
}

void GraphWidget::drawBackground( QPainter* aPainter, const QRectF& aRect )
{
    QGraphicsView::drawBackground( aPainter, aRect );

    QRectF scRect = sceneRect();
    QBrush brush( mBackgroundColor );
    QPen pen( mBackgroundBorder );
    pen.setWidth(0);

    if( mBackgroundType == BackGlobe && mBackground != NULL )
    {
        QRectF source = mBackground->rect();
        aPainter->setRenderHint( QPainter::SmoothPixmapTransform );
        aPainter->drawPixmap( scRect, *mBackground, source );
        aPainter->setPen( pen );
        aPainter->drawRect(scRect);
    }
    else if( mBackgroundType == BackGradient )
    {
        QRectF scRect = sceneRect();
        QLinearGradient gradient(scRect.topLeft(), scRect.bottomRight());
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, Qt::lightGray);
        aPainter->fillRect( aRect.intersected(scRect), gradient );
        aPainter->setBrush(Qt::NoBrush);
        aPainter->setPen( pen );
        aPainter->drawRect(scRect);
    }
    /*else if( mBackgroundType == BackGrid4x4)
    {
        QRectF scRect = sceneRect();
        QLinearGradient gradient(scRect.topLeft(), scRect.bottomRight());
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, Qt::lightGray);
        aPainter->fillRect( aRect.intersected(scRect), gradient );
        aPainter->setBrush(Qt::NoBrush);
        aPainter->drawRect(scRect);
    }*/
    else
    {
        aPainter->setPen(pen);
        aPainter->setBrush(brush);
        aPainter->drawRect(scRect);
    }
}

