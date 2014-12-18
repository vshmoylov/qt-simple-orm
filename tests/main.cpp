#include <QCoreApplication>
#include <QDebug>
#include "objb.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ObjB a;
    DatabaseObject *o = &a;
    const QMetaObject *obj = o->metaObject();
    int methods = obj->methodCount();
    qDebug() << "Method count: " << methods;
    for (int i=0; i<methods; i++){
        QMetaMethod method = obj->method(i);
        qDebug() << method.methodSignature();
    }

    QList<FieldInfo> fields = o->getFields();
    qDebug() << fields;
    qDebug() << o->getPK();
    qDebug() << o->getNotNull();
    qDebug() << o->getTableDDL();

    return 0;
}
