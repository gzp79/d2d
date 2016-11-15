// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "qgraphwidget.h"
#include "scenemanager.h"

#include <math.h>

#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QPixmap>
#include <QLinearGradient>
#include <QScrollBar>
#include <QApplication>

QGraphWidget::QGraphWidget(QWidget* aParent)
    : QGraphicsView(aParent)
    , mSceneManager( NULL )
    , mBackground( NULL )
    , mBackgroundType( BackInvalid )    
    , mAutoFit( false )
    , mHighQuality( false )
{

    setCacheMode(CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);
    setRenderHint(QPainter::TextAntialiasing);

    setInteractive(false);
    setDragMode(ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );    

    mTimer = new QTimer(this);
    mTimer->setInterval(100);
    connect( mTimer, SIGNAL(timeout()), this, SLOT(updateCommands()));
    mTimer->start();    
}

QGraphWidget::~QGraphWidget()
{
    if( mBackground )
    {
        delete mBackground;
        mBackground = NULL;
    }
}

void QGraphWidget::setSceneManager( SceneManager* aSceneManager )
{
    mSceneManager = aSceneManager;
    setScene( mSceneManager );

    setBackground( BackBlack );
}

void QGraphWidget::setAutoFit( bool aFit )
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

void QGraphWidget::keyPressEvent( QKeyEvent* aEvent )
{
    setAutoFit( false );
    QGraphicsView::keyPressEvent(aEvent);
}

void QGraphWidget::enterEvent( QEvent* aEvent )
{
    QGraphicsView::enterEvent( aEvent );
    viewport()->setCursor(Qt::ArrowCursor);
}

void QGraphWidget::mousePressEvent( QMouseEvent* aEvent )
{
    if( aEvent->button() == Qt::RightButton )
    {
        QGraphicsItem* itemUnderMouse = itemAt( aEvent->pos() );
        scene()->clearSelection();
        if( itemUnderMouse )
            itemUnderMouse->setSelected(true);
        scene()->invalidate(); // workaround as selection change does not result an invalidation sometimes
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

void QGraphWidget::mouseReleaseEvent( QMouseEvent* aEvent )
{
    QGraphicsView::mouseReleaseEvent( aEvent );
    viewport()->setCursor( Qt::ArrowCursor );
}

#ifndef QT_NO_WHEELEVENT
void QGraphWidget::wheelEvent(QWheelEvent* aEvent)
{
    qreal sc = pow(1.25, aEvent->delta() / 240.);
    zoomToPixel(aEvent->pos(), sc);
    setAutoFit( false );
}
#endif

void QGraphWidget::zoom(qreal scaleFactor)
{
    scale(scaleFactor, scaleFactor);
}

void QGraphWidget::releaseBackground()
{
    if( mBackground )
    {
        delete mBackground;
        mBackground = NULL;
    }
}

void QGraphWidget::setHighQuality( bool aEnable )
{
    if( mHighQuality == aEnable )
        return;

    mHighQuality = aEnable;
    setRenderHint(QPainter::Antialiasing, aEnable);
    setRenderHint(QPainter::TextAntialiasing, aEnable);
    setRenderHint(QPainter::HighQualityAntialiasing, aEnable);

    emit onHighQualityChanged(mHighQuality);
}

void QGraphWidget::setBackground( int aType )
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

void QGraphWidget::clear()
{
    mSceneManager->reset();
}

void QGraphWidget::zoomIn()
{
    qreal f = qreal(1.2);
    scale( f, f );
    setAutoFit( false );
}

void QGraphWidget::zoomOut()
{
    qreal f = 1/qreal(1.2);
    scale( f, f );
    setAutoFit( false );
}

void QGraphWidget::zoomToPixel( const QPoint& aViewPos, qreal aZoom )
{
    QTransform tr;


    QPoint  pos  = aViewPos;
    QPointF posf = mapToScene(aViewPos);

    //centerOn( aScenePoint );
    scale( aZoom, aZoom );
    double w = this->viewport()->width();
    double h = this->viewport()->height();

    double wf = mapToScene(QPoint(w-1, 0)).x() - mapToScene(QPoint(0,0)).x();
    double hf = mapToScene(QPoint(0, h-1)).y() - mapToScene(QPoint(0,0)).y();

    double lf = posf.x() - pos.x() * wf / w;
    double tf = posf.y() - pos.y() * hf / h;

    ensureVisible(lf, tf, wf, hf, 0, 0);

    QPointF newPos = mapToScene(pos);

    ensureVisible(QRectF(QPointF(lf, tf) - newPos + posf, QSizeF(wf, hf)), 0, 0);
}

void QGraphWidget::zoomFit()
{
    fitInView( mSceneManager->itemsBoundingRect(), Qt::KeepAspectRatio );
}

void QGraphWidget::updateCommands()
{
    if( mSceneManager )
    {
         if( mSceneManager->processCommands( -1 ) && mAutoFit )
         {
             zoomFit();
         }
    }
}

void QGraphWidget::drawBackground( QPainter* aPainter, const QRectF& aRect )
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

