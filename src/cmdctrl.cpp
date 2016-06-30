// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "cmdctrl.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonValue>

CommandReset::CommandReset()
    : SceneManager::Command( Type )
{
}

CommandReset* CommandReset::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "reset" )
        return NULL;

    return new CommandReset();
}

void CommandReset::execute( SceneManager& aScene )
{
    aScene.clear();
}

CommandClear::CommandClear()
    : SceneManager::Command( Type )
{
}

CommandClear* CommandClear::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "clear" )
        return NULL;

    return new CommandClear();
}

void CommandClear::execute( SceneManager& aScene )
{
    aScene.clear();
}

CommandCache::CommandCache()
    : SceneManager::Command( Type )
{
}

CommandCache::CommandCache( int aId )
    : SceneManager::Command( Type )
    , id( aId )
{
}

CommandCache* CommandCache::parse( const QJsonObject& aData )
{
    if( aData["command"].toString() != "cache" )
        return NULL;

    bool ok = true;
    int id = toInt( aData["id"], &ok );
    if( !ok || id < 1 || id > 4 ) { qDebug() << "error in cache command, invalid id: " << aData["id"].toString(); return NULL; }

    return new CommandCache( id );
}

void CommandCache::execute( SceneManager& aScene )
{
    aScene.saveCache( id );
}
