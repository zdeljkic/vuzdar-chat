#ifndef NEWGROUPWIDGET_H
#define NEWGROUPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QMap>
#include <client.h>

namespace Ui {
class NewGroupWidget;
}

class NewGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewGroupWidget(QList<Client *> clients, QWidget *parent = 0);
    ~NewGroupWidget();


private:
    QMap<quint16,QPushButton*> mapOfUsers;
    Ui::NewGroupWidget *ui;
signals:
    void createNewGroup(QString groupName,QList<quint16> idList);

private slots:
    void on_newGroupButton_clicked();
};

#endif // NEWGROUPWIDGET_H
