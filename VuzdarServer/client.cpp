#include "client.h"
#include "server.h"

Client::Client(quint16 id, QTcpSocket *socket, Server *server)
{
    this->id = id;
    this->socket = socket;
    this->server = server;

    connect(this->socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(this->socket, SIGNAL(disconnected()), this, SLOT(disconnecting()));
}

Client::~Client()
{
    socket->disconnectFromHost();

    if (socket->state() == QTcpSocket::UnconnectedState || socket->waitForDisconnected(1000))
        socket->deleteLater();
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

void Client::sendPacket(VuzdarPacket packet)
{
    socket->write(packet.getRawData());

    // !!DEBUG!!
    qDebug() << "Sent packet";
    qDebug() << "To:" << id;
    qDebug() << "Data:" << packet.getRawData().toHex();
    qDebug() << "-----";
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
    QByteArray data = socket->readAll();

    /*
     * !!VAZNO!!
     * Ovdje bi bilo dobro napraviti da "sjecka" podakte koje prima u pakete
     * i salje ih tako serveru (pomocu nekog buffera), za slucaj da 1 VuzdarPacket
     * nije strpan tocno u 1 TCP paket. Trenutno to nije potrebno ako je klijent
     * bas nas VuzdarChat, al ono bilo bi ok to ubuduce napravit.
     */

    // !!DEBUG!!
    qDebug() << "Recieved packet";
    qDebug() << "From:" << id;
    qDebug() << "Data:" << data.toHex();
    qDebug() << "-----";

    server->processPacket(id, VuzdarPacket(data));
}

void Client::disconnecting()
{
    emit signalRemoval(id);
}
