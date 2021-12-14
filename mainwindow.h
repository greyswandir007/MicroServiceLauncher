#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serviceconfigurer.h"
#include "serviceform.h"
#include "serviceparams.h"

#include <QListWidget>
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
    void on_serviceList_customContextMenuRequested(const QPoint &pos);
    void serviceProcessLaunched(QWidget *widget);
    void serviceProcessStarted(QWidget *widget);
    void serviceProcessFinished(QWidget *widget);
    void serviceTabContextMenu(const QPoint& pos);
    void on_runList_customContextMenuRequested(const QPoint &pos);
    void on_serviceList_itemDoubleClicked(QListWidgetItem *item);
    void on_runList_itemDoubleClicked(QListWidgetItem *item);

private:
    void createServiceProcess(ServiceParams serviceParams);

    Ui::MainWindow *ui;
    QStringList allProcess;
    QStringList runProcess;
    QList<ServiceForm *> processList;
    QList<ServiceParams> allServiceList;
    QList<ServiceParams> runServiceList;
    bool selectionChanged;
    ServiceConfigurer *serviceConfigurer;
};

#endif // MAINWINDOW_H
