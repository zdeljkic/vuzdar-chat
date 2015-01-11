#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QByteArray>
#include "../VuzdarCommon/vuzdarpacket.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    QHostAddress getAddress();
    quint16 getPort();
    bool connectToServer(QString hostname, quint16 port);
    void disconnectFromServer();
    void sendPacket(VuzdarPacket packet);
    bool isConnected();

public slots:

private:
    bool connected;
    QTcpSocket socket;
    QByteArray buffer;

private slots:
    void receiveData();

signals:
    void newPacket(VuzdarPacket packet);
};

#endif // CONNECTION_H
