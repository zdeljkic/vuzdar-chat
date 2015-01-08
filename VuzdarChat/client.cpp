#include "client.h"

Client::Client(quint16 id, QString nickname)
{
    this->id = id;
    this->nickname = nickname;
}

quint16 Client::getId()
{
    return id;
}

QString Client::getNickname()
{
    return nickname;
}

