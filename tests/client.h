#ifndef CLIENT_H
#define CLIENT_H
#include "databaseobject.h"

class Client : public DatabaseObject
{
    Q_OBJECT

    TABLE(clients)

    FIELD(int, id)
    FIELD(QString, name)
    FIELD(QString, surname)
    FIELD(QString, middlename)
    FIELD(QDate, birthdate)
    FIELD(QString, phone)
    FIELD(QString, phone2)
    FIELD(QString, description)
    FIELD(int, doctor_id)
    FIELD(QString, policy)

    PRIMARY_KEY(id)

    NOT_NULL_FIELDS(id, name, surname, middlename, birthdate)
public:
    Q_INVOKABLE explicit Client(QObject *parent = 0);


};

#endif // CLIENT_H
