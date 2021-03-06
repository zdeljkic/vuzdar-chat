#include "conversation.h"

QList<QString> Conversation::colors;
quint16 Conversation::myId = 0;

Conversation::Conversation(bool isClient, quint16 id, QString name):
    isClient(isClient), id(id), name(name),
    button(name),
    conversationWindow(QString(name).append(" - VuzdarChat"))
{
    connect(&button, SIGNAL(clicked()), &conversationWindow, SLOT(show()));

    connect(&conversationWindow, SIGNAL(saveHtmlConversation(QString)), this, SLOT(forwardSaveHtmlConversation(QString)));
    connect(&conversationWindow, SIGNAL(sendMessage(QString)), this, SLOT(forwardSendMessage(QString)));

    if (colors.isEmpty()) {
        colors.append("aqua");
        colors.append("fuchsia");
        colors.append("orange");
        colors.append("maroon");
        colors.append("navy");
        colors.append("olive");
        colors.append("blue");
        colors.append("purple");
        colors.append("red");
        colors.append("teal");
        colors.append("green");
    }
}

Conversation::~Conversation() {
    conversationWindow.deleteLater();
}

void Conversation::setId(quint16 id)
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
    if (!conversationWindow.isVisible())
        conversationWindow.show();
    //conversationWindow.raise();
    //conversationWindow.activateWindow();
    conversationWindow.showSystemMessage(message, color);
}

void Conversation::showClientMessage(QString nickname, QString message, QString color)
{
    if (!conversationWindow.isVisible())
        conversationWindow.show();
    //conversationWindow.raise();
    //conversationWindow.activateWindow();
    conversationWindow.showClientMessage(nickname, message, color);
}

void Conversation::signalMessageReply(bool successful)
{
    if (successful && !messageQueue.isEmpty()) {
        conversationWindow.showClientMessage("You", messageQueue.dequeue(), idToColor(myId));
    } else if (!messageQueue.isEmpty()) {
        messageQueue.dequeue();
    }
}

QString Conversation::idToColor(quint16 id)
{
    return colors[id % colors.size()];
}

void Conversation::setMyId(quint16 id)
{
    myId = id;
}

void Conversation::forwardSaveHtmlConversation(QString conversation)
{
    emit saveHtmlConversation(isClient, id, conversation);
}

void Conversation::forwardSendMessage(QString message)
{
    // !!TMP!!
    if (!isClient && message == QString("!leave")) {
        emit leaveGroup(id);
        return;
    } else if (!isClient && message.left(5) == QString("!add ")) {
        QString nickname = message.mid(5);
        emit addToGroup(id, nickname);
        return;
    }

    messageQueue.enqueue(message);
    emit sendMessage(isClient, id, message);
}

