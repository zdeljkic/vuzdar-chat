#include "application.h"
#include "ui_application.h"

Application::Application(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Application)
{
    ui->setupUi(this);
    connect(&server, SIGNAL(newInfoText(QString)), this, SLOT(addText(QString)));

    // provjeri jel config file postoji i ovisno o tome checkaj
    // ili nemoj useOldConfig
    ui->useOldConfig->setChecked(server.configFileExists());
}

Application::~Application()
{
    disconnect(&server, SIGNAL(newInfoText(QString)), this, SLOT(addText(QString)));
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

    addText("--== Starting server... ==--");
    if (server.startServer(ui->passwordText->text(), ui->portText->text().toUShort(), ui->useOldConfig->isChecked())) {
        addText("--== Server successfully started ==--");
        setInputEnabled(false);

        /*
         * Password se sada teoretski jedino moze procitati iz memorije dok je server
         * pokrenut. Nakon brisanja iz passwordText-a ne bi smio ostati nigdje.
         */
        ui->passwordText->setText("");
    } else {
        addText("--== Can't start server ==--");
    }
}

void Application::setInputEnabled(bool enabled)
{
    ui->passwordText->setEnabled(enabled);
    ui->portText->setEnabled(enabled);
    ui->startButton->setEnabled(enabled);
    ui->useOldConfig->setEnabled(enabled);

    ui->stopButton->setEnabled(!enabled);
}

void Application::on_stopButton_clicked()
{
    addText("--== Stopping server... ==--");
    server.stopServer();
    addText("--== Server stopped ==--");
    setInputEnabled(true);

    // provjeri jel config file postoji i ovisno o tome checkaj
    // ili nemoj useOldConfig
    ui->useOldConfig->setChecked(server.configFileExists());
}
