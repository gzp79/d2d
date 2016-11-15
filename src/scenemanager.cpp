// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "scenemanager.h"

#include <limits>
#include <QtGlobal>
#include <QDebug>
#include <QElapsedTimer>
#include <QMutexLocker>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDataStream>
#include <QMessageBox>
#include <QMap>
#include <QGraphicsItemGroup>
#include "qgraphicspointtextitem.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "cmdctrl.h"
#include "cmdline.h"
#include "cmdrect.h"
#include "cmdtext.h"
#include "cmdpoint.h"
#include "cmdpoly.h"
#include "cmdpolyline.h"


template <> inline bool qMapLessThanKey(const QPointF& key1, const QPointF& key2)
{
    return key1.x() != key2.x() ? key1.x() < key2.x() : key1.y() < key2.y();
}

QString getLayerCategoryName( ELayerCategory aCategory )
{
    switch( aCategory )
    {
        case LayerCategoryGraph: return "graph";
        case LayerCategoryText:  return "text";

        default:
            Q_ASSERT(false);
    }

    return "error";
}

Qt::CheckState LayerInfo::getAllCheckState() const
{
    int cnt = 0;
    for( int i = 0; i < LayerCategoryCount; ++i )
    {
        if( visibility[i] )
            cnt++;
    }
    Qt::CheckState st = Qt::PartiallyChecked;
    if( cnt == 0 ) st = Qt::Unchecked;
    if( cnt == LayerCategoryCount ) st = Qt::Checked;

    return st;
}

SceneManager::Command::Command( int aType )
    : type( aType )
{
}

SceneManager::Command::~Command()
{
}

QString SceneManager::Command::toLayer( const QJsonValue& aLayer, QString aDefault )
{
    QString layer = aLayer.toString();
    if( layer.isEmpty() )
        layer = aDefault;

    return layer;
}

QString SceneManager::Command::toLayer( const QGraphicsItem* aItem )
{
    QVariant vr = aItem->data( DataLayerKey );
    if( vr.type() != QVariant::String )
        return QString("default");

    return vr.toString();
}

int SceneManager::Command::toInt( const QJsonValue& aCol, bool* ok )
{
    if( ok )
        *ok = true;

    if( aCol.isString() )
        return aCol.toString().toUInt( ok, 0 );
    if( aCol.isDouble() )
        return static_cast<int>(aCol.toDouble());

    if( ok )
        *ok = false;
    return 0;
}

quint32 SceneManager::Command::toCol32( const QJsonValue& aCol, bool* ok )
{
    if( ok )
        *ok = true;

    if( aCol.isString() )
        return aCol.toString().toUInt( ok, 0 );
    if( aCol.isDouble() )
        return (quint32)(aCol.toDouble());

    return 0xffffffff;
}

quint32 SceneManager::Command::toCol32( const QColor& aCol )
{
    quint32 c32 = ((quint32)aCol.alpha() << 24) + ((quint32)aCol.red() << 16) + ((quint32)aCol.green() << 8) + ((quint32)aCol.blue());
    return c32;
}

QColor SceneManager::Command::toQColor( quint32 aCol )
{
    return QColor( (aCol >> 16) & 0xff,
                   (aCol >> 8) & 0xff,
                    aCol & 0xff,
                   (aCol >> 24) & 0xff );
}

SceneManager::LayerData::LayerData()
{
    visibility.set();
}

void SceneManager::LayerData::clear()
{
    textMap.clear();
}

SceneManager::SceneManager()
    : mAreaType( AreaInvalid )
{
    //mGraphicsScene = new QGraphicsScene( this );
    setItemIndexMethod(QGraphicsScene::NoIndex);
    //mGraphicsScene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setBackgroundBrush(QBrush(QColor(128,128,128,255)));

    resetBBox();
    setAreaPolicy( AreaAuto );
}

void SceneManager::setAreaPolicy( int aType )
{
    if( mAreaType == aType )
        return;

    mAreaType = aType;
    switch( mAreaType )
    {
        default:
        case AreaAuto:  mFixedRect = QRectF(); break;
        case Area01:    mFixedRect = QRectF(0,0,1,1); break;
        case AreaGlobe: mFixedRect = QRectF(-180,-90, 360,180); break;

    }

    updateRect();
    emit onAreaPolicyChanged(mAreaType);
}

