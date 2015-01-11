#include "databaseobject.h"
#include "databasehelper.h"

DatabaseObject::DatabaseObject(QObject *parent) :
    QObject(parent)
{
}

QString DatabaseObject::getTableDDL() const
{
    QString tableDDL;
    QString tableName = getTableName();

    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    QList<FieldInfo> fields = getFields();

    if (sqlDriverName == "QSQLITE"){
        tableDDL = "CREATE TABLE IF NOT EXISTS " + tableName + " ( ";
        foreach (const FieldInfo &field, fields) {
            QString type = DatabaseHelper::getTypeString(field.type, sqlDriverName).toUpper();
            QString name = field.fieldName;
            bool isNull = isNullField(name);
            QString constraints = "";
            if (isPK(name) && isInteger(field.type)){
                constraints = "NOT NULL PRIMARY KEY AUTOINCREMENT";
            } else if (!isNull){
                constraints = "NOT NULL";
            }
            if (!tableDDL.endsWith("( "))
                tableDDL.append(", ");
            tableDDL.append(QString("%1 %2 %3").arg(name).arg(type).arg(constraints).trimmed());
        }
        tableDDL +=");";
    }
    return tableDDL;
}

QString DatabaseObject::getSelect(const QString &whereClause) const
{
    if (whereClause.trimmed().isEmpty())
        return "SELECT * FROM " + getTableName();
    else
        return QString("SELECT * FROM %1 WHERE %2")
                .arg(getTableName())
                .arg(whereClause.trimmed());
}

QString DatabaseObject::getInsert() const
{
    QList<FieldInfo> fields = getFields();
    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    QString sql = "INSERT INTO " + getTableName() + " values (";
    foreach (const FieldInfo &field, fields) {
        QString name = field.fieldName;
        QMetaProperty property = this->metaObject()->property(field.propertyIndex);
        QVariant value = property.read(this);
        if (!sql.endsWith('('))
            sql += ", ";
        if (isPK(name) && isInteger(field.type)){
            sql += "NULL";
        } else {
            sql += DatabaseHelper::getFieldValue(value,sqlDriverName);
        }
    }
    sql += ")";
    return sql;
}

QString DatabaseObject::getUpdate(const QString &whereClause) const
{

    QList<FieldInfo> fields = getFields();
    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    QString sql = "UPDATE " + getTableName() + " SET ";
    foreach (const FieldInfo &field, fields) {
        QString name = field.fieldName;
        if (isPK(name))
            continue;
        QMetaProperty property = this->metaObject()->property(field.propertyIndex);
        QVariant value = property.read(this);
        if (!sql.endsWith("SET "))
            sql += ", ";
        sql += QString("%1 = %2").arg(name).arg(DatabaseHelper::getFieldValue(value,sqlDriverName));
    }
    sql += " WHERE ";
    if (whereClause.isEmpty()){
        QStringList pkList = getPK();
        foreach (const QString &field, pkList) {
            QMetaProperty property = this->metaObject()->property(this->metaObject()->indexOfProperty(field.toLatin1()));
            QVariant value = property.read(this);
            if (!sql.endsWith(" WHERE "))
                sql += " AND ";
            sql += QString("%1 = %2").arg(field).arg(DatabaseHelper::getFieldValue(value,sqlDriverName));
        }
    } else {
        sql += whereClause;
    }
    return sql;
}

QString DatabaseObject::getUpdate(const QList<FieldInfo> &indexFields) const
{
    QString whereClause;
    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    foreach (const FieldInfo &field, indexFields) {
        QMetaProperty property = this->metaObject()->property(field.propertyIndex);
        QVariant value = property.read(this);
        if (!whereClause.isEmpty()) {
            whereClause += " AND ";
        }
        whereClause += QString("%1 = %2").arg(field.fieldName).arg(DatabaseHelper::getFieldValue(value,sqlDriverName));
    }
    return getUpdate(whereClause);
}

