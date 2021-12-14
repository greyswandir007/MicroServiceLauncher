#ifndef SERVICEFORM_H
#define SERVICEFORM_H

#include "jsonhighlighter.h"
#include "serviceparams.h"

#include <QProcess>
#include <QTextCodec>
#include <QWidget>

namespace Ui {
class ServiceForm;
}

class ServiceForm : public QWidget {
    Q_OBJECT

public:
    explicit ServiceForm(QWidget *parent = nullptr);
    explicit ServiceForm(ServiceParams serviceParams, QWidget *parent = nullptr);
    virtual ~ServiceForm();

    void startProcess(QProcessEnvironment env);
    void stopProcess();

    ServiceParams getServiceLaunchParams() const;
    void setServiceLaunchParams(const ServiceParams &value);

signals:
    void serviceFinished(QWidget *);
    void serviceStarted(QWidget *);

private slots:
    void readConsoleReady();
    void finishedConsole(int code, QProcess::ExitStatus status);
    void timedEvent();
    void on_lineWrapCheck_clicked(bool checked);

private:
    void init();

    Ui::ServiceForm *ui;

    QProcess *process = new QProcess();
    QTextCodec *mainTextCodec = QTextCodec::codecForName("UTF8");
    ServiceParams serviceLaunchParams;
    QString command = "java";
    QTimer *delayTimer = nullptr;
    bool started = false;

    QRegularExpression startedInRegEx = QRegularExpression("Started [a-zA-Z0-9]* in [0-9]*[.][0-9]* [a-z]* [(]JVM running for");

    JsonHighlighter *highlighter = nullptr;
};

#endif // SERVICEFORM_H
