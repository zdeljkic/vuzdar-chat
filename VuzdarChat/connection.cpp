#include "connection.h"

Connection::Connection(QObject *parent) :
    QObject(parent),
    connected(false)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

QHostAddress Connection::getAddress()
{
    return socket.peerAddress();
}

quint16 Connection::getPort()
{
    return socket.peerPort();
}

bool Connection::connectToServer(QString hostname, quint16 port)
{
    // Ovo bi mozda trebalo bolje nakodirat (koristit signale), problem je sta:
    // 1. koristi waitForConnected koji zna imat problema na Windowsima
    // 2. bloka, ceka 1s da se uspostavi veza, ako ne vraca false
    //  -> tu je problem i sta bloka, i sta ako treba vise od 1s da se uspostavi veza nece se spojit
    // Trebalo bi napravit da ova klasa emitira signal statusChanged(<tip> status) koji ti preko
    // tog statusa kaze jel nekonektiran (pocetno), konektiran, diskonektiran, greska se desila itd itd
    socket.connectToHost(hostname, port);

    return (connected = socket.waitForConnected(1000));
}

void Connection::disconnectFromServer()
{
    // ovo isto treba bolje nakodirat, da salje postenu VuzdarProtocol disconnect poruku
    // i da onda koristi disconnectFromHost i postepeno se disconnecta
    socket.disconnectFromHost();

    if (socket.state() == QTcpSocket::UnconnectedState || socket.waitForDisconnected(1000))
        connected = false;
}

void Connection::sendPacket(VuzdarPacket packet)
{
    socket.write(packet.getRawData());

    // !!DEBUG!!
    qDebug() << "Sent packet";
    qDebug() << "Data:" << packet.getRawData().toHex();
    qDebug() << "-----";
}

bool Connection::isConnected()
{
    return connected;
}

void Connection::receiveData()
{
    // dodaj podatke bufferu i "nasjeckaj" u pakete
    buffer += socket.readAll();
    qDebug() << "Buffer:" << buffer.toHex();

    VuzdarPacket packet = VuzdarPacket::chopBuffer(buffer);

    if (packet.getType() == VuzdarPacket::MALFORMED_PACKET && packet.getControlCode() == 0x00) {
        // nedovoljno podataka u bufferu, pozovat ce se opet kad se napuni buffer
        return;
    }

    // !!DEBUG!!
    qDebug() << "Received packet";
    qDebug() << "Data:" << packet.getRawData().toHex();
    qDebug() << "-----";

    emit newPacket(packet);
    receiveData();
}
