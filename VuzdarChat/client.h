#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QPushButton>
#include <QQueue>
#include "conversationwindow.h"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(quint16 id, QString nickname);
    ~Client();

    quint16 getId();
    QString getNickname();
    QPushButton *getButton();

    void showSystemMessage(QString message, QString color = QString("black"));
    void showClientMessage(QString message, QString color = QString("black"));
    void showMessageReply(bool successful);

private:
    quint16 id;
    QString nickname;
    QPushButton button;
    ConversationWindow conversationWindow;
    QQueue<QString> messageQueue;

private slots:
    void forwardSaveHtmlConversation(QString conversation);
    void forwardSendMessage(QString message);

signals:
    void saveHtmlConversation(quint16 id, QString conversation);
    void sendMessage(quint16 id, QString message);
};

#endif // CLIENT_H
