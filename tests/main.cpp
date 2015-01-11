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

     DBObjectList clients = DatabaseObject::getAll(Client::staticMetaObject);
//     QList<QSharedPointer<Client> > clients2 = DatabaseObject::getAll1<Client>();
     qDebug() << clients;
     qDebug() << cli.getNotNull();
     qDebug() << cli.getTableDDL();

     Client b;
     b.set_name("bbbbb");
     b.set_surname("bbbbbb");
     b.set_birthdate(QDate(1992, 03, 27));
     b.addObject();

     clients = cli.getAll();
     qDebug() << clients;

     b.set_name("bbb123");
     b.modifyObject();

     clients = cli.getAll();
     qDebug() << clients;

     b.deleteObject();

     clients = cli.getAll();
     qDebug() << clients;
     return 0;
}
