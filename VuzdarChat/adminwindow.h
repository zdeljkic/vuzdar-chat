#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include <conversation.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>
#include <qmap.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <QSpacerItem>


namespace Ui {
class AdminWindow;
}

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(QList<Conversation*> clientList,QList<QString> blockedNicknameList,QString statisticsString,QWidget *parent = 0);
    ~AdminWindow();



private:
    Ui::AdminWindow *ui;
    QMap<QString,QWidget*> mapOfOnlineUsers;
    QMap<QString,QWidget*> mapOfBlockedUsers;
    QSignalMapper *logoutSignalMapper , *blockSignalMapper, *unblockSignalMapper;

private:
    void updateBlockedUsers(QString nickname);
    void updateOnlineUsers(QString nickname);



signals:
    void clientLogout(QString nickname);
    void clientBlocked(QString nickname);
    void nicknameUnblocked(QString nickname);

public slots:
    void removeClient(QString nickname);
    void removeBlockedNickname(QString nickname);
    void addToBlockedClients(QString nickname);

};

#endif // ADMINWINDOW_H
