// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef INPUTTCP_H
#define INPUTTCP_H

#include <QTcpServer>
#include <QThreadPool>
#include <QDebug>

class SceneManager;

class InputTCP : public QTcpServer
{
    Q_OBJECT
public:
    explicit InputTCP( SceneManager* aSceneManager, QObject* aParent = 0 );
    void startServer( int aPort );

protected:
    void incomingConnection( qintptr handle );

signals:

public slots:

private:
    class Runnable;

    SceneManager*   mSceneManager;
    QThreadPool*    mPool;
};

#endif // INPUTTCP_H
