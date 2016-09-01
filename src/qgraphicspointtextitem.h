// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef QGRAPHICSPOINTTEXTITEM
#define QGRAPHICSPOINTTEXTITEM

#include "scenemanager.h"
#include <QGraphicsTextItem>

class QGraphicsPointTextItem : public QGraphicsTextItem
{
    Q_GRAPHICSITEM( SceneManager::GraphicsText )

public:
    struct Data
    {
        QColor color;
        QString text;

        Data()
            : color()
            , text()
        {
        }

        Data( const QColor aCol, const QString& aText )
            : color( aCol )
            , text( aText )
        {
        }
    };
    typedef QVector<Data> DataVector;

    QGraphicsPointTextItem( const QString& aStr, const QColor& aCol );

    void addText( const QString& aText, const QColor& aCol );
    const DataVector& getData() const { return mData; }

protected:

    DataVector mData;

    virtual void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aStyleOpt, QWidget* aWidget ) Q_DECL_OVERRIDE;
    QString genHtml() const;
};

#endif // QGRAPHICSPOINTTEXTITEM

