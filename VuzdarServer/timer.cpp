#include "timer.h"
#include "server.h"

Timer::Timer(Server *server, AdminInfo *adminInfo)
{
    this->server = server;
    this->adminInfo = adminInfo;

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

void Timer::startTimer()
{
    lastDate = QDate::currentDate();
    timer.start(60 * 1000);
}

void Timer::stopTimer()
{
    timer.stop();
}

void Timer::update()
{
    server->pingClients();
    adminInfo->calculateAverageClients();

    // provjeri jel se promijenio dan/tjedan/mjesec/godine pomogu closingDate i pozovi odgovarajuce reset funkcije
    QDate currentDate = QDate::currentDate();

    if (currentDate.year() > lastDate.year()) {
        adminInfo->resetYear();
    } else if (currentDate.month() > lastDate.month()) {
        adminInfo->resetMonth();
    } else if (currentDate.weekNumber() > lastDate.weekNumber()) {
        adminInfo->resetWeek();
    } else if (currentDate > lastDate) {
        adminInfo->resetDay();
    }
}