QString DatabaseObject::getDelete(const QString &whereClause) const
{
    QString sqlDriverName = QSqlDatabase::database().driverName().toUpper();
    QString sql = "DELETE FROM " + getTableName() + " WHERE ";
    if (whereClause.isEmpty()){
        QStringList pkList = getPK();
        foreach (const QString &field, pkList) {
            QMetaProperty property = this->metaObject()->property(this->metaObject()->indexOfProperty(field.toLatin1()));
            QVariant value = property.read(this);
            if (!sql.endsWith(" WHERE "))
                sql += " AND ";
            sql += QString("%1 = %2").arg(field).arg(DatabaseHelper::getFieldValue(value,sqlDriverName));
        }
    } else {
        sql += whereClause;
    }
    return sql;
}

DBObjectList DatabaseObject::getAll(const QString &whereClause) const
{
    const char *classname = metaObject()->className();
    LOG(INFO) << "Reading ALL " << classname << " from DB";

    DBObjectList result;
    QString selectSql = getSelect(whereClause);
    QSqlQuery query(selectSql);

    logQueryState(query);

    while (query.next()){
        DBObjectPointer obj = parseQueryResult(query);
        result.append(obj);
    }

    LOG(INFO) << "Total "<<classname<<" records: " << result.size();
    return result;
}

DBObjectPointer DatabaseObject::getById(QVariant idValue, QString idFieldName) const
{
    /*    int ind = findPropertyCaseInsensitive("id");
    if (ind==-1)
        return DBObjectPointer();*/
    const char *classname = metaObject()->className();
    LOG(INFO) << "Reading " << classname << " with id = " << idValue;

    QString selectSql = getSelect(QString("%1 = %2").arg(idFieldName).arg(DatabaseHelper::getFieldValue(idValue)));
    QSqlQuery query(selectSql);

    logQueryState(query);

    if (query.next()){
        return parseQueryResult(query); // return retrieved data
    }
    return DBObjectPointer(); // empty ptr
}

int DatabaseObject::addObject()
{
    QString insertSql = getInsert();
    QString getIdSql = DatabaseHelper::getLastIdSql();
    const char *classname = metaObject()->className();
    LOG(INFO) << "Inserting new " << classname << " into database";
    QSqlQuery query;
    LOG(TRACE) << "Executing sql: " << insertSql;
    if (!query.exec(insertSql)){
        LOG(ERROR) << "Insert of new "<< classname <<" failed. Error text: " << query.lastError().text();
        return -1;
    }
    LOG(TRACE) << "Executing sql: " << getIdSql;
    query.clear();
    query.exec(getIdSql);
    if(query.next()){
        int id = query.value(0).toInt();
        int ind = findPropertyCaseInsensitive("id");
        QMetaProperty idProperty = metaObject()->property(ind);
        idProperty.write(this, id);
        LOG(INFO) << "Successfully added new " << classname << ", id is: " << id;
        return id;
    } else {
        return -1;
    }
}

bool DatabaseObject::modifyObject()
{
    const char *classname = metaObject()->className();
    LOG(INFO) << "Editing " << classname;
    QString updateSql = getUpdate();
    QSqlQuery query(updateSql);
    logQueryState(query);
    return query.isActive();
}

bool DatabaseObject::deleteObject()
{
    const char *classname = metaObject()->className();
    LOG(INFO) << "Deleting "<<classname;
    QString deleteSql = getDelete();
    QSqlQuery query(deleteSql);
    logQueryState(query);
    return query.isActive();
}

DBObjectPointer DatabaseObject::getById(const QMetaObject &meta, QVariant idValue, QString idFieldName)
{
    DBObjectPointer obj = newInstance(meta);
    return obj->getById(idValue, idFieldName);
}

DBObjectPointer DatabaseObject::newInstance(const QMetaObject &meta)
{
    QObject *newInst = meta.newInstance();
    return DBObjectPointer(qobject_cast<DatabaseObject*>(newInst));;
}

DBObjectList DatabaseObject::getAll(const QMetaObject &meta, const QString &whereClause)
{
    DBObjectPointer obj = newInstance(meta);
    return obj->getAll(whereClause);
}

