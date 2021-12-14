#include "serviceform.h"
#include "ui_serviceform.h"

#include <QTextCodec>
#include <QTimer>
#include <QScrollBar>
#include <QDebug>

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
    setServiceParams(serviceParams);
}

ServiceForm::~ServiceForm() {
    delete ui;
    process->deleteLater();
    delayTimer->deleteLater();
    highlighter->deleteLater();
}

void ServiceForm::startProcess(QProcessEnvironment env) {
    process->setProcessEnvironment(env);
    process->setWorkingDirectory(serviceParams.filePath);
    if (serviceParams.launchDelay == 0) {
        process->start(serviceParams.launchCommand, serviceParams.mainLaunchParams);
    } else  {
        delayTimer->start(serviceParams.launchDelay * 1000);
    }
}

void ServiceForm::stopProcess() {
    process->terminate();
    process->kill();
    process->close();
}

ServiceParams ServiceForm::getServiceParams() const {
    return serviceParams;
}

void ServiceForm::setServiceParams(const ServiceParams &value) {
    serviceParams = value;
    if (!serviceParams.startedRegex.isEmpty()) {
        startedRegEx = QRegularExpression(serviceParams.startedRegex);
    }
}

void ServiceForm::readConsoleReady() {
    QString newLine = mainTextCodec->toUnicode(process->readAllStandardOutput());
    QString text = ui->logOutput->toPlainText() + newLine;
    ui->logOutput->setText(text);
    ui->logOutput->verticalScrollBar()->setValue(ui->logOutput->verticalScrollBar()->maximum());
    if (!started && !serviceParams.startedRegex.isEmpty() && ui->logOutput->toPlainText().contains(startedRegEx)) {
        started = true;
        emit serviceStarted(this);
    }
}

void ServiceForm::readErrorConsoleReady() {
    QString newLine = mainTextCodec->toUnicode(process->readAllStandardError());
    QString text = ui->logOutput->toPlainText() + newLine;
    ui->logOutput->setText(text);
    ui->logOutput->verticalScrollBar()->setValue(ui->logOutput->verticalScrollBar()->maximum());
}

void ServiceForm::serviceStart() {
    emit serviceLaunched(this);
}

void ServiceForm::finishedConsole(int code, QProcess::ExitStatus status) {
    QString text = ui->logOutput->toPlainText();
    ui->logOutput->setText(text + QString("\nProcess finished with code: %1, and status: %2").arg(code).arg(status));
    emit serviceFinished(this);
}

void ServiceForm::timedEvent() {
    delayTimer->stop();
    process->start(serviceParams.launchCommand, serviceParams.mainLaunchParams);
}

void ServiceForm::on_lineWrapCheck_clicked(bool checked) {
    ui->logOutput->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void ServiceForm::init() {
    delayTimer = new QTimer(this);
    highlighter = new JsonHighlighter(ui->logOutput->document());

    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readConsoleReady()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readErrorConsoleReady()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedConsole(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(started()), this, SLOT(serviceStart()));
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(timedEvent()));
}
