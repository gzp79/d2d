// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "inputtcp.h"
#include "scenemanager.h"
#include <QTcpSocket>

class InputTCP::Runnable : public QRunnable
{
public:
    Runnable( InputTCP& aServer, qintptr aSocketDescriptor )
        : mServer( aServer )
        , mSocketDescriptor( aSocketDescriptor )
    {
    }

protected:
    void run()
    {
        if( !mSocketDescriptor )
            return;

        QTcpSocket socket;
        socket.setSocketDescriptor( mSocketDescriptor );
        qDebug() << "socket pool started";

        int open = 0;   // number of open brackets
        int inStr = false;
        QByteArray data;
        //while( socket.state() == QAbstractSocket::ConnectedState )
        while( socket.waitForReadyRead(-1) )
        {
            char c;
            while( socket.getChar( &c ) )
            {
                if( open <= 0 && c != '{' )
                {
                    qDebug() << "dr:" << c;
                    continue;
                }

                data.push_back( c );
                if( c == '"' )
                {
                    // todo: support for escaped " (\")
                    inStr = !inStr;
                    continue;
                }

                if( inStr )
                {
                    //nop
                }
                else if( c == '{' )
                {
                    ++open;
                }
                else if( c == '}' )
                {
                    --open;
                    if( open < 0 )
                    {
                        qDebug() << "error: " << data;
                        data.clear();
                    }
                    else if( open == 0 )
                    {
                        mServer.mSceneManager->addCommand( data );
                        data.clear();
                    }
                }
            }
        }

        socket.close();
        qDebug() << "socket pool stopped";
    }

public:
    InputTCP& mServer;
    qintptr mSocketDescriptor;

};

InputTCP::InputTCP( SceneManager* aSceneManager, QObject* aParent )
    : QTcpServer( aParent )
    , mSceneManager( aSceneManager )
{
    mPool = new QThreadPool(this);
    mPool->setMaxThreadCount(5);
}

void InputTCP::startServer( int aPort )
{
    if( listen( QHostAddress::Any, aPort ) )
    {
        qDebug() << "Server started";
    }
    else
    {
        qDebug() << "Server did not start!";
    }
}

void InputTCP::incomingConnection( qintptr aHandle )
{
    // 1. QTcpServer gets a new connection request from a client.
    // 2. It makes a task (runnable) here.
    // 3. Then, the server grabs one of the threads.
    // 4. The server throws the runnable to the thread.

    // Note: Rannable is a task not a thread
    Runnable *task = new Runnable( *this, aHandle );
    task->setAutoDelete(true);
    mPool->start(task);    
}