void SceneManager::addCommand( const QString& aData )
{
    //create command from string
    // a String my contain multiple
    QByteArray json_data = aData.toUtf8();
    int msgCount = 0;

    int start = 0;
    int startRetry = 0;
    int end = 0;
    int tryCount = 0;
    while( (end = json_data.indexOf( '}', startRetry)) >= 0 )
    {
        QJsonParseError error;
        QByteArray subData = json_data.mid( start, end - start + 1);
        QJsonDocument jdoc = QJsonDocument::fromJson( subData, &error );
        if( error.error != QJsonParseError::NoError )
        {
            qDebug() << "json error in (" << end << "): '" << QString::fromUtf8(subData) << "'";
            qDebug() << "json error: " << error.errorString();
            qDebug() << "json error at: " << error.offset << "(" << aData.left(error.offset) << ")";
            ++startRetry;
            ++tryCount;
            continue;
        }

        Command* cmd = NULL;
        if( (cmd = CommandClear::parse(jdoc.object())) != NULL )            addCommand(cmd);
        else if( (cmd = CommandReset::parse(jdoc.object())) != NULL )       addCommand(cmd);
        else if( (cmd = CommandCache::parse(jdoc.object())) != NULL )       addCommand(cmd);
        else if( (cmd = CommandLine::parse(jdoc.object())) != NULL )        addCommand(cmd);
        else if( (cmd = CommandRect::parse(jdoc.object())) != NULL )        addCommand(cmd);
        else if( (cmd = CommandText::parse(jdoc.object())) != NULL )        addCommand(cmd);
        else if( (cmd = CommandPoint::parse(jdoc.object())) != NULL )       addCommand(cmd);
        else if( (cmd = CommandPoly::parse(jdoc.object())) != NULL )        addCommand(cmd);
        else if( (cmd = CommandPolyLine::parse(jdoc.object())) != NULL )    addCommand(cmd);
        else                                                                qDebug() << "invalid command: " << aData;

        tryCount = 0;
        start = end + 1;
        startRetry = start;
        ++msgCount;
    }

    // qDebug() << "number of extracted json commands: " << msgCount;
    if( tryCount > 0 )
    {
        qDebug() << "give up json parsing of '" << QString::fromUtf8(json_data) << "'";
    }
}

void SceneManager::addCommand( Command* aCmd )
{
    if( !aCmd )
        return;

    QMutexLocker ml( &mMutex );
    if( aCmd->type == CmdReset )
    {
        // drop everything
        releaseQue( mCommandsIn );
    }
    mCommandsIn.push_back( aCmd );
}

static inline bool isTimeout( QElapsedTimer& aTimer, int aTimeOut )
{
    if( aTimeOut < 0 )
        return false;

    return aTimer.elapsed() >= aTimeOut;
}

bool SceneManager::processCommands( int aTimeOut )
{
    takeCommands();

    if( mCommandProcess.empty() )
        return true;

    QElapsedTimer timer;
    timer.start();

    while( !mCommandProcess.empty() && !isTimeout(timer, aTimeOut) )
    {
        Command* cmd = mCommandProcess.front();
        mCommandProcess.pop_front();
        cmd->execute( *this );
        delete cmd;
    }

    updateRect();
    return mCommandProcess.empty();
}

QString SceneManager::getCacheFile( int aId )
{
#ifdef Q_OS_WIN
    QString appDir = QDir::homePath() + "/Application Data/qtViewer/";
#elif defined(Q_OS_LINUX)
    QString appDir = QDir::homePath() + "/.qtViewer/";
#endif

    QString file;
    QDir dir(appDir);
    dir.mkpath( appDir );

    QTextStream( &file ) << aId << ".cache";
    file = dir.absoluteFilePath( file );
    return file;
}

void SceneManager::saveCache( int aId ) const
{
    QString file = getCacheFile( aId );
    QApplication::setOverrideCursor(Qt::WaitCursor);
    save( file );
    QApplication::restoreOverrideCursor();
}

void SceneManager::loadCache( int aId, bool aReset )
{
    QString file = getCacheFile( aId );
    QApplication::setOverrideCursor(Qt::WaitCursor);
    load( file, aReset );
    QApplication::restoreOverrideCursor();
}

