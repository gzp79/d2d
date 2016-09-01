// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include "qgraphwidget.h"
#include "inputtcp.h"
#include "scenemanager.h"
#include "mainwindow.h"

#include <QApplication>
#include <QAbstractEventDispatcher>
#include <QTime>
#include <QMainWindow>
#include <QLocale>

#include "inputnativeevent.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif// Q_OS_WIN

class Application : public QApplication {
public:
    Application( int argc, char **argv )
        : QApplication( argc, argv )
        , mServerPort(1234)
        , mTcpInput(NULL)
        , mNativeEventInput(NULL)        
    {
        setQuitOnLastWindowClosed( true );
    }

    virtual ~Application()
    {
        if( mTcpInput )
            delete mTcpInput;
        if( mNativeEventInput )
            delete mNativeEventInput;
    }

    SceneManager& getScene()
    {
        return mSceneManager;
    }

    void initInputs()
    {
        mTcpInput = new InputTCP( &mSceneManager );
        mTcpInput->startServer( mServerPort );

        mNativeEventInput = new InputNativeEvent( &mSceneManager );
        installNativeEventFilter( mNativeEventInput );
        //QAbstractEventDispatcher::instance()->installNativeEventFilter( mNativeEventInput );
    }

protected:
    SceneManager        mSceneManager;
    int                 mServerPort;
    InputTCP*           mTcpInput;
    InputNativeEvent*   mNativeEventInput;
};


int main(int argc, char **argv)
{
    QLocale::setDefault(QLocale::C);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    Application app( argc, argv);
    app.initInputs();

    MainWindow mainWindow( &app.getScene() );
    mainWindow.show();
    return app.exec();
}

