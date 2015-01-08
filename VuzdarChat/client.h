#ifndef CLIENT_H
#define CLIENT_H

#include <QString>

class Client
{
public:
    Client(quint16 id, QString nickname);

    quint16 getId();
    QString getNickname();

private:
    quint16 id;
    QString nickname;
};

#endif // CLIENT_H
