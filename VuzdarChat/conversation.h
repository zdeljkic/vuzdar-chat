#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QPushButton>
#include <QQueue>
#include "conversationwindow.h"

class Conversation : public QObject
{
    Q_OBJECT

public:
    Conversation(bool isClient, quint16 id, QString name);
    ~Conversation();

    quint16 setId(quint16 id);
    quint16 getId();
    QString getName();
    QPushButton *getButton();

    void showSystemMessage(QString message, QString color = QString("black"));
    void showClientMessage(QString nickname, QString message, QString color = QString("black"));
    void signalMessageReply(bool successful);

    static QString idToColor(quint16 id);
    static void setMyId(quint16 id);

private:
    bool isClient;
    quint16 id;
    QString name;
    QPushButton button;
    ConversationWindow conversationWindow;
    QQueue<QString> messageQueue;
    static QList<QString> colors;
    static quint16 myId;

private slots:
    void forwardSaveHtmlConversation(QString conversation);
    void forwardSendMessage(QString message);

signals:
    void saveHtmlConversation(bool isClient, quint16 id, QString conversation);
    void sendMessage(bool isClient, quint16 id, QString message);
};

#endif // CLIENT_H
