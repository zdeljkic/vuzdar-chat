#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QList<Conversation*> clientList,QList<QString> blockedNicknameList,QString statisticsString,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminWindow)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    ui->setupUi(this);
    ui->onlineScrollAreaWidgetContents->setLayout(new QVBoxLayout(ui->onlineScrollAreaWidgetContents));
    ui->blockedScrollAreaWidgetContents->setLayout(new QVBoxLayout(ui->blockedScrollAreaWidgetContents));
    ui->onlineUsersScrollArea->setAlignment(Qt::AlignTop);
    ui->blockedUsersScrollArea->setAlignment(Qt::AlignTop);
    logoutSignalMapper = new QSignalMapper(this);
    blockSignalMapper = new QSignalMapper(this);
    unblockSignalMapper =new QSignalMapper(this);
    ui->statisticsText->setReadOnly(true);
    ui->statisticsText->setPlainText(statisticsString);
    for(int i =0; i<clientList.size(); ++i){
        updateOnlineUsers(clientList[i]->getName());
    }
    for(int i =0; i<blockedNicknameList.size(); ++i){
         updateBlockedUsers(blockedNicknameList[i]);
    }
    connect(unblockSignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(nicknameUnblocked(QString)));
    connect(unblockSignalMapper, SIGNAL(mapped(QString)), this, SLOT(removeBlockedNickname(QString)));
    connect(logoutSignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(clientLogout(QString)));
    connect(blockSignalMapper, SIGNAL(mapped(QString)), this, SIGNAL(clientBlocked(QString)));
    connect(logoutSignalMapper, SIGNAL(mapped(QString)), this, SLOT(removeClient(QString)));
    connect(blockSignalMapper, SIGNAL(mapped(QString)), this, SLOT(addToBlockedClients(QString)));
}





AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::updateBlockedUsers(QString nickname)
{
    QLabel *nicknameLabel=new QLabel(nickname);
    QPushButton *unblockButton=new QPushButton("unblock");
    QHBoxLayout *blockedUsersHorizontalLayout = new QHBoxLayout();
    blockedUsersHorizontalLayout->addWidget(nicknameLabel);
    blockedUsersHorizontalLayout->addWidget(unblockButton);
    QWidget* row = new QWidget;
    row->setLayout(blockedUsersHorizontalLayout);
    mapOfBlockedUsers.insert(nickname,row);
    ui->blockedScrollAreaWidgetContents->layout()->addWidget(row);
    unblockSignalMapper->setMapping(unblockButton, nickname);
    connect(unblockButton, SIGNAL(clicked()), unblockSignalMapper, SLOT(map()));
}

void AdminWindow::updateOnlineUsers(QString nickname)
{
    QLabel *nicknameLabel=new QLabel(nickname);
    QPushButton *logoutButton=new QPushButton("logout");
    QPushButton *blockButton=new QPushButton("block");
    QHBoxLayout *onlineUsersHorizontalLayout = new QHBoxLayout();
    onlineUsersHorizontalLayout->addWidget(nicknameLabel);
    onlineUsersHorizontalLayout->addWidget(logoutButton);
    onlineUsersHorizontalLayout->addWidget(blockButton);
    QWidget* row = new QWidget;
    row->setLayout(onlineUsersHorizontalLayout);
    mapOfOnlineUsers.insert(nickname,row);
    ui->onlineScrollAreaWidgetContents->layout()->addWidget(row);
    logoutSignalMapper->setMapping(logoutButton,nickname);
    connect(logoutButton, SIGNAL(clicked()), logoutSignalMapper, SLOT(map()));
    blockSignalMapper->setMapping(blockButton, nickname);
    connect(blockButton, SIGNAL(clicked()),blockSignalMapper,SLOT(map()));

}

void AdminWindow::removeClient(QString nickname)
{
    QWidget* row=mapOfOnlineUsers[nickname];
    row->deleteLater();
}

void AdminWindow::addToBlockedClients(QString nickname)
{
    QWidget* row=mapOfOnlineUsers[nickname];
    updateBlockedUsers(nickname);
    row->deleteLater();
}



void AdminWindow::removeBlockedNickname(QString nickname)
{
    QWidget* row=mapOfBlockedUsers[nickname];
    updateOnlineUsers(nickname);
    row->deleteLater();
}





