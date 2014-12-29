#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <server.h>

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
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::Application *ui;
    Server server;
    void setInputEnabled(bool enabled);
};

#endif // APPLICATION_H
