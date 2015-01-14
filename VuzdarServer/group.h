#ifndef GROUP_H
#define GROUP_H

#include <QList>
#include "client.h"
#include "../VuzdarCommon/vuzdarpacket.h"

class Group
{
public:
    Group(quint16 id, QString name, QList<Client *> clients);
    ~Group();
    void sendPacket(VuzdarPacket packet);
    void addMember(Client* client);
    void removeMember(Client* client);
    quint16 getId();
    QString getName();
    QList<quint16> getClientList();
    bool isMember(quint16 id);

private:
    quint16 id;
    QString name;
    QList<Client *> clients;
};

#endif // GROUP_H
