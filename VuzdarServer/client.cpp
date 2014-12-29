#include "client.h"
#include "server.h"

Client::Client(quint16 id, QTcpSocket *socket, Server *server)
{
    this->id = id;
    this->socket = socket;
    this->server = server;

    connect(this->socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

Client::~Client()
{
    delete socket;
}

void Client::setNickname(QString nickname)
{
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

void Client::sendData(QByteArray data)
{
    socket->write(data);
}

QHostAddress Client::getAddress()
{
    return socket->peerAddress();
}

quint16 Client::getPort()
{
    return socket->peerPort();
}

void Client::receiveData()
{
    server->processData(id, socket->readAll());
}
