#include "client.h"

Client::Client(quint16 id, QString nickname):
    button(nickname),
    conversationWindow(nickname.append(" - VuzdarChat"))
{
    this->id = id;
    this->nickname = nickname;

    QObject::connect(&button, SIGNAL(clicked()), &conversationWindow, SLOT(show()));
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

ConversationWindow *Client::getConversationWindow()
{
    return &conversationWindow;
}

