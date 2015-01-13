#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QList>
#include <QPair>
#include <QQueue>
#include <QFile>
#include "conversation.h"
#include "connection.h"
#include "newgroupwindow.h"
#include "conversationwindow.h"
#include "adminwindow.h"
#include "../VuzdarCommon/vuzdarpacket.h"

namespace Ui {
class Application;
}

class Application : public QWidget
{
    Q_OBJECT

public:
    enum State : quint8 {UNCONNECTED = 0, REGISTERING = 1, CONNECTED_UNAUTHENTICATED = 2, CONNECTED_AUTHENTICATED = 3};

    explicit Application(QWidget *parent = 0);
    ~Application();

public slots:
    void addText(QString text);

private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_newGroupButton_clicked();
    void on_authButton_clicked();
    void on_deauthButton_clicked();
    void on_openAdminButton_clicked();

    void processPacket(VuzdarPacket packet);
    void saveHtmlConversation(bool isClient, quint16 id, QString conversation);
    void sendMessage(bool isClient, quint16 id, QString message);
    void createNewGroup(QString name, QList<quint16> idList);
    void kickNickname(QString nickname);
    void banNickname(QString nickname);
    void unbanNickname(QString nickname);

private:
    Ui::Application *ui;
    Connection connection;
    QMap<quint16, Conversation*> clients;
    QMap<quint16, Conversation*> groups;
    State state;
    quint16 myId;
    QString myNickname;
    AdminWindow *adminWindow;
    QList<QString> bannedNicknameList;
    QQueue<QString> kickQueue;
    QQueue<QString> banQueue;
    QQueue<QString> unbanQueue;

    void changeState(State state);
    QList<Conversation*> getClientList();
    void addClientButton(QPushButton *button);
    void addGroupButton(QPushButton *button);
};

#endif // APPLICATION_H
