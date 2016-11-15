// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef CMDCTRL_H
#define CMDCTRL_H

#include "scenemanager.h"
#include <QJsonDocument>

class CommandReset : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdReset,
    };

    static CommandReset* parse( const QJsonObject& aData );

    CommandReset();
    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
};

class CommandClear : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdClear,
    };

    static CommandClear* parse( const QJsonObject& aData );

    CommandClear( QString aLayer );
    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    QString layer;
};

class CommandCache : public SceneManager::Command {
public:
    enum {
        Type = SceneManager::CmdCache,
    };

    static CommandCache* parse( const QJsonObject& aData );

    CommandCache( int aId );
    virtual void execute( SceneManager& aScene ) Q_DECL_OVERRIDE;

protected:
    int     id;

    CommandCache();
};


#endif // CMDCTRL_H

