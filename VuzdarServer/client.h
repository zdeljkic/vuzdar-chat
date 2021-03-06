#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "../VuzdarCommon/vuzdarpacket.h"

class Server;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(quint16 id, QTcpSocket *socket, Server *server);
    ~Client();

    quint16 getId();
    QHostAddress getAddress();
    quint16 getPort();
    void setNickname(QString nickname);
    QString getNickname();
    bool isAlive();
    void setAlive(bool alive);
    bool isAdmin();
    void setAdmin(bool admin);

    void sendPacket(VuzdarPacket packet);

public slots:

private:
    quint16 id;
    QString nickname;
    QTcpSocket *socket;
    Server *server;
    bool alive;
    bool admin;
    QByteArray buffer;

private slots:
    void receiveData();
    void disconnecting();

signals:
    void signalRemoval(quint16 id);
};

#endif // CLIENT_H
