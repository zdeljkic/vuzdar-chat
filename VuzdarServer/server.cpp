#include "server.h"
#include "client.h"

Server::Server()
{
    nextClientId = 1;

    connect(&server, SIGNAL(newConnection()), this, SLOT(createClient()));
}

Server::~Server()
{
    stopServer();
}

bool Server::startServer(QString password, quint16 port)
{
    this->password = password;

    return server.listen(QHostAddress::Any, port);
}

void Server::stopServer()
{
    QMap <quint16, Client *>::const_iterator i = clients.constBegin();

    while (i != clients.constEnd()) {
        delete i.value();
        ++i;
    }

    clients.clear();
    nextClientId = 1;

    server.close();
}

void Server::processData(quint16 clientId, QByteArray data)
{
    QString infoText;
    infoText = QString::number(clientId);
    infoText += ": ";
    infoText += QString::fromUtf8(data);

    emit newInfoText(infoText);

    clients[clientId]->sendData(QString("ok šefe").toUtf8());
}

void Server::createClient()
{
    Client *newClient = new Client(nextClientId, server.nextPendingConnection(), this);

    clients[nextClientId] = newClient;
    ++nextClientId;

    QString infoText;
    infoText = "-= New client connected from ";
    infoText += newClient->getAddress().toString();
    infoText += ":";
    infoText += QString::number(newClient->getPort());
    infoText += ", client id = ";
    infoText += QString::number(newClient->getId());
    infoText += " =-";

    emit newInfoText(infoText);
}
