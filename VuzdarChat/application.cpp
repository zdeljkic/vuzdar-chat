#include "application.h"
#include "ui_application.h"

Application::Application(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Application),
    adminWindow(0)
{
    ui->setupUi(this);
    state = UNCONNECTED;

    connect(&connection, SIGNAL(newPacket(VuzdarPacket)), this, SLOT(processPacket(VuzdarPacket)));
    connect(&connection, SIGNAL(disconnected()), this, SLOT(on_disconnectButton_clicked()));

    newMessage.setSource(QUrl::fromLocalFile("new_message.wav"));
    newMessage.setVolume(1.0f);
}

Application::~Application()
{
    on_disconnectButton_clicked();
    delete ui;
}

void Application::addText(QString text)
{
    ui->displayText->append(text);
}

void Application::on_connectButton_clicked()
{
    // provjeri jel port dobar
    bool ok;
    ui->portText->text().toUShort(&ok);

    if (!ok || ui->portText->text().toUShort() > 65535) {
        addText("--== Didn't try to connect, port invalid ==--");
        return;
    }

    // provjeri jel nadimak upisan
    if (ui->nicknameText->text().isEmpty()) {
        addText("Please enter the nickname before connecting.");
        return;
    }

    // probaj se konektirat
    addText("--== Establishing connection to the server... ==--");
    if (connection.connectToServer(ui->hostnameText->text(), ui->portText->text().toUShort())) {
        addText("--== Established connection to the server, registering nickname... ==--");
        changeState(REGISTERING);

        // probaj registrirat nadimak
        connection.sendPacket(VuzdarPacket::generateRegistrationPacket(
                                  ui->nicknameText->text()));
    } else {
        addText("--== Connection unsuccessful ==--");
    }
}

void Application::on_disconnectButton_clicked()
{
    if (connection.isConnected()) {
        connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                              VuzdarPacket::DISCONNECT, 0x00));
        connection.disconnectFromServer();
    }

    QMap<quint16, Conversation*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i) {
        delete i.value();
    }

    clients.clear();

    for (i = groups.constBegin(); i != groups.constEnd(); i++) {
        delete i.value();
    }

    groups.clear();

    kickQueue.clear();
    banQueue.clear();
    unbanQueue.clear();

    addText("--== Disconnected from the server ==--");
    changeState(UNCONNECTED);
}

void Application::changeState(Application::State state)
{
    this->state = state;

    if (state == UNCONNECTED) {
        ui->hostnameText->setEnabled(true);
        ui->portText->setEnabled(true);
        ui->nicknameText->setEnabled(true);
        ui->connectButton->setEnabled(true);

        ui->disconnectButton->setEnabled(false);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(false);
        ui->openAdminButton->setEnabled(false);
        ui->newGroupButton->setEnabled(false);
    } else if(state == REGISTERING) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->nicknameText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(false);
        ui->openAdminButton->setEnabled(false);
        ui->newGroupButton->setEnabled(false);
    } else if (state == CONNECTED_UNAUTHENTICATED) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->nicknameText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(true);
        ui->authButton->setEnabled(true);
        ui->deauthButton->setEnabled(false);
        ui->openAdminButton->setEnabled(false);
        ui->newGroupButton->setEnabled(true);
    } else if (state == CONNECTED_AUTHENTICATED) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->nicknameText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(true);
        ui->openAdminButton->setEnabled(true);
        ui->newGroupButton->setEnabled(true);
    }
}

QList<Conversation *> Application::getClientList()
{
    QList<Conversation*> list;
    QMap<quint16, Conversation*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i) {
        list.append(i.value());
    }

    return list;
}

void Application::addClientButton(QPushButton *button)
{
    // ubaci gumbic na svoje abecedno mjesto
    // trenutno abecedno == ko je prvi unicode znak
    QVBoxLayout *layout = (QVBoxLayout *) ui->clientGroupBox->layout();

    int i;

    for (i = 0; i < layout->count() - 1; ++i) {
        // u layoutu su gumbici i jedan spacer na kraju
        QPushButton *button2 = (QPushButton *) layout->itemAt(i)->widget();

        if (button2->text() > button->text())
            break;
    }

    layout->insertWidget(i, button);
}

