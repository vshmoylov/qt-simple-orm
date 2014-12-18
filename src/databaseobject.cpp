#include "databaseobject.h"
#include "databasehelper.h"

DatabaseObject::DatabaseObject(QObject *parent) :
    QObject(parent)
{
}

QString DatabaseObject::getTableDDL(QString tableName)
{
    QString tableDDL;
    if (tableName.isEmpty())
        tableName = this->metaObject()->className();

    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    QList<FieldInfo> fields = getFields();

    if (sqlDriverName == "QSQLITE"){
        tableDDL = "CREATE TABLE IF NOT EXISTS " + tableName + " ( ";
        foreach (const FieldInfo &field, fields) {
            QString type = DatabaseHelper::getTypeString(field.second, sqlDriverName).toUpper();
            bool isNull = isNullField(field.first);
            QString name = field.first;
            QString additionalTypeStr = "";
            if (isPK(name) && type == "INTEGER"){
                additionalTypeStr = "NOT NULL PRIMARY KEY AUTOINCREMENT";
            } else if (!isNull){
                additionalTypeStr = "NOT NULL";
            }
            if (!tableDDL.endsWith("( "))
                tableDDL.append(", ");
            tableDDL.append(QString("%1 %2 %3").arg(name).arg(type).arg(additionalTypeStr).trimmed());
        }
        tableDDL +=");";
    }
    return tableDDL;
}

QList<FieldInfo> DatabaseObject::getFields()
{
    QList< QPair<QString, QVariant::Type> > result;
    const QMetaObject *obj = this->metaObject();
    int propsCount = obj->propertyCount();
    int propsOffset = obj->propertyOffset();
    for (int i=propsOffset; i<propsCount; i++){
        QMetaProperty property = obj->property(i);
        result.append(qMakePair(QString(property.name()), property.type()));
    }
    return result;
}

QStringList DatabaseObject::getPK() const
{
    return QStringList();
}

QStringList DatabaseObject::getNotNull() const
{
    return QStringList();
}

bool DatabaseObject::isPK(QString fieldName)
{
    return getPK().contains(fieldName, Qt::CaseInsensitive);
}

bool DatabaseObject::isNullField(QString fieldName)
{
    return !getNotNull().contains(fieldName, Qt::CaseInsensitive);
}
