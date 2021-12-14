#include "serviceconfigurer.h"
#include <QString>
#include <QSettings>
#include "serviceparams.h"
#include "servicetypes.h"

ServiceConfigurer::ServiceConfigurer() {
    serviceTypes = defaultServiceTypes;
}

ServiceConfigurer::ServiceConfigurer(QSettings *settings) {
    loadServiceTypes(settings);
    if (serviceTypes.isEmpty()) {
        serviceTypes = defaultServiceTypes;
    }
}

ServiceParams ServiceConfigurer::createServiceParams(QString filename) {
    ServiceParams serviceParams;
    serviceParams.fullName = filename;
    serviceParams.fileName = filename.section("/", -1, -1);
    serviceParams.filePath = filename.left(filename.length() - serviceParams.fileName.length());
    checkServiceType(&serviceParams);
    return serviceParams;
}

ServiceParams ServiceConfigurer::readServiceParams(QSettings *settings) {
    ServiceParams serviceParams;
    bool delayConverted;
    QString fullname = settings->value("fullname").toString();
    QString filename = settings->value("filename").toString();
    QString processPath = settings->value("filepath").toString();
    QString type = settings->value("type").toString();
    QString launchParams = settings->value("launcherParams").toString();
    QString launchCommand = settings->value("launchCommand").toString();
    QString params = settings->value("serviceParams").toString();
    QString startedRegex = settings->value("startedRegex").toString();
    int launchDelay = settings->value("launchDelay").toInt(&delayConverted);
    serviceParams.fullName = fullname;
    if (filename != nullptr && !filename.isEmpty()) {
        serviceParams.fileName = filename;
    } else {
        serviceParams.fileName = fullname.section("/", -1, -1);
    }
    if (processPath != nullptr && !processPath.isEmpty()) {
        serviceParams.filePath = processPath;
    } else {
        serviceParams.filePath = fullname.left(fullname.length() - serviceParams.fileName.length());
    }
    if (launchParams != nullptr && !launchParams.isEmpty()) {
        serviceParams.launchParams = launchParams;
    }
    if (type != nullptr && !type.isEmpty()) {
        serviceParams.type = type;
    }
    if (launchCommand != nullptr && !launchCommand.isEmpty()) {
        serviceParams.launchCommand = launchCommand;
    }
    if (params != nullptr && !params.isEmpty()) {
        serviceParams.serviceParams = params;
    }
    if (startedRegex != nullptr && !startedRegex.isEmpty()) {
        serviceParams.startedRegex = startedRegex;
    }
    if (delayConverted) {
        serviceParams.launchDelay = launchDelay;
    }
    checkServiceType(&serviceParams);
    return serviceParams;
}

ServiceParams ServiceConfigurer::readOldServiceParams(QSettings *settings, QString baseName) {
    ServiceParams serviceParams;
    bool delayConverted;
    QString process = settings->value(baseName).toString();
    QString shortName = settings->value(baseName + "_short").toString();
    QString processPath = settings->value(baseName + "_path").toString();
    QString command = settings->value(baseName + "_command").toString();
    QString type = settings->value(baseName + "_type").toString();
    QString launchParams = settings->value(baseName + "_launchParams").toString();
    QString launchCommand = settings->value(baseName + "_launchCommand").toString();
    QString params = settings->value(baseName + "_serviceParams").toString();
    QString startedRegex = settings->value(baseName + "_startedRegex").toString();
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
    if (command != nullptr && !command.isEmpty()) {
        serviceParams.launchParams = command;
    }
    if (launchParams != nullptr && !launchParams.isEmpty()) {
        serviceParams.launchParams = launchParams;
    }
    if (type != nullptr && !type.isEmpty()) {
        serviceParams.type = type;
    }
    if (launchCommand != nullptr && !launchCommand.isEmpty()) {
        serviceParams.launchCommand = launchCommand;
    }
    if (params != nullptr && !params.isEmpty()) {
        serviceParams.serviceParams = params;
    }
    if (startedRegex != nullptr && !startedRegex.isEmpty()) {
        serviceParams.startedRegex = startedRegex;
    }
    if (delayConverted) {
        serviceParams.launchDelay = launchDelay;
    }
    checkServiceType(&serviceParams);
    return serviceParams;
}

void ServiceConfigurer::writeServiceParams(QSettings *settings, ServiceParams serviceParams) {
     settings->setValue("fullname", serviceParams.fullName);
     settings->setValue("filename", serviceParams.fileName);
     settings->setValue("filepath", serviceParams.filePath);
     settings->setValue("launcherParams", serviceParams.launchParams);
     settings->setValue("launchCommand", serviceParams.launchCommand);
     settings->setValue("serviceParams", serviceParams.serviceParams);
     settings->setValue("type", serviceParams.type);
     settings->setValue("launchDelay", serviceParams.launchDelay);
     settings->setValue("startedRegex", serviceParams.startedRegex);
}

