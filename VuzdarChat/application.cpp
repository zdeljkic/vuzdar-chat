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

void Application::on_sendButton_clicked()
{
    connection.sendData(ui->messageText->toPlainText().toUtf8());
    ui->messageText->setPlainText("");
    addText("-= Sent message =-");
}
