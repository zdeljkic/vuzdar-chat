#include "server.h"
#include "client.h"

Server::Server()
{
    nextClientId = 1;
    nextGroupId = 1;

    connect(&server, SIGNAL(newConnection()), this, SLOT(createClient()));
}

Server::~Server()
{
    stopServer();
}

bool Server::startServer(QString password, quint16 port)
{
    this->password = password;

    Group *g = new Group(0, "+ All", QList<Client *>()); // nulta grupa, ukljucuje sve
    groups.insert(0, g);

    return server.listen(QHostAddress::Any, port);
}

void Server::stopServer()
{
    QMap <quint16, Client*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i) {
        delete i.value();
    }

    clients.clear();
    nextClientId = 1;

    QMap <quint16, Group*>::const_iterator j;

    for (j = groups.constBegin(); j != groups.constEnd(); ++j) {
        delete j.value();
    }

    groups.clear();
    nextGroupId = 1;

    server.close();
}

void Server::processPacket(quint16 id, VuzdarPacket packet)
{
    VuzdarPacket::PacketType type = packet.getType();

    if (type == VuzdarPacket::REGISTRATION) {
        QString nickname = packet.getNickname();

        if (nickname.isNull()) {
            // nadimak ima nedopustene znakove
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodePacket(VuzdarPacket::REGISTRATION, 0xF2));
        } else if(nickname.length() > 50) {
            // nadimak predugacak
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodePacket(VuzdarPacket::REGISTRATION, 0xF1));
        } else if (!isNicknameUnique(nickname)) {
            // nadimak nije jedinstven
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodePacket(VuzdarPacket::REGISTRATION, 0xF0));
        } else {
            // nadimak dobar, klijent je sluzbeno registriran == ima nadimak
            // treba poslati:
            // 1. njemu odgovor da je nadimak dobar
            // 2. svima drugima da je doso novi klijent
            // 3. njemu listu aktivnih klijenata

            // 1.
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodePacket(VuzdarPacket::REGISTRATION, 0x00));

            // 2.
            QList<QPair<quint16, QString> > list;
            list.append(QPair<quint16, QString>(id, nickname));
            VuzdarPacket p = VuzdarPacket::generateAliveClientPacket(list);
            groups[0]->sendPacket(p);

            // postavi mu nadimak i dodaj ga u nultu grupu
            // to se radi tek sad da ne primi on sam paket da je on doso na server (2)
            clients[id]->setNickname(nickname);
            groups[0]->addMember(clients[id]);

            // 3.
            clients[id]->sendPacket(
                        VuzdarPacket::generateAliveClientPacket(generateAliveClientList()));

        }
    } else if (type == VuzdarPacket::TEXT_PRIVATE_MESSAGE) {
        if (!clients.contains(packet.getId())) {
            // klijent neaktivan/ne postoji
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodeIdPacket(VuzdarPacket::TEXT_PRIVATE_MESSAGE, 0xF0, packet.getId()));
        } else if (packet.getMessage().isNull()) {
            // poruka sadrzi nepodrzane znakove
            clients[id]->sendPacket(
                        VuzdarPacket::generateControlCodeIdPacket(VuzdarPacket::TEXT_PRIVATE_MESSAGE, 0xF1, packet.getId()));
        } else {
            // sve ok:
            // 1. njemu odgovaram da je sve ok
            // 2. saljem poruku primatelju

            // 1.
            clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                        VuzdarPacket::TEXT_PRIVATE_MESSAGE, 0x00, packet.getId()));

            // 2.
            clients[packet.getId()]->sendPacket(VuzdarPacket::generateTextPrivateMessagePacket(
                                                    0x00, id, packet.getMessage()));
        }
    } else if (type == VuzdarPacket::TEXT_GROUP_MESSAGE) {
        quint8 controlCode = packet.getControlCode();

        if (controlCode == 0x00) {
            // grupna poruka

            if (!groups.contains(packet.getId())) {
                // ne postoji grupa
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF3, packet.getId()));
            } else if (packet.getMessage().isNull()) {
                // nedopusteni znakovi u poruci
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF4, packet.getId()));
            } else {
                // poruka je dobra:
                // 1. reci mu to
                // 2. proslijedi svima poruku

                // 1.
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0x02, packet.getId()));

                // 2.
                groups[packet.getId()]->sendPacket(VuzdarPacket::generateTextGroupMessagePacket(
                                                       0x00, packet.getId(), id, packet.getMessage()));
            }
        } else if (controlCode == 0x10) {
            // zahtjev za stvaranje grupe

            if (packet.getGroupName().isNull()) {
                // nedopusteni znakovi u imenu grupe
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF1, packet.getId()));
            } else if (packet.getGroupName().length() > 50) {
                // predugacko ime grupe
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF0, packet.getId()));
            } else {
                // provjeri jel prvi id bas id onog ko je poslo ovo
                // da sprijecimo laziranje "ko je stvorio grupu"
                // ako nije, odgovori sa error paketom, nepravilan paket
                QList<quint16> list = packet.getGroupClientList();

                if (list[0] != id)
                    clients[id]->sendPacket(VuzdarPacket::generateControlCodePacket(
                                                VuzdarPacket::ERROR, 0x01));

                // provjeri jel svi klijenti postoje
                for (int i = 0; i < list.size(); ++i)
                    if (!clients.contains(list[i])) {
                        // neki klijent ne postoji
                        clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                                    VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF2, packet.getId()));
                        return;
                    }

                // svi klijenti postoje, treba:
                // 1. odgovoriti da je grupa stvorena
                // 2. javiti svima da su dodani u grupu
                quint16 groupId = getNextGroupId();
                QList<Client *> list2;

                for (int i = 0; i < list.size(); ++i)
                    list2.append(clients[list[i]]);

                Group *g = new Group(groupId, packet.getGroupName(), list2);
                groups.insert(groupId, g);

                // 1.
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0x01, groupId));

                // 2.
                VuzdarPacket p = VuzdarPacket::generateNewGroupPacket(groupId, packet.getGroupName(), list);
                g->sendPacket(p);
            }
        } else if (controlCode == 0x11) {
            // klijent zeli izac iz grupe

            if (!groups.contains(packet.getId())) {
                // grupa ne postoji
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF3, packet.getId()));
            } else {
                // sve je ok, izbaci ga iz grupe i posalji:
                // 1. njemu da je izbacen iz grupe
                // 2. ostalima da je izaso iz grupe
                groups[packet.getId()]->removeMember(clients[id]);

                // 1.
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0x03, packet.getId()));

                // 2.
                groups[packet.getId()]->sendPacket(VuzdarPacket::generateGroupMemberChangePacket(
                                                       0x11, packet.getId(), id));

                // provjeri jel grupe vise ima clanova
                // ako nema - izbrisi ju
                if (groups[packet.getId()]->getClientList().size() == 0) {
                    delete groups[packet.getId()];
                    groups.remove(packet.getId());
                }
            }
        } else if (controlCode == 0x12) {
            // klijent dodaje nekog u grupu

            if (!groups.contains(packet.getId())) {
                // grupa ne postoji
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF3, packet.getId()));
            } else if(!clients.contains(packet.getSecondId())) {
                // klijent (kojeg se dodaje) ne postoji
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF5, packet.getId()));
            } else if(groups[packet.getId()]->getClientList().contains(packet.getSecondId())) {
                // klijent je vec u grupi
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0xF6, packet.getId()));
            } else {
                // sve je ok, dodaj ga u grupu i posalji:
                // 1. prvom klijentu da je dodavanje uspjesno
                // 2. ostalima da je dodan u grupe
                // 3. dodanom klijentu poruku da je dodan u grupu
                quint16 groupId = packet.getId();

                // 1.
                clients[id]->sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                                            VuzdarPacket::TEXT_GROUP_MESSAGE, 0x04, groupId));

                // 2.
                groups[groupId]->sendPacket(VuzdarPacket::generateGroupMemberChangePacket(
                                                       0x12, groupId, packet.getSecondId()));

                // dodaj ga u grupu
                // to se tek sad radi da ne primi paket da je dodan novi clan (on) u grupu (pod 2.)
                groups[groupId]->addMember(clients[packet.getSecondId()]);

                // 3.
                clients[packet.getSecondId()]->sendPacket(VuzdarPacket::generateNewGroupPacket(
                                                              groupId, groups[groupId]->getName(),
                                                              groups[groupId]->getClientList()));
            }
        } else {
            // nepoznati control code, saljem ERROR: nepravilan paket
            clients[id]->sendPacket(VuzdarPacket::generateControlCodePacket(
                                        VuzdarPacket::ERROR, 0x01));
        }
    } else if (type == VuzdarPacket::DISCONNECT) {

    }

}

