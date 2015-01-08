#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <QMap>
#include <QList>
#include "client.h"
#include "connection.h"
#include "newgroupwindow.h"
#include "conversationwindow.h"

namespace Ui {
class Application;
}

class Application : public QWidget
{
    Q_OBJECT

public:
    enum State : quint8 {UNCONNECTED = 0, CONNECTED_UNAUTHENTICATED = 1, CONNECTED_AUTHENTICATED = 2};

    explicit Application(QWidget *parent = 0);
    ~Application();

public slots:
    void addText(QString text);

private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_newGroupButton_clicked();

    void createNewGroup(QString name, QList<quint16> idList);

private:
    Ui::Application *ui;
    Connection connection;
    QList<Client*> clients;

    void setInputEnabled(State state);
};

#endif // APPLICATION_H
