// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDPOLY_H
#define CMDPOLY_H

#include "scenemanager.h"

class CommandPoly : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdPoly,
    };

    static CommandPoly* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandPoly( const QString& aLayer, const QVector<qreal>& aX, const QVector<qreal>& aY, quint32 aCol );

    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString         layer;
    QVector<qreal>  x,y;
    quint32         col;

    CommandPoly();
};


#endif // CMDRECT_H

