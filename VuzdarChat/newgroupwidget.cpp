#include "newgroupwidget.h"
#include "ui_newgroupwidget.h"
#include "client.h"

NewGroupWidget::NewGroupWidget(QList<Client*> clients, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewGroupWidget)
{
    ui->setupUi(this);
    QVBoxLayout* layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    QPushButton *button[clients.size()];
    ui->UsersScrollArea->setAlignment(Qt::AlignTop);
    for(int i =0; i<clients.size(); ++i) {
        button[i]= new QPushButton(clients[i]->getNickname());
        mapOfUsers.insert(clients[i]->getId(),button[i]);
        button[i]->setCheckable(true);
        ui->scrollAreaWidgetContents->layout()->addWidget(button[i]);
}
}


NewGroupWidget::~NewGroupWidget()
{
    delete ui;
}

void NewGroupWidget::on_newGroupButton_clicked()
{
    QList<quint16> idList;
    QMap<quint16, QPushButton*>::const_iterator i;
    for (i = mapOfUsers.begin(); i != mapOfUsers.end(); ++i) {
        if (i.value()->isChecked())
        {
            idList.append(i.key());
        }

    }
    emit createNewGroup(ui->groupName->text(),idList);
}
