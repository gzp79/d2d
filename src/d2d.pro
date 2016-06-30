lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

QT += widgets
QT += network

CONFIG += console

HEADERS += \
    graphwidget.h \
    mainwindow.h \
    inputtcp.h \
    scenemanager.h \
    cmdline.h \
    cmdrect.h \
    cmdtext.h \
    inputnativeevent.h \
    cmdpoint.h \
    qgraphicspointtext.h \
    cmdctrl.h \
    cmdpoly.h

SOURCES += \
    main.cpp \
    graphwidget.cpp \
    mainwindow.cpp \
    inputtcp.cpp \
    scenemanager.cpp \
    cmdline.cpp \
    cmdrect.cpp \
    cmdtext.cpp \
    inputnativeevent.cpp \
    cmdpoint.cpp \
    qgraphicspointtext.cpp \
    cmdctrl.cpp \
    cmdpoly.cpp

FORMS += \
    mainwindow.ui