void Application::addGroupButton(QPushButton *button)
{
    // ubaci gumbic na svoje abecedno mjesto
    // trenutno abecedno == ko je prvi unicode znak
    QVBoxLayout *layout = (QVBoxLayout *) ui->groupGroupBox->layout();

    int i;

    for (i = 1; i < layout->count() - 1; ++i) {
        // u layoutu je new group gumbic, ostali gumbici i jedan spacer na kraju
        QPushButton *button2 = (QPushButton *) layout->itemAt(i)->widget();

        if (button2->text() > button->text())
            break;
    }

    layout->insertWidget(i, button);
}

void Application::on_newGroupButton_clicked()
{
    QList<Conversation*> list = getClientList();
    list.removeAll(clients[myId]); // makni mene

    NewGroupWindow *ngw = new NewGroupWindow(list);
    connect(ngw, SIGNAL(createNewGroup(QString,QList<quint16>)), this, SLOT(createNewGroup(QString,QList<quint16>)));

    ngw->show();
}

void Application::processPacket(VuzdarPacket packet)
{
    VuzdarPacket::PacketType type = packet.getType();
    quint8 controlCode = packet.getControlCode();

    if (type == VuzdarPacket::REGISTRATION) {
        if (controlCode == 0x00) {
            // uspjesno regitriran slijedi lista aktivnih korisnika
            addText("--== Successfully registered, waiting for client list... ==--");
            myNickname = ui->nicknameText->text();
            changeState(CONNECTED_UNAUTHENTICATED);
        } else if (controlCode == 0xF0){
            addText("--== Registration unsuccessful, nickname isn't unique ==--");
            on_disconnectButton_clicked();
        } else if (controlCode == 0xF1){
            addText("--== Registration unsuccessful, nickname too long ==--");
            on_disconnectButton_clicked();
        } else if (controlCode == 0xF2){
            addText("--== Registration unsuccessful, unallowed characters in nickname ==--");
            on_disconnectButton_clicked();
        } else if (controlCode == 0xF3){
            addText("--== Registration unsuccessful, nickname banned ==--");
            on_disconnectButton_clicked();
        } else if (controlCode >= 0xF4) {
            addText("--== Registration unsuccessful, reason unknown ==--");
            on_disconnectButton_clicked();
        } else {
            addText("--== Unknown reply to registration message, aborting ==--");
            on_disconnectButton_clicked();
        }
    } else if (type == VuzdarPacket::CLIENT_ACTIVITY) {
        if (controlCode == 0x01) {
            QList<QPair<quint16, QString> > list = packet.getAliveClientList();
            quint16 id;
            QString nickname;

            for (int i = 0; i < list.size(); ++i) {
                id = list[i].first;
                nickname = list[i].second;

                if (!clients.contains(id)) {
                    // novi klijent kojeg nemamo spremljenog
                    Conversation *c = new Conversation(true, id, nickname);
                    clients.insert(id, c);

                    addClientButton(c->getButton());

                    connect(c, SIGNAL(sendMessage(bool,quint16,QString)), this, SLOT(sendMessage(bool,quint16,QString)));
                    connect(c, SIGNAL(saveHtmlConversation(bool,quint16,QString)), this, SLOT(saveHtmlConversation(bool,quint16,QString)));

                    if (nickname == myNickname) {
                        // to je ovaj klijent u listi
                        myId = id;
                        Conversation::setMyId(myId);
                    }
                }
            }

            addText("--== Client list updated, new clients added ==--");
        } else if (controlCode == 0x00) {
            QList<quint16> list = packet.getDeadClientList();

            for (int i = 0; i < list.size(); ++i) {
                if (clients.contains(list[i])) {
                    // izbrisi klijenta
                    // znaci makni ga iz mape, delete-aj ga, i javi u grupne razgovore da je umro
                    delete clients[list[i]];
                    clients.remove(list[i]);
                }
            }
        } else {
            addText("--== Unknow client activity message, ignoring ==--");
        }
    } else if (type == VuzdarPacket::TEXT_PRIVATE_MESSAGE) {
        quint16 id = packet.getId();
        QString message = packet.getMessage();

        if (controlCode == 0x00 && !message.isEmpty()) {
            // normalna direktna poruka

            if (!clients.contains(id)) {
                addText("--== Recieved message from unknow client id: ==--");
                addText(QString("From (id): ").append(QString::number(id)));
                addText(QString("Message: ").append(message));
            } else {
                clients[id]->showClientMessage(clients[id]->getName(), message, Conversation::idToColor(id));
                newMessage.play();
            }
        } else if (message.isEmpty()) {
            // potvrda
            if (controlCode == 0x00) {
                clients[id]->signalMessageReply(true);
                newMessage.play();
            } else {
                clients[id]->signalMessageReply(false);

                if (controlCode == 0xF0)
                    clients[id]->showSystemMessage("Unable to send message, client doesn't exist.");
                else if (controlCode == 0xF1)
                    clients[id]->showSystemMessage("Unable to send message, unallowed characters in message.");
                else
                    clients[id]->showSystemMessage("Unable to send message, unknown reason.");
            }
        } else {
            addText("--== Recieved direct text message with unknown control code, ignoring ==--");
        }
    } else if (type == VuzdarPacket::TEXT_GROUP_MESSAGE) {
        if (controlCode == 0x01) {
            // grupa uspjesno stvorena
            addText("--== Group sucessfully created, waiting for new group packet... ==--");
        } else if (controlCode == 0xF0) {
            addText("--== Can't create group, name too long ==--");
        } else if (controlCode == 0xF1) {
            addText("--== Can't create group, unallowed characters in name ==--");
        } else if (controlCode == 0xF2) {
            addText("--== Can't create group, one or more clients don't exist ==--");
        } else if (controlCode == 0x10) {
            // dodan sam u grupu
            QList<quint16> list = packet.getGroupClientList();

            if (list.isEmpty()) {
                addText("--== Recieved new group message without any members, ignoring ==--");
                return;
            }

            Conversation *c = new Conversation(false, packet.getId(), packet.getGroupName());
            groups.insert(packet.getId(), c);

            connect(c, SIGNAL(sendMessage(bool,quint16,QString)), this, SLOT(sendMessage(bool,quint16,QString)));
            connect(c, SIGNAL(saveHtmlConversation(bool,quint16,QString)), this, SLOT(saveHtmlConversation(bool,quint16,QString)));
            connect(c, SIGNAL(addToGroup(quint16,QString)), this, SLOT(addToGroup(quint16,QString)));
            connect(c, SIGNAL(leaveGroup(quint16)), this, SLOT(leaveGroup(quint16)));

            addGroupButton(c->getButton());

            QString newGroupCreated;
            newGroupCreated += "You have been added to the group ";
            newGroupCreated += packet.getGroupName();
            newGroupCreated += " by ";
            newGroupCreated += clients[list.first()]->getName();
            newGroupCreated += ". Group members: ";

            for (int i = 0; i < list.size(); ++i) {
                newGroupCreated += clients[list[i]]->getName();

                if (i != list.size() - 1)
                    newGroupCreated += ", ";
            }

            newGroupCreated += ".";

            c->showSystemMessage(newGroupCreated);
        } else if (controlCode == 0x00) {
            // normalna poruka
            quint16 groupId = packet.getId();
            quint16 senderId = packet.getSecondId();
            QString message = packet.getMessage();

            if (senderId == myId)
                return; // necu sam sebi jos jednom prikazat poruku

            groups[groupId]->showClientMessage(
                        clients[senderId]->getName(), message, Conversation::idToColor(senderId));
            newMessage.play();
        } else if (controlCode == 0x02) {
            // poruka poslana
            groups[packet.getId()]->signalMessageReply(true);
            newMessage.play();
        } else if (controlCode == 0xF3) {
            groups[packet.getId()]->signalMessageReply(false);
            groups[packet.getId()]->showSystemMessage("Unable to send message, group doesn't exist.");
        } else if (controlCode == 0xF4) {
            groups[packet.getId()]->signalMessageReply(false);
            groups[packet.getId()]->showSystemMessage("Unable to send message, unallowed characters in message.");
        } else if (controlCode == 0x11) {
            groups[packet.getId()]->showSystemMessage(
                    QString(" left the group.").prepend(clients[packet.getSecondId()]->getName()));
        } else if (controlCode == 0x12) {
            groups[packet.getId()]->showSystemMessage(
                        QString(" joined the group.").prepend(clients[packet.getSecondId()]->getName()));
        } else if (controlCode == 0x03) {
            groups[packet.getId()]->deleteLater();
            groups.remove(packet.getId());
        }
    } else if (type == VuzdarPacket::ADMIN) {
        if (controlCode == 0x00) {
            addText("--== Authentication successful ==--");
            changeState(CONNECTED_AUTHENTICATED);
        } else if (controlCode == 0xF0) {
            addText("--== Deauthenticated - wrong password ==--");
        } else if (controlCode == 0x01) {
            addText("--== Deauthentication successful ==--");
            changeState(CONNECTED_UNAUTHENTICATED);
        } else if (controlCode == 0x20) {
            bannedNicknameList = packet.getBannedNicknameList();
            addText("--== Banned nicknames recieved ==--");
            addText("--== Opening admin window (2/2) - requesting statistics from server... ==--");

            connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                                      VuzdarPacket::ADMIN, 0x21));
        } else if (controlCode == 0x21) {
            QString stat = packet.getAdminString();
            addText("--== Statistics recieved, opening admin window... ==--");

            if (adminWindow)
                adminWindow->deleteLater();
            adminWindow = new AdminWindow(getClientList(), bannedNicknameList, stat);
            connect(adminWindow, SIGNAL(clientLogout(QString)), this, SLOT(kickNickname(QString)));
            connect(adminWindow, SIGNAL(clientBlocked(QString)), this, SLOT(banNickname(QString)));
            connect(adminWindow, SIGNAL(nicknameUnblocked(QString)), this, SLOT(unbanNickname(QString)));
            adminWindow->show();
            adminWindow->raise();
            adminWindow->activateWindow();
        } else if (controlCode == 0x10) {
            // uspjesan kick
            if (kickQueue.isEmpty())
                return;
            QString nickname = kickQueue.dequeue();
            adminWindow->removeClient(nickname);
        } else if (controlCode == 0x11) {
            // uspjesan ban
            if (banQueue.isEmpty())
                return;
            QString nickname = banQueue.dequeue();
            adminWindow->addToBlockedClients(nickname);
        } else if (controlCode == 0x12) {
            // uspjesan unban
            if (unbanQueue.isEmpty())
                return;
            QString nickname = unbanQueue.dequeue();
            adminWindow->removeBlockedNickname(nickname);
        }
    } else if (type == VuzdarPacket::PING) {
        connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                                  VuzdarPacket::PING, 0x00));
    } else if (type == VuzdarPacket::DISCONNECT) {
        if (controlCode == 0x00)
            addText("--== Disconnected from server, reason: server closing ==--");
        else if (controlCode == 0x10)
            addText("--== Disconnected from server, reason: kicked ==--");
        else if (controlCode == 0x11)
            addText("--== Disconnected from server, reason: banned ==--");
        else if (controlCode == 0x12)
            addText("--== Disconnected from server, reason: timeout (you didn't reply to ping) ==--");
        else
            addText("--== Disconnected from server, reason: unknown ==--");

        on_disconnectButton_clicked();
    } else if (type == VuzdarPacket::ERROR) {
        if (controlCode == 0x00)
            addText("--== Recieved error message from server: unexpected packet ==--");
        else if (controlCode == 0x01)
            addText("--== Recieved error message from server: malformed packet ==--");
        else if (controlCode == 0x02)
            addText("--== Recieved error message from server: packet unrecognized ==--");
        else
            addText("--== Recieved error message from server: unknown error ==--");
    } else {
        addText("--== Unrecognized packet type recieved, ignoring ==--");
    }
}

