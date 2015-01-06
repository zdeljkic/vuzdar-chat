#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPair>
#include <QTcpSocket>
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

    bool startServer(QString password, quint16 port, bool useOldConfig);
    void stopServer();
    void processPacket(quint16 id, VuzdarPacket packet);
    bool configFileExists();

public slots:

private:
    QTcpServer server;
    QMap<quint16, Client*> clients;
    QMap<quint16, Group*> groups;
    AdminInfo adminInfo;

    quint16 nextClientId;
    quint16 nextGroupId;

    quint16 getNextClientId();
    quint16 getNextGroupId();

    bool isNicknameUnique(QString nickname);
    QList<QPair<quint16, QString> > generateAliveClientList();

private slots:
    void createClient();
    void removeClient(quint16 id, bool timeout = false);

signals:
    void newInfoText(QString text);

};

#endif // SERVER_H