void SceneManager::save( const QString& aFile ) const
{
    QFile file(aFile);
    if( !file.open(QFile::WriteOnly | QFile::Truncate ))
    {
        QString str = "Could not create " + aFile;
        QMessageBox::critical(NULL, tr("Save error"), str );
        return;
    }

    QTextStream strm( &file );

    QList<QGraphicsItem*> itemList = items();
    for( QList<QGraphicsItem*>::const_iterator it = itemList.begin(); it != itemList.end(); ++it )
    {
        QGraphicsItem* item = *it;
        int type = item->type();
        bool ok = false;
        int cmdType = item->data( DataTypeKey ).toInt( &ok );

        if( !ok || cmdType >= CmdCount )
        {
            // non-serailizable
            continue;
        }

        switch( cmdType )
        {
            case CommandLine::Type:     CommandLine::save( strm, item ); break;
            case CommandRect::Type:     CommandRect::save( strm, item ); break;
            case CommandText::Type:     CommandText::save( strm, item ); break;
            case CommandPoint::Type:    CommandPoint::save( strm, item ); break;
            case CommandPoly::Type:     CommandPoly::save( strm, item ); break;
            case CommandPolyLine::Type: CommandPolyLine::save( strm, item ); break;
            default:
                qDebug() << "no serializer for " << type;
        }
    }

    file.close();
}

void SceneManager::load( const QString& aFile, bool aReset )
{
    if( aReset )
        addCommand( new CommandReset() );

    QFile file(aFile);
    if( !file.open(QFile::ReadOnly) )
    {
        QString str = "Could not open " + aFile;
        QMessageBox::critical(NULL, tr("Load error"), str );
        return;
    }

    QTextStream strm( &file );
    addCommand( strm.readAll() );
}

void SceneManager::reset()
{
    resetBBox();
    mLayers.clear();
    QGraphicsScene::clear();
    emit onLayersChanged();
}

void SceneManager::clear()
{
    resetBBox();
    for( LayerMap::iterator it = mLayers.begin(); it != mLayers.end(); ++it )
    {
        it.value().clear();
    }
    QGraphicsScene::clear();
}


void SceneManager::clear( QString aLayer )
{
    LayerData* layer = findLayer( aLayer );
    if( !layer )
        return;

    resetBBox();
    layer->clear();
    QList<QGraphicsItem*> allItems = items();
    for( QList<QGraphicsItem*>::iterator it = allItems.begin(); it != allItems.end(); ++it )
    {
        QGraphicsItem* item = *it;
        QString name = item->data( DataLayerKey ).toString();
        if( name == aLayer )
            removeItem( item );
          else
            addBBox( item );
    }
    updateRect();
}

QRectF SceneManager::itemsBoundingRect()
{
    if( mItemMax.x() >= mItemMin.x() )
    {
        QRectF rect;
        rect.setCoords( mItemMin.x(), mItemMin.y(), mItemMax.x(), mItemMax.y() );
        return rect;
    }

    return QGraphicsScene::itemsBoundingRect();
}

void SceneManager::addItem( QGraphicsItem* aItem, const QString& aLayer, ELayerCategory aCategory )
{
    addBBox( aItem );
    aItem->setData(DataLayerKey, aLayer);
    aItem->setData(DataLayerCategory, (int)aCategory);

    LayerData& layer = createLayer( aLayer );
    QGraphicsPointTextItem* textItem = qgraphicsitem_cast<QGraphicsPointTextItem*>(aItem);
    if( textItem )
    {
        layer.textMap.insert( aItem->pos(), textItem);
        textItem->setVisible( layer.visibility[TextCategory]);
    }
    else
    {
       aItem->setVisible( layer.visibility[GeometryCategory]);
    }

    QGraphicsScene::addItem( aItem );
}

LayerInfoList SceneManager::getLayers() const
{
    LayerInfoList lst;
    for( LayerMap::const_iterator it = mLayers.begin(); it != mLayers.end(); ++it )
    {
       const LayerData& layer = it.value();
       LayerInfo info;
       info.name = it.key();
       info.visibility = layer.visibility;
       lst.push_back( info );
    }
    return lst;
}

void SceneManager::setLayerVisibility( const QString& aName, ELayerCategory aCategory, bool aVisible )
{
    LayerData* layer = findLayer( aName );
    if( !layer || aCategory < 0 )
        return;

    bool changed = false;
    if( aCategory >= LayerCategoryCount )
    {
        if( aVisible )
        {
            changed = ~(layer->visibility).any();
            layer->visibility.set();
        }
        else
        {
            changed = layer->visibility.any();
            layer->visibility.reset();
        }
    }
    else
    {
        changed = layer->visibility.test(aCategory) != aVisible;
        layer->visibility.set(aCategory, aVisible);
    }

    if( changed )
    {
        updateLayerVisibility();
        LayerInfo info;
        info.name = aName;
        info.visibility = layer->visibility;
        emit onLayersVisibilityChanged( info );
    }
}

