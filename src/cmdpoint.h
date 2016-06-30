// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDPOINT_H
#define CMDPOINT_H

#include "scenemanager.h"


//todo: handle newline in text

class CommandPoint : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdPoint,
    };

    static CommandPoint* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandPoint( const QString& aLayer, qreal aX,qreal aY, quint32 aCol );

    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString layer;
    qreal   x,y;
    quint32 col;

    CommandPoint();
};


#endif // CMDTEXT_H

