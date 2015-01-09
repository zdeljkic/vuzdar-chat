#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QPushButton>
#include "conversationwindow.h"

class Client
{
public:
    Client(quint16 id, QString nickname);

    quint16 getId();
    QString getNickname();
    QPushButton *getButton();
    ConversationWindow *getConversationWindow();


private:
    quint16 id;
    QString nickname;
    QPushButton button;
    ConversationWindow conversationWindow;
};

#endif // CLIENT_H
