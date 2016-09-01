lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

QT += widgets
QT += network

CONFIG += console

HEADERS += \
    mainwindow.h \
    scenemanager.h \
    inputtcp.h \
    inputnativeevent.h \
    \
    cmdctrl.h \
    cmdpoint.h \
    cmdline.h \
    cmdrect.h \
    cmdtext.h \
    cmdpoly.h \
    cmdpolyline.h \
    \
    qgraphicspointitem.h \
    qgraphicspointtextitem.h \
    qgraphwidget.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    scenemanager.cpp \
    inputtcp.cpp \
    inputnativeevent.cpp \
    \
    cmdctrl.cpp \
    cmdpoint.cpp \
    cmdline.cpp \
    cmdrect.cpp \
    cmdtext.cpp \
    cmdpoly.cpp \
    cmdpolyline.cpp \
    \
    qgraphicspointitem.cpp \
    qgraphicspointtextitem.cpp \
    qgraphwidget.cpp


FORMS += \
    mainwindow.ui

