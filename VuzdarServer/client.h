#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

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

    void sendData(QByteArray data);

public slots:

private:
    quint16 id;
    QString nickname;
    QTcpSocket *socket;
    Server *server;

private slots:
    void receiveData();

signals:

};

#endif // CLIENT_H
