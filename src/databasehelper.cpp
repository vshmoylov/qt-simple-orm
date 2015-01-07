#include "databasehelper.h"
#include <QDate>
#include <QSqlDatabase>

QString DatabaseHelper::getTypeString(QVariant::Type type, const QString &databaseDriverName)
{
    QString result;
    if (databaseDriverName == "QSQLITE"){
        switch (type) {
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::UInt:
        case QVariant::ULongLong:
            result = "INTEGER";
            break;
        case QVariant::String:
            result = "VARCHAR";
            break;
        case QVariant::Date:
            result = "DATE";
            break;
        case QVariant::Time:
            result = "TIME";
            break;
        case QVariant::Char:
            result = "CHAR";
            break;
        default:
            result = "VARCHAR";
            break;
        }
    }
    return result;
}

QString DatabaseHelper::getFieldValue(const QVariant &value)
{
    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    return getFieldValue(value, sqlDriverName);
}

QString DatabaseHelper::getFieldValue(const QVariant &value, const QString &databaseDriverName)
{
    switch (value.type()) {
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::UInt:
    case QVariant::ULongLong:
        return value.toString();
    case QVariant::Date:
        return QString("'%1'").arg(value.toDate().toString(Qt::ISODate));
    default:
        return QString("'%1'").arg(value.toString());
    }
}
