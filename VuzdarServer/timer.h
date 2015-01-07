#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <QDate>
#include "admininfo.h"

class Server;

class Timer : public QObject
{
    Q_OBJECT

public:
    Timer(Server *server, AdminInfo *adminInfo);

    void startTimer();
    void stopTimer();

private:
    QTimer timer;
    Server *server;
    AdminInfo *adminInfo;
    QDate lastDate;

private slots:
    void update();
};

#endif // TIMER_H