void ServiceConfigurer::prepareLaunchParams(ServiceParams *serviceParams) {
    ServiceType type = serviceTypes.value(serviceParams->type);
    serviceParams->mainLaunchParams.clear();
    if (type.hasLauncher) {
        if (type.launchParams != nullptr && !type.launchParams.isEmpty()) {
            serviceParams->mainLaunchParams.append(type.launchParams.split("\n"));
        }
        if (serviceParams->launchParams != nullptr && !serviceParams->launchParams.isEmpty()) {
            serviceParams->mainLaunchParams.append(serviceParams->launchParams.split("\n"));
        }
        serviceParams->mainLaunchParams.append(serviceParams->fileName);
    }
    if (serviceParams->serviceParams != nullptr && !serviceParams->serviceParams.isEmpty()) {
        serviceParams->mainLaunchParams.append(serviceParams->serviceParams.split("\n"));
    }
}

QString ServiceConfigurer::getLaunchCommand(ServiceParams serviceParams) {
    QString result;
    prepareLaunchParams(&serviceParams);
    result = serviceParams.filePath + serviceParams.launchCommand;
    for (QString param: serviceParams.mainLaunchParams) {
        result += " " + (param.contains(" ") ? "\"" + param + "\"": param);
    }
    return result;
}

QMap<QString, ServiceType> ServiceConfigurer::getServiceTypes() {
    return serviceTypes;
}

void ServiceConfigurer::storeServiceTypes(QSettings *settings) {
    settings->beginGroup("Types");
    int i = 0;
    settings->beginWriteArray("type");
    for (auto type: serviceTypes.keys()) {
        settings->setArrayIndex(i);
        settings->setValue("name", type);
        settings->setValue("hasLauncher", serviceTypes[type].hasLauncher);
        settings->setValue("launchCommand", serviceTypes[type].launchCommand);
        settings->setValue("launchParams", serviceTypes[type].launchParams);
        settings->setValue("startedRegex", serviceTypes[type].startedRegex);
        settings->setValue("extensionCount", serviceTypes[type].extensions.size());
        settings->beginWriteArray("extensions");
        QStringList extensions;
        for (int j = 0; j < serviceTypes[type].extensions.size(); j++) {
            settings->setArrayIndex(j);
            settings->setValue("item", serviceTypes[type].extensions.at(j));
        }
        settings->endArray();
        i++;
    }
    settings->endArray();
    settings->endGroup();
}

QList<ServiceParams> ServiceConfigurer::readServiceList(QSettings *settings, QString listName) {
    QList<ServiceParams> serviceList;
    int count = settings->value("allcount").toInt();
    if (listName.compare("process_run") == 0) {
        count = settings->value("runcount").toInt();
    }
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            serviceList.append(readOldServiceParams(settings, listName + QString("_%1").arg(i)));
        }
    } else {
        settings->beginGroup("Process");
        count = settings->beginReadArray(listName);
        for (int i = 0; i < count; i++) {
            settings->setArrayIndex(i);
            serviceList.append(readServiceParams(settings));
        }
        settings->endArray();
        settings->endGroup();
    }
    return serviceList;
}

void ServiceConfigurer::writeServiceList(QSettings *settings, QString listName, QList<ServiceParams> serviceList) {
    settings->beginGroup("Process");
    settings->beginWriteArray(listName);
    for (int i = 0; i < serviceList.count(); i++) {
        settings->setArrayIndex(i);
        writeServiceParams(settings, serviceList.at(i));
    }
    settings->endArray();
    settings->endGroup();
}

void ServiceConfigurer::checkServiceType(ServiceParams *serviceParams) {
    if (serviceParams->type == nullptr || serviceParams->type.isEmpty()) {
        bool found = false;
        for (auto type: serviceTypes.keys()) {
            for (auto ext: serviceTypes[type].extensions) {
                if (serviceParams->fileName.endsWith(ext)) {
                    serviceParams->type = type;
                    serviceParams->startedRegex = serviceTypes[type].startedRegex;
                    if (serviceParams->launchCommand == nullptr || serviceParams->launchCommand.isEmpty()) {
                        if (serviceTypes[type].hasLauncher) {
                            serviceParams->launchCommand = serviceTypes[type].launchCommand;
                        } else {
                            serviceParams->launchCommand = serviceParams->fileName;
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    } else if (serviceParams->launchCommand == nullptr || serviceParams->launchCommand.isEmpty()) {
        if (serviceTypes[serviceParams->type].hasLauncher) {
            serviceParams->launchCommand = serviceTypes[serviceParams->type].launchCommand;
        } else {
            serviceParams->launchCommand = serviceParams->fileName;
        }
    }
}

void ServiceConfigurer::loadServiceTypes(QSettings *settings) {
    settings->beginGroup("Types");
    int count = settings->beginReadArray("type");
    for (int i = 0; i < count; i++) {
        settings->setArrayIndex(i);
        QString typeName = settings->value("name").toString();
        bool hasLauncher = settings->value("hasLauncher").toBool();
        QString launchCommand = settings->value("launchCommand").toString();
        QString launchParams = settings->value("launchParams").toString();
        QString startedRegex = settings->value("startedRegex").toString();
        int extensionCount = settings->value("extensionCount").toInt();
        settings->beginReadArray("extensions");
        QStringList extensions;
        for (int j = 0; j < extensionCount; j++) {
            settings->setArrayIndex(j);
            extensions.append(settings->value("item").toString());
        }
        settings->endArray();
        serviceTypes.insert(typeName, ServiceType(hasLauncher, launchCommand, launchParams, extensions, startedRegex));
    }
    settings->endArray();
    settings->endGroup();
}
