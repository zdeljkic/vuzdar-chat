#include "conversationwindow.h"
#include "ui_conversationwindow.h"

ConversationWindow::ConversationWindow(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConversationWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
}

ConversationWindow::~ConversationWindow()
{
    delete ui;
}

void ConversationWindow::showSystemMessage(QString message, QString color)
{
    ui->conversationText->insertHtml(QString("<font color=" + color + ">" + message + "</font><br />"));
}

void ConversationWindow::showClientMessage(QString name, QString message, QString color)
{
    ui->conversationText->insertHtml(QString("<font color=" + color + ">" + name + ": " + "</font>" + message + "\n"));
}



void ConversationWindow::on_saveConversationButton_clicked()
{
    emit(saveHtmlConversation(ui->conversationText->toHtml()));
}

void ConversationWindow::on_sendButton_clicked()
{
    emit(sendMessage(ui->messageText->toPlainText()));
}
