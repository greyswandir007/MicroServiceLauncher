#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serviceform.h"
#include "serviceparams.h"

#include <QMainWindow>
#include <QProcess>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_addToLaunchListButton_clicked();
    void on_removeFromLaunchListButton_clicked();
    void on_launchAllButton_clicked();
    void on_StopAllButton_clicked();
    void on_runList_itemSelectionChanged();
    void on_commandLineEdit_textChanged();
    void on_commandLineEdit_cursorPositionChanged();
    void on_serviceList_customContextMenuRequested(const QPoint &pos);
    void on_launchDelay_valueChanged(int value);
    void serviceProcessStarted(QWidget *widget);
    void serviceProcessFinished(QWidget *widget);
    void serviceTabContextMenu(const QPoint& pos);

    void on_runList_customContextMenuRequested(const QPoint &pos);

private:
    ServiceParams readServiceParams(QString baseName, QSettings *settings);
    ServiceParams createServiceParams(QString filename);
    void writeServiceParams(QString baseName, QSettings *settings, ServiceParams serviceParams);
    void createServiceProcess(ServiceParams serviceParams);

    Ui::MainWindow *ui;
    QStringList allProcess;
    QStringList runProcess;
    QList<ServiceForm *> processList;
    QList<ServiceParams> allServiceList;
    QList<ServiceParams> runServiceList;
    bool selectionChanged;
};

#endif // MAINWINDOW_H
