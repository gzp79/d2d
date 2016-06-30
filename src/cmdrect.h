// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDRECT_H
#define CMDRECT_H

#include "scenemanager.h"

class CommandRect : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdRect,
    };

    static CommandRect* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandRect( const QString& aLayer, qreal aX0,qreal aY0,qreal aX1,qreal aY1, quint32 aCol );

    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString layer;
    qreal   x0,y0,x1,y1;
    quint32 col;

    CommandRect();
};


#endif // CMDRECT_H

