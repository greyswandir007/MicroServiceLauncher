#ifndef SERVICEPARAMS_H
#define SERVICEPARAMS_H

#include <QString>
#include <QStringList>

class ServiceParams {
public:
    QString fullName;
    QString fileName;
    QString filePath;
    QString launchParams;
    int launchDelay = 0;

    QStringList mainLaunchParams;

    bool operator==(const ServiceParams params) {
        return params.fullName == fullName;
    }
};

#endif // SERVICEPARAMS_H
