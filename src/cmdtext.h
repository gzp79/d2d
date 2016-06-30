// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDTEXT_H
#define CMDTEXT_H

#include "scenemanager.h"


//todo: handle newline in text

class CommandText : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdText,
    };

    static CommandText* parse( const QJsonObject& aData );
    static void save( QTextStream& aStrm, const QGraphicsItem* aItem );

    CommandText( const QString& aLayer, qreal aX,qreal aY, const QString& aText, quint32 aCol );

    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString layer;
    qreal   x,y;
    QString text;
    quint32 col;

    CommandText();
};


#endif // CMDTEXT_H

