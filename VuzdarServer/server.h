#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPair>
#include <QTcpServer>
#include "../VuzdarCommon/vuzdarpacket.h"
#include "group.h"
#include "admininfo.h"

class Client;

class Server : public QObject
{
    Q_OBJECT

public:
    Server();
    ~Server();

    bool startServer(QString password, quint16 port);
    void stopServer();
    void processPacket(quint16 id, VuzdarPacket packet);
    bool isNicknameUnique(QString nickname);
    QList<QPair<quint16, QString> > generateAliveClientList();

public slots:

private:
    QTcpServer server;
    QMap<quint16, Client*> clients;
    QMap<quint16, Group*> groups;
    quint16 nextClientId;
    quint16 nextGroupId;

    quint16 getNextClientId();
    quint16 getNextGroupId();

    AdminInfo loadData();
    void saveData();

private slots:
    void createClient();
    void removeClient(quint16 id);

signals:
    void newInfoText(QString text);

};

#endif // SERVER_H
