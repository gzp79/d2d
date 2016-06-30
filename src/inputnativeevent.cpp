// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#include <QtGlobal>

#include "inputnativeevent.h"
#include "scenemanager.h"

#include <QString>
#include <QDebug>
#include <QTextStream>

#include "cmdctrl.h"
#include "cmdline.h"
#include "cmdrect.h"
#include "cmdtext.h"
#include "cmdpoint.h"

InputNativeEvent::InputNativeEvent( SceneManager* aSceneManager )
    : QAbstractNativeEventFilter()
    , mSceneManager( aSceneManager )
{
}

bool InputNativeEvent::nativeEventFilter( const QByteArray& aEventType, void* aMessage, long* aResult )
{
#ifdef Q_OS_WIN
    Q_UNUSED( aEventType );
    if( winEventFilter( (MSG*)aMessage ) )
    {
        *aResult = 1;
        return true;
    }
#elif defined(Q_OS_LINUX)
    Q_UNUSED(aResult);
    Q_UNUSED(aEventType);
    Q_UNUSED(aMessage);
#endif // Q_OS_*

    return false;
}

#ifdef Q_OS_WIN
bool InputNativeEvent::winEventFilter( const MSG* aMsg )
{
    if( aMsg->message != WM_COPYDATA )
    {
        return false;
    }

    COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)aMsg->lParam;
    switch( copyData->dwData )
    {
        case 0: // Rect passed
        {
            struct PassedRect
            {
                float l;
                float t;
                float r;
                float b;
                DWORD color;
            };
            PassedRect* rect = (PassedRect*)copyData->lpData;
            float l = qMin( rect->l, rect->r );
            float r = qMax( rect->l, rect->r );
            float t = qMax( rect->t, rect->b );
            float b = qMin( rect->t, rect->b );
            mSceneManager->addCommand( new CommandRect("",l,t,r,b,rect->color) );
        } break;

        case 1: // Text passed
        {
            struct PassedText
            {
                float x;
                float y;
                char  text[ 64 ];
            };
            PassedText* text = (PassedText*)copyData->lpData;
            mSceneManager->addCommand( new CommandText("",text->x, text->y, QString(text->text), 0xffffffff) );
        } break;

        case 2: // Clear
        {
            mSceneManager->addCommand( new CommandClear() );
        } break;

        case 4: //Line passed
        {
            struct PassedLine
            {
                float l;
                float t;
                float r;
                float b;
                DWORD color;
            };
            PassedLine* line = (PassedLine*)copyData->lpData;
            mSceneManager->addCommand( new CommandLine( "", line->l,line->t, line->r,line->b, line->color ) );
        } break;

        case 5: // Text with color passed
        {
            struct PassedColorText
            {
                float x;
                float y;
                DWORD color;
                char  text[ 64 ];
            };
            PassedColorText* text = (PassedColorText*)copyData->lpData;

            mSceneManager->addCommand( new CommandText("", text->x, text->y, QString(text->text), text->color) );
        } break;

        case 6:
        {
            struct PassedPoint
            {
                float x;
                float y;
                DWORD color;
            };
            PassedPoint* pnt = (PassedPoint*)copyData->lpData;
            mSceneManager->addCommand( new CommandPoint( "", pnt->x, pnt->y, pnt->color ) );
        } break;

        case 7:
        {
            struct PassedCache
            {
                int id;
            };
            PassedCache* cache = (PassedCache*)copyData->lpData;
            mSceneManager->addCommand( new CommandCache( cache->id ) );
        } break;

        case 8:
        {
            const char* str = (const char*)copyData->lpData;
            mSceneManager->addCommand( QByteArray( str ) );
        } break;

    }

    return true;
}
#endif //Q_OS_WIN
