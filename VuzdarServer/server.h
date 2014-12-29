#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>
#include <QTcpServer>

class Client;

class Server : public QObject
{
    Q_OBJECT

public:
    Server();
    ~Server();

    bool startServer(QString password, quint16 port);
    void stopServer();
    void processData(quint16 clientId, QByteArray data);

public slots:

private:
    QTcpServer server;
    QMap <quint16, Client*> clients;
    quint16 nextClientId;
    QString password;

private slots:
    void createClient();

signals:
    void newInfoText(QString text);

};

#endif // SERVER_H
