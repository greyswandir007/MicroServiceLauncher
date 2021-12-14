#ifndef SERVCICETYPE_H
#define SERVCICETYPE_H
#include <QString>
#include <QStringList>

class ServiceType {
public:
    bool hasLauncher;
    QString launchCommand;
    QString launchParams;
    QStringList extensions;
    QString startedRegex;

    ServiceType(bool hasLauncher, QString launchCommand, QString launchParams, QStringList extensions,
                QString startedRegex) {
        this->hasLauncher = hasLauncher;
        this->launchCommand = launchCommand;
        this->launchParams = launchParams;
        this->extensions = extensions;
        this->startedRegex = startedRegex;
    }
    ServiceType() {
        hasLauncher = false;
    }
};

#endif // SERVCICETYPE_H