QGraphicsPointTextItem* SceneManager::getTextAt( const QString& aLayer, QPointF aPnt )
{
    LayerData* layer = findLayer( aLayer );
    if( !layer )
        return NULL;

    QMap<QPointF,QGraphicsPointTextItem*>::iterator it = layer->textMap.find( aPnt );
    if( it == layer->textMap.end() )
        return NULL;
    return it.value();
}

void SceneManager::setSelectedAt( const QPointF& aPos )
{
  QList<QGraphicsItem*> newSelection = items( aPos );
  QList<QGraphicsItem*> selection = selectedItems();
  for( QList<QGraphicsItem*>::iterator it = selection.begin(); it != selection.end(); ++it )
  {
      if( !newSelection.contains(*it) )
          (*it)->setSelected( false );
  }

  for( QList<QGraphicsItem*>::iterator it = newSelection.begin(); it != newSelection.end(); ++it )
  {
      QGraphicsItem* item = *it;
      if( item->data(DataLayerKey).toString() != "$layer" )
            (*it)->setSelected( true );
  }
}

void SceneManager::takeCommands()
{
    QMutexLocker ml( &mMutex );

    if( mCommandsIn.empty() )
        return;

    if( mCommandsIn.front()->type == CmdReset )
    {
        reset();
        releaseQue( mCommandProcess );
        mCommandsIn.pop_front();
        mCommandProcess.swap( mCommandsIn );
        updateRect();
        return;
    }

    mCommandProcess += mCommandsIn;
    mCommandsIn.clear();
}

void SceneManager::releaseQue( QList<Command*>& aQue )
{
    for( QList<Command*>::iterator it = aQue.begin(); it != aQue.end(); ++it )
    {
        delete *it;
    }
    aQue.clear();
}

void SceneManager::resetBBox()
{
    mItemMin.rx() = std::numeric_limits<qreal>::max();
    mItemMin.ry() = std::numeric_limits<qreal>::max();
    mItemMax.rx() = -std::numeric_limits<qreal>::max();
    mItemMax.ry() = -std::numeric_limits<qreal>::max();
}

void SceneManager::addBBox( const QPointF& aPnt )
{
    mItemMin.rx() = qMin( aPnt.x(), mItemMin.x() );
    mItemMin.ry() = qMin( aPnt.y(), mItemMin.y() );
    mItemMax.rx() = qMax( aPnt.x(), mItemMax.x() );
    mItemMax.ry() = qMax( aPnt.y(), mItemMax.y() );
}


void SceneManager::addBBox( QGraphicsItem* aItem )
{
    QVariant r = aItem->data( DataBound );
    if( r.type() == QVariant::RectF )
    {
        QRectF rect = r.toRectF();
        addBBox( rect.topLeft() );
        addBBox( rect.bottomRight() );
    }
    else if( r.type() == QVariant::PointF )
    {
        QPointF pnt = r.toPointF();
        addBBox( pnt );
    }
}


void SceneManager::updateRect()
{   
    QRectF rect = mFixedRect;
    if( rect == QRectF() && mItemMax.x() >= mItemMin.x() )
    {
        rect.setCoords( mItemMin.x(), mItemMin.y(), mItemMax.x(), mItemMax.y() );
        QPointF d = (mItemMax - mItemMin);
        d *= 0.025;
        rect.adjust( -d.x(), -d.y(), d.x(), d.y() );
    }
    setSceneRect( rect );
}

SceneManager::LayerData& SceneManager::createLayer( const QString& aName )
{
    LayerMap::iterator it = mLayers.find( aName );
    if( it == mLayers.end() ) {
        // qDebug() << "creating new layer " << aName;
        it = mLayers.insert( it, aName, LayerData() );
        emit onLayersChanged();
    }
    return it.value();
}

SceneManager::LayerData* SceneManager::findLayer( const QString& aName )
{
    LayerMap::iterator it = mLayers.find(aName);
    if( it == mLayers.end() )
        return NULL;

    return &it.value();
}

void SceneManager::updateLayerVisibility()
{
    QList<QGraphicsItem*> allItems = items();
    for( QList<QGraphicsItem*>::iterator it = allItems.begin(); it != allItems.end(); ++it )
    {
        QGraphicsItem* item = *it;
        QString name = item->data( DataLayerKey ).toString();
        int cat = item->data( DataLayerCategory ).toInt();
        bool visible = true;
        LayerData* layer = findLayer( name );
        if( !layer || cat < 0 || cat > LayerCategoryCount )
        {
            visible = false;
            continue;
        }

        visible = ( cat == LayerCategoryCount || layer->visibility.test( cat ) );
        item->setVisible( visible );
        if( !visible )
            item->setSelected( false );
    }
}

