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

            for (int i = 0; i < list.size(); ++i) {
                if (!clients.contains(list[i].first)) {
                    Client *c = new Client(list[i].first, list[i].second);
                    clients.insert(list[i], c);

                    if (list[i].second == ui->nicknameText->text()) {
                        // to je ovaj klijent u listi
                        myId = list[i].first;
                        myNickname = list[i].second;
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
    }
}

void Application::createNewGroup(QString name, QList<quint16> idList)
{
    qDebug() << "nova grupa:" << name << "id-evi:" << idList;
}
