#include "conversation.h"

Conversation::Conversation(bool isClient, quint16 id, QString name):
    button(name),
    conversationWindow(QString(name).append(" - VuzdarChat")),
    isClient(isClient), id(id), name(name)
{
    connect(&button, SIGNAL(clicked()), &conversationWindow, SLOT(show()));

    connect(&conversationWindow, SIGNAL(saveHtmlConversation(QString)), this, SLOT(forwardSaveHtmlConversation(QString)));
    connect(&conversationWindow, SIGNAL(sendMessage(QString)), this, SLOT(forwardSendMessage(QString)));
}

Conversation::~Conversation() {
    conversationWindow.deleteLater();
}

quint16 Conversation::setId(quint16 id)
{
    this->id = id;
}

quint16 Conversation::getId()
{
    return id;
}

QString Conversation::getName()
{
    return name;
}

QPushButton *Conversation::getButton()
{
    return &button;
}

void Conversation::showSystemMessage(QString message, QString color)
{
    conversationWindow.show();
    conversationWindow.raise();
    conversationWindow.activateWindow();
    conversationWindow.showSystemMessage(message, color);
}

void Conversation::showClientMessage(QString nickname, QString message, QString color)
{
    conversationWindow.show();
    conversationWindow.raise();
    conversationWindow.activateWindow();
    conversationWindow.showClientMessage(nickname, message, color);
}

void Conversation::signalMessageReply(bool successful)
{
    if (successful) {
        conversationWindow.showClientMessage("You", messageQueue.dequeue());
    } else {
        messageQueue.dequeue();
    }
}

void Conversation::forwardSaveHtmlConversation(QString conversation)
{
    emit saveHtmlConversation(isClient, id, conversation);
}

void Conversation::forwardSendMessage(QString message)
{
    messageQueue.enqueue(message);
    emit sendMessage(isClient, id, message);
}

