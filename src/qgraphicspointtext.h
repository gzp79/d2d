// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef QGRAPHICSPOINTTEXT
#define QGRAPHICSPOINTTEXT

#include "scenemanager.h"
#include <QGraphicsTextItem>

class QGraphicsPointText : public QGraphicsTextItem
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

    QGraphicsPointText( const QString& aStr, const QColor& aCol );

    void addText( const QString& aText, const QColor& aCol );
    const DataVector& getData() const { return mData; }

protected:

    DataVector mData;

    virtual void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aStyleOpt, QWidget* aWidget ) Q_DECL_OVERRIDE;
    QString genHtml() const;
};

#endif // QGRAPHICSPOINTTEXT

