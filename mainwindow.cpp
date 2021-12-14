#include "mainwindow.h"
#include "serviceparams.h"
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
    for(int i = 0; i < count; i++) {
        ServiceParams serviceParams = readServiceParams(QString("process_%1").arg(i), &settings);
        allServiceList.append(serviceParams);
        ui->serviceList->addItem(serviceParams.fileName);
    }
    count = settings.value("runcount").toInt();
    for(int i = 0; i < count; i++) {
        ServiceParams serviceParams = readServiceParams(QString("process_run_%1").arg(i), &settings);
        runServiceList.append(serviceParams);
        ui->runList->addItem(serviceParams.fileName);
    }
    ui->environmentEdit->setText(settings.value("env_vars").toString());
    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tabWidget->tabBar(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(serviceTabContextMenu(const QPoint&)));
}

MainWindow::~MainWindow() {
    QSettings settings("MSLaunch.ini", QSettings::IniFormat);
    settings.setValue("allcount", ui->serviceList->count());
    settings.setValue("runcount", ui->runList->count());
    for(int i = 0; i < ui->serviceList->count(); i++) {
        writeServiceParams(QString("process_%1").arg(i), &settings, allServiceList.at(i));
    }
    for(int i = 0; i < ui->runList->count(); i++) {
        writeServiceParams(QString("process_run_%1").arg(i), &settings, runServiceList.at(i));
    }
    settings.setValue("env_vars", ui->environmentEdit->toPlainText());
    settings.sync();
    on_StopAllButton_clicked();
    delete ui;
}

void MainWindow::on_addButton_clicked() {
    QString fname = QFileDialog::getOpenFileName(this, "add service");
    if (fname.length() > 0) {
        ServiceParams serviceParams = createServiceParams(fname);
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

void MainWindow::on_runList_itemSelectionChanged() {
    selectionChanged = true;
    if (ui->runList->currentRow() >= 0) {
        ui->commandLineEdit->setText(runServiceList.at(ui->runList->currentRow()).launchParams);
        ui->launchDelay->setValue(runServiceList.at(ui->runList->currentRow()).launchDelay);
    } else {
        qDebug() << "selection changed to -1";
        ui->commandLineEdit->clear();
        ui->launchDelay->setValue(0);
    }
}

ServiceParams MainWindow::readServiceParams(QString baseName, QSettings *settings) {
    ServiceParams serviceParams;
    bool delayConverted;
    QString process = settings->value(baseName).toString();
    QString shortName = settings->value(baseName + "_short").toString();
    QString processPath = settings->value(baseName + "_path").toString();
    QString launchParams = settings->value(baseName + "_command").toString();
    int launchDelay = settings->value(baseName + "_delay").toInt(&delayConverted);
    serviceParams.fullName = process;
    if (shortName != nullptr && !shortName.isEmpty()) {
        serviceParams.fileName = shortName;
    } else {
        serviceParams.fileName = process.section("/", -1, -1);
    }
    if (processPath != nullptr && !processPath.isEmpty()) {
        serviceParams.filePath = processPath;
    } else {
        serviceParams.filePath = process.left(process.length() - serviceParams.fileName.length());
    }
    if (launchParams != nullptr && !launchParams.isEmpty()) {
        serviceParams.launchParams = launchParams;
    }
    if (delayConverted) {
        serviceParams.launchDelay = launchDelay;
    }
    serviceParams.mainLaunchParams.append("-jar");
    serviceParams.mainLaunchParams.append(serviceParams.fileName);
    return serviceParams;
}

ServiceParams MainWindow::createServiceParams(QString filename) {
    ServiceParams serviceParams;
    serviceParams.fullName = filename;
    serviceParams.fileName = filename.section("/", -1, -1);
    serviceParams.filePath = filename.left(filename.length() - serviceParams.fileName.length());
    serviceParams.mainLaunchParams.append("-jar");
    serviceParams.mainLaunchParams.append(serviceParams.fileName);
    return serviceParams;
}

void MainWindow::writeServiceParams(QString baseName, QSettings *settings, ServiceParams serviceParams) {
     settings->setValue(baseName, serviceParams.fullName);
     settings->setValue(baseName + "_short", serviceParams.fileName);
     settings->setValue(baseName + "_path", serviceParams.filePath);
     settings->setValue(baseName + "_command", serviceParams.launchParams);
     settings->setValue(baseName + "_delay", serviceParams.launchDelay);
}

void MainWindow::createServiceProcess(ServiceParams serviceParams) {
    bool processExists = false;
    for (ServiceForm* service: processList) {
        if (service->getServiceLaunchParams().fullName.compare(serviceParams.fullName) == 0) {
            processExists = true;
        }
    }
    if (processExists) {
        return;
    }
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
    connect(process, SIGNAL(serviceStarted(QWidget *)), this, SLOT(serviceProcessStarted(QWidget *)));
    connect(process, SIGNAL(serviceFinished(QWidget *)), this, SLOT(serviceProcessFinished(QWidget *)));
    process->startProcess(env);
}

void MainWindow::on_commandLineEdit_textChanged() {
    if (ui->runList->currentRow() >= 0 && !selectionChanged) {
        runServiceList[ui->runList->currentRow()].launchParams = ui->commandLineEdit->toPlainText();
    }
}

void MainWindow::on_commandLineEdit_cursorPositionChanged() {
    if (ui->runList->currentRow() >= 0 && !selectionChanged) {
        runServiceList[ui->runList->currentRow()].launchParams = ui->commandLineEdit->toPlainText();
    }
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

void MainWindow::on_launchDelay_valueChanged(int value) {
    if (ui->runList->currentRow() >= 0 && !selectionChanged) {
        runServiceList[ui->runList->currentRow()].launchDelay = value;
    }
    selectionChanged = false;
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
