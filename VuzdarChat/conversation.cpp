#include "client.h"

Client::Client(quint16 id, QString nickname):
    button(nickname),
    conversationWindow(QString(nickname).append(" - VuzdarChat"))
{
    this->id = id;
    this->nickname = nickname;

    connect(&button, SIGNAL(clicked()), &conversationWindow, SLOT(show()));

    connect(&conversationWindow, SIGNAL(saveHtmlConversation(QString)), this, SLOT(forwardSaveHtmlConversation(QString)));
    connect(&conversationWindow, SIGNAL(sendMessage(QString)), this, SLOT(forwardSendMessage(QString)));
}

Client::~Client() {
    conversationWindow.deleteLater();
}

quint16 Client::getId()
{
    return id;
}

QString Client::getNickname()
{
    return nickname;
}

QPushButton *Client::getButton()
{
    return &button;
}

void Client::showSystemMessage(QString message, QString color)
{
    conversationWindow.show();
    conversationWindow.showSystemMessage(message, color);
}

void Client::showClientMessage(QString message, QString color)
{
    conversationWindow.show();
    conversationWindow.showClientMessage(nickname, message, color);
}

void Client::showMessageReply(bool successful)
{
    if (successful) {
        conversationWindow.showClientMessage("You", messageQueue.dequeue());
    } else {
        messageQueue.dequeue();
    }
}

void Client::forwardSaveHtmlConversation(QString conversation)
{
    emit saveHtmlConversation(id, conversation);
}

void Client::forwardSendMessage(QString message)
{
    messageQueue.enqueue(message);
    emit sendMessage(id, message);
}