bool Server::isNicknameUnique(QString nickname)
{
    QMap<quint16, Client*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i) {
        if (nickname == i.value()->getNickname())
            return false;
    }

    return true;
}

QList<QPair<quint16, QString> > Server::generateAliveClientList()
{
    // vraca sve sluzbeno registrirane klijente, tj one koji imaju nadimak
    QList<QPair<quint16, QString> > list;

    QMap <quint16, Client*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i)
        if (!i.value()->getNickname().isNull())
            list.append(QPair<quint16, QString>(i.key(), i.value()->getNickname()));

    return list;
}

quint16 Server::getNextClientId()
{
    return nextClientId++;
}

quint16 Server::getNextGroupId()
{
    return nextGroupId++;
}

void Server::createClient()
{
    quint16 nextClientId = getNextClientId();

    Client *newClient = new Client(nextClientId, server.nextPendingConnection(), this);
    connect(newClient, SIGNAL(signalRemoval(quint16)), this, SLOT(removeClient(quint16)));

    clients[nextClientId] = newClient;

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

void Server::removeClient(quint16 id)
{
    groups[0]->removeMember(clients[id]);
    delete clients[id];
    clients.remove(id);

    QList<quint16> list;
    list.append(id);

    groups[0]->sendPacket(VuzdarPacket::generateDeadClientPacket(list));

    QString infoText;
    infoText = "-= Client ";
    infoText += QString::number(id);
    infoText += " disconnected =-";

    emit newInfoText(infoText);
}
