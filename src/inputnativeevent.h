// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef INPUTWMCOPYDATA_H
#define INPUTWMCOPYDATA_H

#include <QAbstractNativeEventFilter>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif //Q_OS_WIN

class SceneManager;

class InputNativeEvent : public QAbstractNativeEventFilter
{
public:
    InputNativeEvent( SceneManager* aSceneManager );
    virtual bool nativeEventFilter( const QByteArray &aEventType, void* aMessage, long* aResult ) Q_DECL_OVERRIDE;

private:
    SceneManager*   mSceneManager;

#ifdef Q_OS_WIN
    bool winEventFilter( const MSG* aMsg );
#endif //Q_OS_WIN
};


#endif // INPUTWMCOPYDATA_H

