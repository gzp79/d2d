// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDPOLYLINE_H
#define CMDPOLYLINE_H

#include "scenemanager.h"

class CommandPolyLine : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdPolyLine,
    };

    static CommandPolyLine* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandPolyLine( const QString& aLayer, const QVector<qreal>& aX, const QVector<qreal>& aY, quint32 aCol );

    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString         layer;
    QVector<qreal>  x,y;
    quint32         col;

    CommandPolyLine();
};


#endif // CMDPOLYLINE_H