void Application::saveHtmlConversation(bool isClient, quint16 id, QString conversation)
{
    qDebug() << "saving html conversation kao";
    qDebug() << "jel client:" << isClient;
    qDebug() << "id:" << id;
    qDebug() << "html:" << conversation;

    QString name = isClient ? clients[id]->getName() : groups[id]->getName();
    name.append(".html");

    QFile file(name);

    if (!file.open(QIODevice::WriteOnly)) {
        addText(QString("Unable to open file ").append(name));
        return;
    }

    conversation.replace(QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"),
                         QString("<!DOCTYPE html>"));

    conversation.replace(QString("<head>"),
                         QString("<head><meta charset=\"utf-8\">"));

    conversation.replace(QString("10pt"),
                         QString("12pt"));

    QByteArray data = conversation.toUtf8();

    if (file.write(data) == data.size())
        addText(QString(name).append(" successfully saved."));
    else
        addText(QString(name).append(" unsuccessfully saved."));

    file.close();
}

void Application::sendMessage(bool isClient, quint16 id, QString message)
{
    if (isClient)
        connection.sendPacket(VuzdarPacket::generateTextPrivateMessagePacket(
                              0x00, id, message));
    else connection.sendPacket(VuzdarPacket::generateTextGroupMessagePacket(
                                   0x00, id, myId, message));
}

