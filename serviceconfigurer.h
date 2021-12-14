#ifndef SERVICECONFIGURER_H
#define SERVICECONFIGURER_H

#include "servicetype.h"

#include <QMap>


class QSettings;
class QString;
class ServiceParams;

class ServiceConfigurer {
public:
    ServiceConfigurer();
    ServiceConfigurer(QSettings *settings);

    ServiceParams createServiceParams(QString filename);
    ServiceParams readServiceParams(QSettings *settings);
    ServiceParams readOldServiceParams(QSettings *settings, QString baseName);
    void writeServiceParams(QSettings *settings, ServiceParams serviceParams);
    void prepareLaunchParams(ServiceParams *serviceParams);
    QString getLaunchCommand(ServiceParams serviceParams);
    QMap<QString, ServiceType> getServiceTypes();
    void storeServiceTypes(QSettings *settings);

    QList<ServiceParams> readServiceList(QSettings *settings, QString listName);
    void writeServiceList(QSettings *settings, QString listName, QList<ServiceParams> serviceList);

private:
    void checkServiceType(ServiceParams *serviceParams);
    void loadServiceTypes(QSettings *settings);

    QMap<QString, ServiceType> serviceTypes;
};

#endif // SERVICECONFIGURER_H
