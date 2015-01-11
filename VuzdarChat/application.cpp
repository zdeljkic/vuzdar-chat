#include "application.h"
#include "ui_application.h"

Application::Application(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Application)
{
    ui->setupUi(this);
    state = UNCONNECTED;

    connect(&connection, SIGNAL(newPacket(VuzdarPacket)), this, SLOT(processPacket(VuzdarPacket)));
}

Application::~Application()
{
    delete ui;

    on_disconnectButton_clicked();

    QMap<quint16, Client*>::const_iterator i;

    for (i = clients.constBegin(); i != clients.constEnd(); i++) {
        delete i.value();
    }
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
    connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                              VuzdarPacket::DISCONNECT, 0x00));
    connection.disconnectFromServer();

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
    } else if (state == CONNECTED_UNAUTHENTICATED) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->nicknameText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(true);
        ui->openAdminButton->setEnabled(true);
    }
}

QList<Client *> Application::getClientList()
{
    QList<Client*> list;
    QMap<quint16, Client*>::const_iterator i;

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

void Application::on_newGroupButton_clicked()
{
    NewGroupWindow *ngw = new NewGroupWindow(getClientList());
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
        } else if (controlCode >= 0xF4 && controlCode <= 0xFF) {
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
                    Client *c = new Client(id, nickname);
                    clients.insert(id, c);

                    addClientButton(c->getButton());

                    connect(c, SIGNAL(sendMessage(quint16,QString)), this, SLOT(sendMessage(quint16,QString)));
                    connect(c, SIGNAL(saveHtmlConversation(quint16,QString)), this, SLOT(saveHtmlConversation(quint16,QString)));

                    if (nickname == myNickname) {
                        // to je ovaj klijent u listi
                        myId = id;
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
                clients[id]->showClientMessage(message);
            }
        } else if (message.isEmpty()) {
            // potvrda
            if (controlCode == 0x00) {
                clients[id]->showMessageReply(true);
            } else {
                clients[id]->showMessageReply(false);

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
    } else if (type == VuzdarPacket::PING) {
        connection.sendPacket(VuzdarPacket::generateControlCodePacket(
                                  VuzdarPacket::PING, 0x00));
    }
}

void Application::saveHtmlConversation(quint16 id, QString conversation)
{
    qDebug() << "saving html conversation kao";
    qDebug() << "id:" << id;
    qDebug() << "html:" << conversation;
}

void Application::sendMessage(quint16 id, QString message)
{
    connection.sendPacket(VuzdarPacket::generateTextPrivateMessagePacket(
                              0x00, id, message));
}

void Application::createNewGroup(QString name, QList<quint16> idList)
{
    qDebug() << "nova grupa:" << name << "id-evi:" << idList;
}
