#include "serviceform.h"
#include "ui_serviceform.h"

#include <QTextCodec>
#include <QTimer>
#include <QScrollBar>

ServiceForm::ServiceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServiceForm) {
    ui->setupUi(this);
    init();
}

ServiceForm::ServiceForm(ServiceParams serviceParams, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServiceForm) {
    ui->setupUi(this);
    init();
    setServiceLaunchParams(serviceParams);
}

ServiceForm::~ServiceForm() {
    delete ui;
    process->deleteLater();
    delayTimer->deleteLater();
    highlighter->deleteLater();
}

void ServiceForm::startProcess(QProcessEnvironment env) {
    process->setProcessEnvironment(env);
    process->setWorkingDirectory(serviceLaunchParams.filePath);
    if (serviceLaunchParams.launchDelay == 0) {
        process->start(command, serviceLaunchParams.mainLaunchParams);
    } else  {
        delayTimer->start(serviceLaunchParams.launchDelay * 1000);
    }
}

void ServiceForm::stopProcess() {
    process->terminate();
    process->kill();
    process->close();
}

ServiceParams ServiceForm::getServiceLaunchParams() const {
    return serviceLaunchParams;
}

void ServiceForm::setServiceLaunchParams(const ServiceParams &value) {
    serviceLaunchParams = value;
    if (serviceLaunchParams.launchParams != nullptr && !serviceLaunchParams.launchParams.isEmpty()) {
        serviceLaunchParams.mainLaunchParams.removeLast();
        serviceLaunchParams.mainLaunchParams.append(serviceLaunchParams.launchParams.split("\n"));
        serviceLaunchParams.mainLaunchParams.append(serviceLaunchParams.fileName);
    }
}

void ServiceForm::readConsoleReady() {
    QString newLine = mainTextCodec->toUnicode(process->readAllStandardOutput());
    QString text = ui->logOutput->toPlainText() + newLine;
    ui->logOutput->setText(text);
    ui->logOutput->verticalScrollBar()->setValue(ui->logOutput->verticalScrollBar()->maximum());
    if (!started && ui->logOutput->toPlainText().contains(startedInRegEx)) {
        started = true;
        emit serviceStarted(this);
    }
}

void ServiceForm::finishedConsole(int code, QProcess::ExitStatus status) {
    QString text = ui->logOutput->toPlainText();
    ui->logOutput->setText(text + QString("\nProcess finished with code: %1, and status: %2").arg(code).arg(status));
    emit serviceFinished(this);
}

void ServiceForm::timedEvent() {
    delayTimer->stop();
    process->start(command, serviceLaunchParams.mainLaunchParams);
}

void ServiceForm::on_lineWrapCheck_clicked(bool checked) {
    ui->logOutput->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void ServiceForm::init() {
    delayTimer = new QTimer(this);
    highlighter = new JsonHighlighter(ui->logOutput->document());
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readConsoleReady()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedConsole(int, QProcess::ExitStatus)));
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(timedEvent()));
}
