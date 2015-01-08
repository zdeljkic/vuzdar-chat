#include "application.h"
#include "ui_application.h"

Application::Application(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Application)
{
    ui->setupUi(this);
    connect(&connection, SIGNAL(newInfoText(QString)), this, SLOT(addText(QString)));
}

Application::~Application()
{
    delete ui;
}

void Application::addText(QString text)
{
    ui->displayText->appendPlainText(text);
}

void Application::on_connectButton_clicked()
{
    bool ok;
    ui->portText->text().toUShort(&ok);

    if (!ok || ui->portText->text().toUShort() > 65535) {
        addText("--== Didn't try to connect, port invalid ==--");
        return;
    }

    if (connection.connectToServer(ui->hostnameText->text(), ui->portText->text().toUShort())) {
        addText("--== Successfully connected to the server ==--");
        // input promijeni, da nemres opet kliknut connect i da mozes disconnect
    } else {
        addText("--== Connection unsuccessful ==--");
        // obrnuto od ovog gore
    }
}

void Application::on_disconnectButton_clicked()
{
    connection.disconnectFromServer();
    addText("--== Disconnected from the server ==--");
}

void Application::setInputEnabled(Application::State state)
{
    if (state == UNCONNECTED) {
        ui->hostnameText->setEnabled(true);
        ui->portText->setEnabled(true);
        ui->connectButton->setEnabled(true);

        ui->disconnectButton->setEnabled(false);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(false);
        ui->openAdminButton->setEnabled(false);
    } else if (state == CONNECTED_UNAUTHENTICATED) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(true);
        ui->authButton->setEnabled(true);
        ui->deauthButton->setEnabled(false);
        ui->openAdminButton->setEnabled(false);
    } else if (state == CONNECTED_UNAUTHENTICATED) {
        ui->hostnameText->setEnabled(false);
        ui->portText->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->disconnectButton->setEnabled(true);
        ui->passwordText->setEnabled(false);
        ui->authButton->setEnabled(false);
        ui->deauthButton->setEnabled(true);
        ui->openAdminButton->setEnabled(true);
    }
}

void Application::on_newGroupButton_clicked()
{
    // !!DEBUG!!
    Client *c = new Client(1, "antun");
    clients.append(c);
    c = new Client(2, "borna");
    clients.append(c);
    c = new Client(3, "žad");
    clients.append(c);

    NewGroupWindow *ngw = new NewGroupWindow(clients);
    connect(ngw, SIGNAL(createNewGroup(QString,QList<quint16>)), this, SLOT(createNewGroup(QString,QList<quint16>)));

    ngw->show();
}

void Application::createNewGroup(QString name, QList<quint16> idList)
{
    qDebug() << "nova grupa:" << name << "id-evi:" << idList;
}
