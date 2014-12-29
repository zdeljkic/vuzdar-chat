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
    explicit Application(QWidget *parent = 0);
    ~Application();

public slots:
    void addText(QString text);

private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_sendButton_clicked();

private:
    Ui::Application *ui;
    Connection connection;
    void setInputEnabled(bool enabled);
};

#endif // APPLICATION_H
