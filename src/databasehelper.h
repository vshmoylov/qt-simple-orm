#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QVariant>

class DatabaseHelper
{
public:
    static QString getTypeString(QVariant::Type type, const QString &databaseDriverName);
    static QString getFieldValue(const QVariant &value);
    static QString getFieldValue(const QVariant &value, const QString &databaseDriverName);
    static QString getLastIdSql();
};

#endif // DATABASEHELPER_H
