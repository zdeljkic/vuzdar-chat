#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <connection.h>

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

private:
    Ui::Application *ui;
    Connection connection;
    QMap<quint16, Client*> clients;

    void setInputEnabled(State state);
};

#endif // APPLICATION_H