QList<FieldInfo> DatabaseObject::getFields() const
{
    QList< FieldInfo > result;
    const QMetaObject *obj = this->metaObject();
    int propsCount = obj->propertyCount();
    int propsOffset = obj->propertyOffset();
    for (int i=propsOffset; i<propsCount; i++){
        QMetaProperty property = obj->property(i);
        result.append({i, QString(property.name()), property.type()});
    }
    return result;
}

QStringList DatabaseObject::getPK() const
{
    int idx = metaObject()->indexOfClassInfo("PK");
    if (idx<0)
        return QStringList();
    QMetaClassInfo metaInfo = metaObject()->classInfo(idx);
    return QString(metaInfo.value()).split(QRegExp("[, ]"), QString::SkipEmptyParts);
}

QStringList DatabaseObject::getNotNull() const
{
    int idx = metaObject()->indexOfClassInfo("NotNull");
    if (idx<0)
        return QStringList();
    QMetaClassInfo metaInfo = metaObject()->classInfo(idx);
    return QString(metaInfo.value()).split(QRegExp("[, ]"), QString::SkipEmptyParts);
}

QString DatabaseObject::getTableName() const
{
    int idx = metaObject()->indexOfClassInfo("TableName");
    if (idx<0)
        return metaObject()->className();
    return metaObject()->classInfo(idx).value();

}

void DatabaseObject::clear()
{
    QList<FieldInfo> fields = getFields();
    foreach (const FieldInfo &field, fields) {
        QMetaProperty property = this->metaObject()->property(field.propertyIndex);
        property.write(this, QVariant(field.type));
    }
}

bool DatabaseObject::isPK(const QString &fieldName) const
{
    return getPK().contains(fieldName, Qt::CaseInsensitive);
}

bool DatabaseObject::isNullField(const QString &fieldName) const
{
    return !getNotNull().contains(fieldName, Qt::CaseInsensitive);
}

bool DatabaseObject::isInteger(QVariant::Type type) const
{
    switch (type) {
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::UInt:
    case QVariant::ULongLong:
        return true;
    default:
        break;
    }
    return false;
}

int DatabaseObject::findPropertyCaseInsensitive(QString propertyName) const
{
    QList<FieldInfo> fields = getFields();
    foreach (const FieldInfo &field, fields) {
        if (0 == QString::compare(field.fieldName, propertyName, Qt::CaseInsensitive)){
            return field.propertyIndex;
        }
    }
    return -1;
}

DBObjectPointer DatabaseObject::parseQueryResult(QSqlQuery query) const
{
    QList<FieldInfo> fields = getFields();
    const QMetaObject *metaObj = this->metaObject();
    QObject *newInst = metaObj->newInstance();
    DatabaseObject *ptr = qobject_cast<DatabaseObject*>(newInst);
    DBObjectPointer result(ptr);
    for (int i=0; i<fields.size(); i++){
        const FieldInfo &field = fields[i];
        QMetaProperty property = metaObj->property(field.propertyIndex);
        QVariant value = query.value(i);
        property.write(result.data(), value);
    }
    return result;
}

void DatabaseObject::logQueryState(const QSqlQuery &query) const
{
    LOG(TRACE) << "Query state: " << (query.isActive()?"Active":"Inactive")
               << "; Rows affected: " << query.numRowsAffected()
               << "; SQL: " << query.lastQuery();
}


QDebug operator<<(QDebug d, const DatabaseObject &object)
{
    d << "\n{ ";
    QList<FieldInfo> fields = object.getFields();
    foreach (const FieldInfo &field, fields) {
        QMetaProperty property = object.metaObject()->property(field.propertyIndex);
        d << field.fieldName << ": " << property.read(&object) << "\n";
    }
    d << "}\n";
    return d;
}


QDebug operator<<(QDebug d, const DBObjectPointer &object)
{
    return operator <<(d, *object);
}


/*template<class T> QList<QSharedPointer<T> > DatabaseObject::getAll1()
{
    return castList<T>(DatabaseObject::getAll(T::staticMetaObject));
}*/
