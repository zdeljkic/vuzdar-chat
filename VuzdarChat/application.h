#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QList>
#include <QPair>
#include <QCloseEvent>
#include "client.h"
#include "connection.h"
#include "newgroupwindow.h"
#include "conversationwindow.h"
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

    void processPacket(VuzdarPacket packet);
    void saveHtmlConversation(quint16 id, QString conversation);
    void sendMessage(quint16 id, QString message);
    void createNewGroup(QString name, QList<quint16> idList);

private:
    Ui::Application *ui;
    Connection connection;
    QMap<quint16, Client*> clients;
    State state;
    quint16 myId;
    QString myNickname;

    void changeState(State state);
    QList<Client*> getClientList();
    void addClientButton(QPushButton *button);
};

#endif // APPLICATION_H
