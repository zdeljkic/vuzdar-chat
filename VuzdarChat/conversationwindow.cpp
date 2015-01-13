#include "conversationwindow.h"
#include "ui_conversationwindow.h"

ConversationWindow::ConversationWindow(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConversationWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setWindowTitle(title);
}

ConversationWindow::~ConversationWindow()
{
    delete ui;
}

void ConversationWindow::showSystemMessage(QString message, QString color)
{
    ui->conversationText->append(QString("<font color=" + color + ">" + message.toHtmlEscaped() + "</font>"));
}

void ConversationWindow::showClientMessage(QString name, QString message, QString color)
{
    ui->conversationText->append(QString("<font color=" + color + ">" + name + ": " + "</font>" + message.toHtmlEscaped()));
}

void ConversationWindow::on_saveConversationButton_clicked()
{
    emit saveHtmlConversation(ui->conversationText->toHtml());
}

void ConversationWindow::on_sendButton_clicked()
{
    emit sendMessage(ui->messageText->toPlainText());
    ui->messageText->setHtml("");
}
