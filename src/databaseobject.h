#ifndef DATABASEOBJECT_H
#define DATABASEOBJECT_H

#include <QObject>
#include <QtSql>

#define FIELD(TYPE, NAME) \
    Q_PROPERTY(TYPE NAME READ get##NAME WRITE set##NAME) \
    public: \
    TYPE get##NAME () const { return m_##NAME; } \
    void set##NAME (const TYPE &value) { m_##NAME = value; } \
    private: \
    TYPE m_##NAME = TYPE();

#define PRIMARY_KEY(keys...) \
    Q_CLASSINFO("PK", #keys)

#define NOT_NULL_FIELDS(fields...) \
    Q_CLASSINFO("NotNull", #fields)

#define TABLE(TABLENAME) \
    Q_CLASSINFO("TableName", #TABLENAME)

typedef struct { int propertyIndex; QString fieldName; QVariant::Type type; } FieldInfo;

class DatabaseObject;
typedef QSharedPointer<DatabaseObject> DBObjectPointer;
typedef QList<DBObjectPointer> DBObjectList;

class DatabaseObject : public QObject
{

    Q_OBJECT
public:
    Q_INVOKABLE explicit DatabaseObject(QObject *parent = 0);

    void clear();

    //ORM-related functions
    DBObjectList getAll(const QString &whereClause = QString()) const;
    DBObjectPointer getById(QVariant idValue, QString idFieldName = QString("id")) const;
    //int add/persist()
    //bool modify/save()
    //bool delete()

    //Metadata-related functions
    QList<FieldInfo> getFields() const;
    QStringList getPK() const;
    QStringList getNotNull() const;
    QString getTableName() const;

    //SQL-related functions
    QString getTableDDL() const;
    QString getSelect(const QString &whereClause = QString()) const;
    QString getInsert() const;
    QString getUpdate(const QString &whereClause = QString()) const;
    QString getUpdate(const QList<FieldInfo> &indexFields) const;
    QString getDelete(const QString &whereClause = QString()) const;

protected:
    bool isPK(const QString &fieldName) const;
    bool isNullField(const QString &fieldName) const;
    bool isInteger(QVariant::Type type) const;
    int findPropertyCaseInsensitive(QString propertyName) const;
    DBObjectPointer parseQueryResult(QSqlQuery query) const;
};

QDebug operator<< (QDebug d, const DatabaseObject &object);
QDebug operator<< (QDebug d, const DBObjectPointer &object);

#endif // DATABASEOBJECT_H
