#include "mainwindow.h"
#include "serviceparams.h"
#include "servicesettingsdialog.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QTextCodec>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QSettings settings("MSLaunch.ini", QSettings::IniFormat);
    int count = settings.value("allcount").toInt();
    serviceConfigurer = new ServiceConfigurer(&settings);
    if (count > 0) {
        allServiceList = serviceConfigurer->readServiceList(&settings, "process");
        runServiceList = serviceConfigurer->readServiceList(&settings, "process_run");
    } else {
        allServiceList = serviceConfigurer->readServiceList(&settings, "all");
        runServiceList = serviceConfigurer->readServiceList(&settings, "run");
    }
    for (auto param: allServiceList) {
        ui->serviceList->addItem(param.fileName);
    }
    for (auto param: runServiceList) {
        ui->runList->addItem(param.fileName);
    }
    ui->environmentEdit->setText(settings.value("env_vars").toString());
    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tabWidget->tabBar(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(serviceTabContextMenu(const QPoint&)));
}

MainWindow::~MainWindow() {
    QSettings settings("MSLaunch.ini", QSettings::IniFormat);
    settings.clear();
    serviceConfigurer->writeServiceList(&settings, "all", allServiceList);
    serviceConfigurer->writeServiceList(&settings, "run", runServiceList);
    settings.setValue("env_vars", ui->environmentEdit->toPlainText());
    serviceConfigurer->storeServiceTypes(&settings);
    settings.sync();
    on_StopAllButton_clicked();
    delete serviceConfigurer;
    delete ui;
}

void MainWindow::on_addButton_clicked() {
    QString fname = QFileDialog::getOpenFileName(this, "add service");
    if (fname.length() > 0) {
        ServiceParams serviceParams = serviceConfigurer->createServiceParams(fname);
        allServiceList.append(serviceParams);
        ui->serviceList->addItem(serviceParams.fileName);
    }
}

void MainWindow::on_removeButton_clicked() {
    if (ui->serviceList->currentRow() >= 0) {
        allServiceList.removeAt(ui->serviceList->currentRow());
        QListWidgetItem *item = ui->serviceList->takeItem(ui->serviceList->currentRow());
        delete item;
        ui->serviceList->setCurrentRow(-1);
    }
}

void MainWindow::on_addToLaunchListButton_clicked() {
    if (ui->serviceList->currentRow() >= 0
            && !runServiceList.contains(allServiceList.at(ui->serviceList->currentRow()))) {
        runServiceList.append(allServiceList.at(ui->serviceList->currentRow()));
        ui->runList->addItem(ui->serviceList->currentItem()->text());
    }
}

void MainWindow::on_removeFromLaunchListButton_clicked() {
    if (ui->runList->currentRow() >= 0) {
        int i = ui->runList->currentRow();
        ui->runList->setCurrentRow(-1);
        runServiceList.removeAt(i);
        QListWidgetItem *item = ui->runList->takeItem(i);
        delete item;
    }
}

void MainWindow::on_launchAllButton_clicked() {
    for (int i = 0; i < runServiceList.count(); i++) {
        createServiceProcess(runServiceList.at(i));
    }
}

void MainWindow::on_StopAllButton_clicked() {
    for (int i = 0; i < processList.count(); i++) {
        ServiceForm *process = processList[i];
        process->stopProcess();
        ui->tabWidget->removeTab(1);
        delete process;
    }
    processList.clear();
}

void MainWindow::createServiceProcess(ServiceParams serviceParams) {
    bool processExists = false;
    for (ServiceForm* service: processList) {
        if (service->getServiceParams().fullName.compare(serviceParams.fullName) == 0) {
            processExists = true;
        }
    }
    if (processExists) {
        return;
    }
    serviceConfigurer->prepareLaunchParams(&serviceParams);
    ServiceForm *process = new ServiceForm(serviceParams);
    processList.append(process);
    ui->tabWidget->addTab(process, serviceParams.fileName);
    int index = ui->tabWidget->indexOf(process);
    if (index != -1) {
        ui->tabWidget->setTabIcon(index, QIcon(":/pictures/GreyButton.png"));
    }
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList envVars = ui->environmentEdit->toPlainText().split("\n");
    for (QString envVar : envVars) {
        QStringList var = envVar.split("=");
        if (env.contains(var.at(0))) {
            env.remove(var.at(0));
        }
        env.insert(var.at(0), var.at(1));
    }
    connect(process, SIGNAL(serviceLaunched(QWidget *)), this, SLOT(serviceProcessLaunched(QWidget *)));
    connect(process, SIGNAL(serviceStarted(QWidget *)), this, SLOT(serviceProcessStarted(QWidget *)));
    connect(process, SIGNAL(serviceFinished(QWidget *)), this, SLOT(serviceProcessFinished(QWidget *)));
    process->startProcess(env);
}

void MainWindow::on_serviceList_customContextMenuRequested(const QPoint &pos) {
    if (ui->serviceList->currentRow() >= 0) {
        QMenu menu;
        menu.addAction("Launch");
        if (menu.exec(ui->serviceList->mapToGlobal(pos))) {
            createServiceProcess(allServiceList.at(ui->serviceList->currentRow()));
        }
    }
}

void MainWindow::serviceProcessLaunched(QWidget *widget) {
    int index = ui->tabWidget->indexOf(widget);
    if (index != -1) {
        ui->tabWidget->setTabIcon(index, QIcon(":/pictures/YellowButton.png"));
    }
}

void MainWindow::serviceProcessStarted(QWidget *widget) {
    int index = ui->tabWidget->indexOf(widget);
    if (index != -1) {
        ui->tabWidget->setTabIcon(index, QIcon(":/pictures/GreenButton.png"));
    }
}

void MainWindow::serviceProcessFinished(QWidget *widget) {
    int index = ui->tabWidget->indexOf(widget);
    if (index != -1) {
        ui->tabWidget->setTabIcon(index, QIcon(":/pictures/RedButton.png"));
    }
}

void MainWindow::serviceTabContextMenu(const QPoint &pos) {
    int index = ui->tabWidget->tabBar()->tabAt(pos);
    if (index > 0) {
        QMenu menu;
        menu.addAction("Stop");
        if (menu.exec(ui->tabWidget->tabBar()->mapToGlobal(pos))) {
            ServiceForm *process = processList[index - 1];
            process->stopProcess();
            processList.removeAt(index - 1);
            ui->tabWidget->removeTab(index);
            delete process;
        }
    }
}

void MainWindow::on_runList_customContextMenuRequested(const QPoint &pos) {
    if (ui->runList->currentRow() >= 0) {
        QMenu menu;
        menu.addAction("Launch");
        if (menu.exec(ui->runList->mapToGlobal(pos))) {
            createServiceProcess(runServiceList.at(ui->runList->currentRow()));
        }
    }
}

void MainWindow::on_serviceList_itemDoubleClicked(QListWidgetItem *item) {
    if (item != nullptr) {
        ServiceSettingsDialog dlg(serviceConfigurer);
        dlg.setServiceParams(allServiceList.at(ui->serviceList->currentRow()));
        if (dlg.exec()) {
            allServiceList[ui->serviceList->currentRow()] = dlg.getServiceParams();
        }
    }
}

void MainWindow::on_runList_itemDoubleClicked(QListWidgetItem *item) {
    if (item != nullptr) {
        ServiceSettingsDialog dlg(serviceConfigurer);
        dlg.setServiceParams(runServiceList.at(ui->runList->currentRow()));
        if (dlg.exec()) {
            runServiceList[ui->runList->currentRow()] = dlg.getServiceParams();
        }
    }
}
