// Copyright (c) 2005-2016 Gzp
// Distributed under MIT License.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scenemanager.h"

class SceneManager;
class QTreeWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SceneManager* aSceneManager, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnTest_clicked();

    void on_btnSaveCache1_clicked();
    void on_btnSaveCache2_clicked();
    void on_btnSaveCache3_clicked();
    void on_btnSaveCache4_clicked();
    void on_btnLoadCache1_clicked();
    void on_btnLoadCache2_clicked();
    void on_btnLoadCache3_clicked();
    void on_btnLoadCache4_clicked();

    void on_btnZoomIn_clicked();
    void on_btnZoomOut_clicked();
    void on_btnZoomFit_clicked();
    void on_btnClear_clicked();
    void on_btnOpen_clicked();

    void on_cbxBackground_currentIndexChanged(int index);
    void handleBackgroundChange(int);
    void on_cbxSceneArea_currentIndexChanged(int index);
    void handleSceneAreaChange(int);
    void on_tblLayers_itemChanged(QTreeWidgetItem* item, int column);
    void handleLayersChanged();
    void handleLayersVisibilityChanged(LayerInfo);

    void on_btnScreenCapture_clicked();

private:
    enum {
        DataLayerKey    = Qt::UserRole + 1,
        DataLayerCategoryKey,
    };

    Ui::MainWindow* ui;
    SceneManager*   mSceneManager;
    bool            mInHandleLayersVisibilityChanged;
    QString         mLastSelectedFile;
    QString         mLastSelectedScreenFile;
};

#endif // MAINWINDOW_H
