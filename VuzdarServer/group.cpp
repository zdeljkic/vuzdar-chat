#include "group.h"

Group::Group(quint16 id, QString name, QList<Client *> clients)
{
    this->id = id;
    this->name = name;
    this->clients = clients;
}

Group::~Group()
{

}

void Group::sendPacket(VuzdarPacket packet)
{
    for (int i = 0; i < clients.size(); ++i) {
        clients[i]->sendPacket(packet);
    }
}

void Group::addMember(Client *client)
{
    if (!clients.contains(client))
        clients.append(client);
}

void Group::removeMember(Client *client)
{
    clients.removeAll(client);
}

quint16 Group::getId()
{
    return id;
}

QString Group::getName()
{
    return name;
}

QList<quint16> Group::getClientList()
{
    QList<quint16> list;

    for (int i = 0; i < clients.size(); ++i)
        list.append(clients[i]->getId());

    return list;
}

bool Group::isMember(quint16 id)
{
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i]->getId() == id)
            return true;
    }

    return false;
}

