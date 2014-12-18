#ifndef OBJB_H
#define OBJB_H
#include "databaseobject.h"

class ObjB : public DatabaseObject
{
    Q_OBJECT
    FIELD(int, id)
    FIELD(QString, name)
    PRIMARY_KEY(id)
    NOT_NULL_FIELDS(name)
public:
    explicit ObjB(QObject *parent = 0);

};

#endif // OBJB_H
