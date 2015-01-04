#include "application.h"
#include "ui_application.h"

Application::Application(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Application)
{
    ui->setupUi(this);
    connect(&server, SIGNAL(newInfoText(QString)), this, SLOT(addText(QString)));
}

Application::~Application()
{
    delete ui;
}

void Application::addText(QString text)
{
    ui->displayText->appendPlainText(text);
}

void Application::on_startButton_clicked()
{
    if (ui->passwordText->text().length() > 100) {
        addText("--== Didn't start server, password too long ==--");
        return;
    }

    bool ok;
    ui->portText->text().toUShort(&ok);

    if (!ok || ui->portText->text().toUShort() > 65535) {
        addText("--== Didn't start server, port invalid ==--");
        return;
    }

    if (server.startServer(ui->passwordText->text(), ui->portText->text().toUShort())) {
        addText("--== Server successfully started ==--");
        setInputEnabled(false);
    } else {
        addText("--== Can't start server ==--");
    }
}

void Application::setInputEnabled(bool enabled)
{
    ui->passwordText->setEnabled(enabled);
    ui->portText->setEnabled(enabled);
    ui->startButton->setEnabled(enabled);
    ui->stopButton->setEnabled(!enabled);
}

void Application::on_stopButton_clicked()
{
    server.stopServer();
    addText("--== Server stopped ==--");
    setInputEnabled(true);
}
