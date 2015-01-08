#ifndef NEWGROUPWINDOW_H
#define NEWGROUPWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QMap>
#include <client.h>

namespace Ui {
class NewGroupWindow;
}

class NewGroupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NewGroupWindow(QList<Client *> clients, QWidget *parent = 0);
    ~NewGroupWindow();


private:
    QMap<quint16,QPushButton*> mapOfUsers;
    Ui::NewGroupWindow *ui;

signals:
    void createNewGroup(QString groupName,QList<quint16> idList);

private slots:
    void on_newGroupButton_clicked();
};

#endif // NEWGROUPWINDOW_H
