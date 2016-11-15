// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QList>
#include <QMutex>
#include <QStringList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <bitset>

class QGraphicsScene;
class QGraphicsItem;
class QTextStream;
class QGraphicsPointTextItem;
class QGraphicsItemGroup;

#define Q_GRAPHICSITEM( A )                                 \
    public:                                                 \
        enum { Type = A };                                  \
        virtual int type() const    { return Type; }        \
    private:


enum ELayerCategory {
    LayerCategoryGraph,
    LayerCategoryText,
    LayerCategoryCount
};
typedef std::bitset<LayerCategoryCount> LayerCategoryMask;

QString getLayerCategoryName( ELayerCategory aPart );

struct LayerInfo {
    QString name;
    LayerCategoryMask visibility;

    Qt::CheckState getAllCheckState() const;
};
typedef QList<LayerInfo>    LayerInfoList;


class SceneManager : public QGraphicsScene {
    Q_OBJECT
public:
    enum {
        CmdInvalid,
        CmdReset,       // don't wait for the other commands, drop everything immediatelly and reset to "initial state"
        CmdClear,       // add command to the que and clear when it is processed
        CmdCache,       // save scene into a register
        CmdLine,        // add a line
        CmdRect,        // add a rect
        CmdText,        // add a text
        CmdPoint,       // add a point
        CmdPoly,        // add a polygon
        CmdPolyLine,    // add a polyline

        CmdCount,
    };

    enum {
        DataTypeKey       = 1,
        DataBound         = 2,
        DataLayerKey      = 3,
        DataLayerCategory = 4,
    };

    enum {
        GraphicsText = QGraphicsItem::UserType + 1,
        GraphicsPoint,
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

        static QString toLayer( const QJsonValue& aLayer, QString aDefault = "default" );
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
    void    loadCache( int aId, bool aReset );
    void    save( const QString& aFile ) const;
    void    load( const QString& aFile, bool aReset );

    void    reset();
    void    clear();
    void    clear( QString aLayer );
    QRectF  itemsBoundingRect();
    void    addItem( QGraphicsItem* aItem, const QString& aLayer, ELayerCategory aCategory );

    LayerInfoList   getLayers() const;
    void            setLayerVisibility( const QString& aName, ELayerCategory aCategory, bool aVisible );

    QGraphicsPointTextItem* getTextAt( const QString& aLayer, QPointF aPnt );
    void                    setSelectedAt( const QPointF& aPos );

signals:
    void onAreaPolicyChanged( int aValue );
    void onShowTextChanged( bool aValue );
    void onLayersVisibilityChanged( LayerInfo aName );
    void onLayersChanged();

private:    
    typedef QMap<QPointF,QGraphicsPointTextItem*>   PointToTextMap;
    enum LayerCategory {
        TextCategory,
        GeometryCategory,
        CategoryCount,
    };


    struct LayerData {
        PointToTextMap      textMap;        // to speed up text groupping
        LayerCategoryMask   visibility;

        LayerData();

        void clear();
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
    void        addBBox( QGraphicsItem* aItem );
    void        updateRect();

    LayerData&  createLayer( const QString& aName );
    LayerData*  findLayer( const QString& aName );
    void        updateLayerVisibility();

    static QString getCacheFile( int aId );
};

#endif // SCENEMANAGER_H
