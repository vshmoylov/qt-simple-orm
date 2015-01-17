#ifndef DATABASEOBJECT_H
#define DATABASEOBJECT_H

#include <QObject>
#include <QtSql>

//logging support
#ifndef LOG
#include <QDebug>
#define LOG(...) qDebug()
#endif

//orm-related macros
#define FIELD(TYPE, NAME, DBTYPE...) \
    Q_PROPERTY(TYPE NAME READ get_##NAME WRITE set_##NAME) \
    Q_CLASSINFO(#NAME, #DBTYPE) \
    public: \
    TYPE get_##NAME () const { return m_##NAME; } \
    void set_##NAME (const TYPE &value) { m_##NAME = value; } \
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
//QStringList a;

template<typename T>QList< QSharedPointer<T> > castList(const DBObjectList &objList)
{
     QList< QSharedPointer<T> > result;
     foreach (const DBObjectPointer &ptr, objList) {
         result.append(ptr.objectCast<T>());
     }
     return result;
}

template<typename T>QList<T> derefCastList(const DBObjectList &objList)
{
    QList<T> result;
    foreach (const DBObjectPointer &ptr, objList) {
        result.append(T(*ptr));
    }
    return result;
}

class DatabaseObject : public QObject
{

    Q_OBJECT
public:
    Q_INVOKABLE explicit DatabaseObject(QObject *parent = 0);

    void clear();

    //ORM-related functions
    DBObjectList getAll(const QString &whereClause = QString()) const;
    DBObjectPointer getById(QVariant idValue, QString idFieldName = QString("id")) const;
    virtual int addObject();     //int add/persist()
    virtual bool modifyObject(); //bool modify/save()
    virtual bool deleteObject(); //bool delete()

    static DBObjectList getAll(const QMetaObject &meta, const QString &whereClause = QString());
//    template<class T> static QList< QSharedPointer<T> > getAll1();
    static DBObjectPointer getById(const QMetaObject &meta, QVariant idValue, QString idFieldName = QString("id"));
    static DBObjectPointer newInstance(const QMetaObject &meta);


    //Metadata-related functions
    QList<FieldInfo> getFields() const;
    QStringList getPK() const;
    QStringList getNotNull() const;
    QString getTableName() const;

    //SQL-related functions
    QString getTableDDL() const;
    QString getDatabaseType(const QString &fieldName) const;
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
    void logQueryState(const QSqlQuery &query) const;
};

QDebug operator<< (QDebug d, const DatabaseObject &object);
QDebug operator<< (QDebug d, const DBObjectPointer &object);

#endif // DATABASEOBJECT_H
