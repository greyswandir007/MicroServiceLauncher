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

    ServiceParams getServiceParams() const;
    void setServiceParams(const ServiceParams &value);

signals:
    void serviceFinished(QWidget *);
    void serviceStarted(QWidget *);
    void serviceLaunched(QWidget *);

private slots:
    void readConsoleReady();
    void readErrorConsoleReady();
    void serviceStart();
    void finishedConsole(int code, QProcess::ExitStatus status);
    void timedEvent();
    void on_lineWrapCheck_clicked(bool checked);

private:
    void init();

    Ui::ServiceForm *ui;

    QProcess *process = new QProcess();
    QTextCodec *mainTextCodec = QTextCodec::codecForName("UTF8");
    ServiceParams serviceParams;
    QTimer *delayTimer = nullptr;
    bool started = false;

    QRegularExpression startedRegEx;

    JsonHighlighter *highlighter = nullptr;
};

#endif // SERVICEFORM_H
