// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QList>
#include <QMutex>
#include <QStringList>
#include <QGraphicsItem>
#include <QGraphicsScene>

class QGraphicsScene;
class QGraphicsItem;
class QTextStream;
class QGraphicsPointText;
class QGraphicsItemGroup;

#define Q_GRAPHICSITEM( A )                                 \
    public:                                                 \
        enum { Type = A };                                  \
        virtual int type() const    { return Type; }        \
    private:


enum ELayerPart {
    LayerPartGraph,
    LayerPartText,
    LayerPartCount
};

QString getLayerPartName( ELayerPart aPart );

struct LayerInfo {
    QString name;
    bool    visible[LayerPartCount];

    Qt::CheckState getAllCheckState() const;
};
typedef QList<LayerInfo>    LayerInfoList;


class SceneManager : public QGraphicsScene {
    Q_OBJECT
public:
    enum {
        CmdInvalid,
        CmdReset,  // don't wait for the other commands, drop everything immediatelly and clear queues
        CmdClear,  // add command to the que and clear when it is processed
        CmdCache,  // save scene into a register
        CmdLine,   // add a line
        CmdRect,   // add a rect
        CmdText,   // add a text
        CmdPoint,  // add a point
        CmdPoly,   // add a polygon

        CmdCount,
    };

    enum {
        DataTypeKey     = 1,
        DataBound       = 2,
        DataLayerKey    = 3,
    };

    enum {
        GraphicsText = QGraphicsItem::UserType + 1,
    };

    enum {
        Area01,
        AreaGlobe,
        AreaAuto,
        AreaInvalid,
    };

    class Command {
    public:
        const int   type;

        virtual ~Command();
        virtual void execute( SceneManager& aScene ) = 0;

    protected:
        Command( int aType );

        static QString toLayer( const QJsonValue& aLayer );
        static QString toLayer( const QGraphicsItem* aItem );
        static int toInt( const QJsonValue& aCol, bool* ok );
        static quint32 toCol32( const QJsonValue& aCol, bool* ok );
        static quint32 toCol32( const QColor& aCol );
        static QColor toQColor( quint32 aCol );
    };

    SceneManager();    

    void    setAreaPolicy( int aArea );
    int     areaPolicy() const                  { return mAreaType; }

    void    addCommand( const QString& aData );
    void    addCommand( Command* aCmd );
    bool    processCommands( int aTimeOut );

    void    saveCache( int aId ) const;
    void    loadCache( int aId, bool aClearPrev );
    void    save( const QString& aFile ) const;
    void    load( const QString& aFile, bool aClearPrev );

    // overriden scene functionality
    void    clear();
    QRectF  itemsBoundingRect();
    void    addItem( QGraphicsItem* aItem, const QString& aLayer, ELayerPart aPart );

    LayerInfoList   getLayers() const;
    void            setLayerVisibility( const QString& aName, ELayerPart aPart, bool aVisible );

    QGraphicsPointText* getTextAt( const QString& aLayer, QPointF aPnt );

signals:
    void onAreaPolicyChanged( int aValue );
    void onShowTextChanged( bool aValue );
    void onLayersVisibilityChanged( LayerInfo aName );
    void onLayersChanged();

private:    
    typedef QMap<QPointF,QGraphicsPointText*>   PointToTextMap;

    struct LayerData {
        QGraphicsItemGroup* parts[LayerPartCount];
        PointToTextMap      textMap;        // to speed up text groupping

        LayerData();

        bool    initParts( QGraphicsScene* aScene );
        void    fillInfo(LayerInfo& aInfo ) const;
    };
    typedef QMap<QString,LayerData>    LayerMap;

    QMutex           mMutex;
    QList<Command*>  mCommandsIn;
    QList<Command*>  mCommandProcess;

    int             mAreaType;
    QPointF         mItemMin, mItemMax;
    QRectF          mFixedRect;
    LayerMap        mLayers;

    void        takeCommands();

    void        releaseQue( QList<Command*>& aQue );
    void        resetBBox();
    void        addBBox( const QPointF& aPnt );
    void        updateRect();

    LayerData&  createLayer( const QString& aName );
    LayerData*  findLayer( const QString& aName );

    static QString getCacheFile( int aId );
};

#endif // SCENEMANAGER_H
