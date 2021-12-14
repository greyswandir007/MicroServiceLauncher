#ifndef SERVICETYPES_H
#define SERVICETYPES_H
#include "servicetype.h"

#include <QString>
#include <QStringList>
#include <QMap>

const QMap<QString, ServiceType> defaultServiceTypes({
            {"java", ServiceType(true, "java", "-jar", QStringList({".jar"}), "Started [a-zA-Z0-9]* in [0-9]*[.][0-9]* [a-z]* [(]JVM running for")},
            {"exe", ServiceType(false, "", "", QStringList({".exe"}), "")},
            {"bat", ServiceType(true, "cmd", "/c", QStringList({".bat"}), "")},
            {"sh", ServiceType(true, "/bin/sh", "", QStringList({".sh"}), "")}
});


#endif // SERVICETYPES_H
