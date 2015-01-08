#include "connection.h"

Connection::Connection(QObject *parent) :
    QObject(parent)
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

    return socket.waitForConnected(1000);
}

void Connection::disconnectFromServer()
{
    // ovo isto treba bolje nakodirat, da salje postenu VuzdarProtocol disconnect poruku
    // i da onda koristi disconnectFromHost i postepeno se disconnecta
    socket.abort();
}

void Connection::sendPacket(VuzdarPacket packet)
{
    socket.write(packet.getRawData());
}

void Connection::receiveData()
{
    // !!VAZNO!! treba sredit sjeckanje paketa
    emit newPacket(VuzdarPacket(socket.readAll()));
}
