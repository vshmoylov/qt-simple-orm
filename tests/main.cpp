#include <QCoreApplication>
#include <QDebug>
#include "objb.h"
#include "client.h"
#include <QtSql>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    /*ObjB a;
    a.setname("Vitya");
    DatabaseObject *o = &a;
    const QMetaObject *obj = o->metaObject();
    int methods = obj->methodCount();
    qDebug() << "Method count: " << methods;
    for (int i=0; i<methods; i++){
        QMetaMethod method = obj->method(i);
        qDebug() << method.methodSignature();
    }

    QList<FieldInfo> fields = o->getFields();
//    qDebug() << fields;
    qDebug() << o->getPK();
    qDebug() << o->getNotNull();
    qDebug() << o->getTableDDL();
    qDebug() << o->getInsert();
    qDebug() << o->getUpdate();
    qDebug() << o->getDelete();

*/
     QString dbFileName = "C:\\data.sqlite";
     db.setDatabaseName(dbFileName);
     db.open();
     Client cli;
     DBObjectList clients = cli.getAll();
     qDebug() << clients;
     qDebug() << cli.getNotNull();
     qDebug() << cli.getTableDDL();
    return 0;
}
