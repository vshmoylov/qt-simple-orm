#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H


#include <QVariant>

class DatabaseHelper
{
public:
    static QString getTypeString(QVariant::Type type, QString databaseDriverName);
    static QString getFieldValue(QVariant value, QString databaseDriverName);
};

#endif // DATABASEHELPER_H
