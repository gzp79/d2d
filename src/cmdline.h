// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDLINE_H
#define CMDLINE_H

#include "scenemanager.h"

class CommandLine : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdLine,
    };

    static CommandLine* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandLine( const QString& aLayer, qreal aX0,qreal aY0,qreal aX1,qreal aY1, quint32 aCol );
    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString layer;
    qreal   x0,y0,x1,y1;
    quint32 col;

    CommandLine();
};


#endif // CMDLINE_H