void Application::addToGroup(quint16 groupId, QString nickname)
{
    quint16 clientId = 0;

    QMap<quint16, Conversation*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); ++i) {
        if (i.value()->getName() == nickname) {
            clientId = i.value()->getId();
            break;
        }
    }

    if (clientId)
        connection.sendPacket(VuzdarPacket::generateGroupMemberChangePacket(
                              0x12, groupId, clientId));
}

void Application::leaveGroup(quint16 groupId)
{
    connection.sendPacket(VuzdarPacket::generateControlCodeIdPacket(
                              VuzdarPacket::TEXT_GROUP_MESSAGE, 0x11, groupId));
}

void Application::createNewGroup(QString name, QList<quint16> idList)
{
    idList.prepend(myId);

    connection.sendPacket(VuzdarPacket::generateNewGroupPacket(
                              0, name, idList));
}

void Application::kickNickname(QString nickname)
{
    connection.sendPacket(VuzdarPacket::generateAdminPacket(
                              0x10, nickname));
    kickQueue.enqueue(nickname);
}

void Application::banNickname(QString nickname)
{
    connection.sendPacket(VuzdarPacket::generateAdminPacket(
                              0x11, nickname));
    banQueue.enqueue(nickname);
}

void Application::unbanNickname(QString nickname)
{
    connection.sendPacket(VuzdarPacket::generateAdminPacket(
                              0x12, nickname));
    unbanQueue.enqueue(nickname);
}

void Application::on_authButton_clicked()
{
    addText("--== Authenticating... ==--");
    connection.sendPacket(VuzdarPacket::generateAdminPacket(
                              0x00, ui->passwordText->text()));
    ui->passwordText->setText("");
}

void Application::on_deauthButton_clicked()
{
    addText("--== Deauthenticating... ==--");
    connection.sendPacket(VuzdarPacket::generateAdminPacket(
                              0x01, ui->passwordText->text()));
}

void Application::on_openAdminButton_clicked()
{
    addText("--== Opening admin window (1/2) - requesting banned nicknames from server... ==--");
    connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                              VuzdarPacket::ADMIN, 0x20));
}
