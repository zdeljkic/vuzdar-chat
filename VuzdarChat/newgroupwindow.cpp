#include "newgroupwindow.h"
#include "ui_newgroupwindow.h"
#include "conversation.h"

NewGroupWindow::NewGroupWindow(QList<Conversation*> clients, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewGroupWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    ui->scrollAreaWidgetContents->setLayout(new QVBoxLayout(ui->scrollAreaWidgetContents));
    QPushButton *button[clients.size()];
    ui->UsersScrollArea->setAlignment(Qt::AlignTop);

    for(int i = 0; i < clients.size(); ++i) {
        button[i]= new QPushButton(clients[i]->getName());
        mapOfUsers.insert(clients[i]->getId(),button[i]);
        button[i]->setCheckable(true);
        ui->scrollAreaWidgetContents->layout()->addWidget(button[i]);
    }
}


NewGroupWindow::~NewGroupWindow()
{
    delete ui;
}

void NewGroupWindow::on_newGroupButton_clicked()
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
    this->deleteLater();
}
