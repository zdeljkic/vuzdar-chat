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
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnecting()));

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

bool Client::isAlive()
{
    return alive;
}

void Client::setAlive(bool alive)
{
    this->alive = alive;
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
    // dodaj podatke bufferu i "nasjeckaj" u pakete
    buffer += socket->readAll();

    VuzdarPacket packet = VuzdarPacket::chopBuffer(buffer);

    if (packet.getType() == VuzdarPacket::MALFORMED_PACKET && packet.getControlCode() == 0x00) {
        // nedovoljno podataka u bufferu, pozovat ce se opet kad se napuni buffer
        return;
    }

    // !!DEBUG!!
    qDebug() << "Recieved packet";
    qDebug() << "From:" << id;
    qDebug() << "Data:" << packet.getRawData().toHex();
    qDebug() << "-----";

    server->processPacket(id, packet);
}

void Client::disconnecting()
{
    emit signalRemoval(id);
}
