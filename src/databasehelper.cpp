#include "databasehelper.h"

QString DatabaseHelper::getTypeString(QVariant::Type type, QString databaseDriverName)
{
    QString result;
    if (databaseDriverName == "QSQLITE"){
        switch (type) {
        case QVariant::Int:
        case QVariant::Bool:
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
