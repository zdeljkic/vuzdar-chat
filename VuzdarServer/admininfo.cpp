#include "admininfo.h"
#include <QCryptographicHash>
#include <QDebug>

AdminInfo::AdminInfo(QString password)
{
    qsrand(QDateTime::currentDateTime().toTime_t());

    for (int i = 0; i < 32; ++i) {
        // dodaj 32 random byte-a u salt
        // to se radi tako da se dobije random integer, pretvori u raspon od
        // -128 do 127, casta u signed char i doda na salt
        salt.append((char) ((qrand() % 256) - 128));
    }

    QByteArray dataForHashing;
    dataForHashing.append(salt);
    dataForHashing.append(password.toUtf8());

    hash = QCryptographicHash::hash(dataForHashing, QCryptographicHash::Sha256);

    bannedNicknames = QList<QString>();
    totalTime = 0;
    numClients = 0;
    timeDay = timeWeek = timeMonth = timeYear = 0;
    minClientsDay = minClientsWeek = minClientsMonth = minClientsYear = 0;
    maxClientsDay = maxClientsWeek = maxClientsMonth = maxClientsYear = 0;
    avgClientsDay = avgClientsWeek = avgClientsMonth = avgClientsYear = 0;
    totalMessagesDay = totalMessagesWeek = totalMessagesMonth = totalMessagesYear = 0;
    totalClientsLogoutDay = totalClientsLogoutWeek = totalClientsLogoutMonth = totalClientsLogoutYear = 0;
    totalClientsTimeoutDay = totalClientsTimeoutWeek = totalClientsTimeoutMonth = totalClientsTimeoutYear = 0;

    qDebug() << "-----";
    qDebug() << "Salt:" << salt.toHex();
    qDebug() << "Hash:" << hash.toHex();

}

AdminInfo::AdminInfo(QByteArray salt, QByteArray hash, QList<QString> bannedNicknames, quint64 totalTime, QDate closingDate, quint32 timeDay, quint32 timeWeek, quint32 timeMonth, quint32 timeYear, quint16 minClientsDay, quint16 minClientsWeek, quint16 minClientsMonth, quint16 minClientsYear, quint16 maxClientsDay, quint16 maxClientsWeek, quint16 maxClientsMonth, quint16 maxClientsYear, quint16 avgClientsDay, quint16 avgClientsWeek, quint16 avgClientsMonth, quint16 avgClientsYear, quint32 totalMessagesDay, quint32 totalMessagesWeek, quint32 totalMessagesMonth, quint32 totalMessagesYear, quint32 totalClientsLogoutDay, quint32 totalClientsLogoutWeek, quint32 totalClientsLogoutMonth, quint32 totalClientsLogoutYear, quint32 totalClientsTimeoutDay, quint32 totalClientsTimeoutWeek, quint32 totalClientsTimeoutMonth, quint32 totalClientsTimeoutYear)
{
    this->salt = salt;
    this->hash = hash;

    this->bannedNicknames = bannedNicknames;

    this->totalTime = totalTime;

    this->numClients = 0;

    this->timeDay = timeDay;
    this->timeWeek = timeWeek;
    this->timeMonth = timeMonth;
    this->timeYear = timeYear;

    this->minClientsDay = minClientsDay;
    this->minClientsWeek = minClientsWeek;
    this->minClientsMonth = minClientsMonth;
    this->minClientsYear = minClientsYear;

    this->maxClientsDay = maxClientsDay;
    this->maxClientsWeek = maxClientsWeek;
    this->maxClientsMonth = maxClientsMonth;
    this->maxClientsYear = maxClientsYear;

    this->avgClientsDay = avgClientsDay;
    this->avgClientsWeek = avgClientsWeek;
    this->avgClientsMonth = avgClientsMonth;
    this->avgClientsYear = avgClientsYear;

    this->totalMessagesDay = totalMessagesDay;
    this->totalMessagesWeek = totalMessagesWeek;
    this->totalMessagesMonth = totalMessagesMonth;
    this->totalMessagesYear = totalMessagesYear;

    this->totalClientsLogoutDay = totalClientsLogoutDay;
    this->totalClientsLogoutWeek = totalClientsLogoutWeek;
    this->totalClientsLogoutMonth = totalClientsLogoutMonth;
    this->totalClientsLogoutYear = totalClientsLogoutYear;

    this->totalClientsTimeoutDay = totalClientsTimeoutDay;
    this->totalClientsTimeoutWeek = totalClientsTimeoutWeek;
    this->totalClientsTimeoutMonth = totalClientsTimeoutMonth;
    this->totalClientsTimeoutYear = totalClientsTimeoutYear;

    // provjeri jel se promijenio dan/tjedan/mjesec/godine pomogu closingDate i pozovi odgovarajuce reset funkcije
    QDate currentDate = QDate::currentDate();

    if (currentDate.year() > closingDate.year()) {
        resetYear();
    } else if (currentDate.month() > closingDate.month()) {
        resetMonth();
    } else if (currentDate.weekNumber() > closingDate.weekNumber()) {
        resetWeek();
    } else if (currentDate > closingDate) {
        resetDay();
    }
}

void AdminInfo::prepareForSaving()
{
    closingDate = QDate::currentDate();
}

bool AdminInfo::validatePassword(QString password)
{
    QByteArray dataForHashing;
    dataForHashing.append(salt);
    dataForHashing.append(password.toUtf8());

    if (hash == QCryptographicHash::hash(dataForHashing, QCryptographicHash::Sha256))
        return true;
    else
        return false;
}

bool AdminInfo::checkBannedNickname(QString nickname)
{
    return bannedNicknames.contains(nickname);
}

void AdminInfo::addBannedNickname(QString nickname)
{
    if (!bannedNicknames.contains(nickname))
        bannedNicknames.append(nickname);
}

void AdminInfo::removeBannedNickname(QString nickname)
{
    bannedNicknames.removeAll(nickname);
}

void AdminInfo::startTime()
{
    lastStartingTime = QDateTime::currentMSecsSinceEpoch();
}

void AdminInfo::stopTime()
{
    totalTime += QDateTime::currentMSecsSinceEpoch() - lastStartingTime;
}

void AdminInfo::increaseClients()
{
    ++numClients;

    if (numClients > maxClientsDay)
        maxClientsDay = numClients;

    if (numClients > maxClientsWeek)
        maxClientsWeek = numClients;

    if (numClients > maxClientsMonth)
        maxClientsMonth = numClients;

    if (numClients > maxClientsYear)
        maxClientsYear = numClients;
}

void AdminInfo::decreaseClients(bool timeout)
{
    --numClients;

    if (numClients < minClientsDay)
        minClientsDay = numClients;

    if (numClients < minClientsWeek)
        minClientsWeek = numClients;

    if (numClients < minClientsMonth)
        minClientsMonth = numClients;

    if (numClients < minClientsYear)
        minClientsYear = numClients;

    if (timeout == true) {
        ++totalClientsTimeoutDay;
        ++totalClientsTimeoutWeek;
        ++totalClientsTimeoutMonth;
        ++totalClientsTimeoutYear;
    } else {
        ++totalClientsLogoutDay;
        ++totalClientsLogoutWeek;
        ++totalClientsLogoutMonth;
        ++totalClientsLogoutYear;
    }
}

void AdminInfo::calculateAverageClients()
{
    avgClientsDay = (timeDay * avgClientsDay + numClients) / (timeDay + 1);
    avgClientsWeek = (timeWeek * avgClientsWeek + numClients) / (timeWeek + 1);
    avgClientsMonth = (timeMonth * avgClientsMonth + numClients) / (timeMonth + 1);
    avgClientsYear = (timeYear * avgClientsYear + numClients) / (timeYear + 1);

    ++timeDay;
    ++timeWeek;
    ++timeMonth;
    ++timeYear;
}

void AdminInfo::increaseMessages()
{
    ++totalMessagesDay;
    ++totalMessagesWeek;
    ++totalMessagesMonth;
    ++totalMessagesYear;
}

void AdminInfo::resetDay()
{
    timeDay = 0;

    minClientsDay = numClients;
    maxClientsDay = numClients;
    avgClientsDay = numClients;

    totalMessagesDay = 0;
    totalClientsLogoutDay = 0;
    totalClientsTimeoutDay = 0;
}

void AdminInfo::resetWeek()
{
    timeWeek = 0;

    minClientsWeek = numClients;
    maxClientsWeek = numClients;
    avgClientsWeek = numClients;

    totalMessagesWeek = 0;
    totalClientsLogoutWeek = 0;
    totalClientsTimeoutWeek = 0;

    resetDay();
}

void AdminInfo::resetMonth()
{
    timeMonth = 0;

    minClientsMonth = numClients;
    maxClientsMonth = numClients;
    avgClientsMonth = numClients;

    totalMessagesMonth = 0;
    totalClientsLogoutMonth = 0;
    totalClientsTimeoutMonth = 0;

    resetWeek();
}

void AdminInfo::resetYear()
{
    timeYear = 0;

    minClientsYear = numClients;
    maxClientsYear = numClients;
    avgClientsYear = numClients;

    totalMessagesYear = 0;
    totalClientsLogoutYear = 0;
    totalClientsTimeoutYear = 0;

    resetMonth();
}

QList<QString> AdminInfo::getBannedNicknames()
{
    return bannedNicknames;
}

QString AdminInfo::getStatisticsString()
{
    QString statisticsString;

    // total hours:minutes:seconds
    int s = (totalTime / 1000) % 60;
    int m = (totalTime / (1000 * 60)) % 60;
    int h = totalTime / (1000 * 60 * 60);

    quint64 currentTime = QDateTime::currentMSecsSinceEpoch() - lastStartingTime;
    int ls = (currentTime / 1000) % 60;
    int lm = (currentTime / (1000 * 60)) % 60;
    int lh = currentTime / (1000 * 60 * 60);

    double pd = (double) totalClientsTimeoutDay / (totalClientsTimeoutDay + totalClientsLogoutDay);
    double pw = (double) totalClientsTimeoutWeek / (totalClientsTimeoutWeek + totalClientsLogoutWeek);
    double pm = (double) totalClientsTimeoutMonth / (totalClientsTimeoutMonth + totalClientsLogoutMonth);
    double py = (double) totalClientsTimeoutYear / (totalClientsTimeoutYear + totalClientsLogoutYear);

    statisticsString += QString("Uptime (total): ") += QString::number(h) += QString(" hours, ")
            += QString::number(m) += QString(" minutes and ") += QString::number(s) += QString(" seconds\n");

    statisticsString += QString("Uptime (since last start) ") += QString::number(lh) += QString(" hours, ")
            += QString::number(lm) += QString(" minutes and ") += QString::number(ls) += QString(" seconds\n");

    statisticsString += QString("Min/max/avg clients today: ") += QString::number(minClientsDay) += QString(" / ")
            += QString::number(maxClientsDay) += QString(" / ") += QString::number(avgClientsDay) += QString("\n");

    statisticsString += QString("Min/max/avg clients this week: ") += QString::number(minClientsWeek) += QString(" / ")
            += QString::number(maxClientsWeek) += QString(" / ") += QString::number(avgClientsWeek) += QString("\n");

    statisticsString += QString("Min/max/avg clients this month: ") += QString::number(minClientsMonth) += QString(" / ")
            += QString::number(maxClientsMonth) += QString(" / ") += QString::number(avgClientsMonth) += QString("\n");

    statisticsString += QString("Min/max/avg clients this year: ") += QString::number(minClientsYear) += QString(" / ")
            += QString::number(maxClientsYear) += QString(" / ") += QString::number(avgClientsYear) += QString("\n");

    statisticsString += QString("Total messages today/this week/this month/this year: ") += QString::number(totalMessagesDay) += QString(" / ")
            += QString::number(totalMessagesWeek) += QString(" / ") += QString::number(totalMessagesMonth) += QString(" / ") += QString::number(totalMessagesYear) += QString("\n");

    statisticsString += QString("Percentage of timed out clients today/this week/this month/this year: ")
            += QString::number(pd, 'f', 1) += QString("% / ") += QString::number(pw, 'f', 1) += QString("% / ")
            += QString::number(pm, 'f', 1) += QString("% / ") += QString::number(py, 'f', 1) += QString("%\n");

    return statisticsString;
}

QDataStream &operator<<(QDataStream &out, AdminInfo &adminInfo)
{
    adminInfo.prepareForSaving();

    // !!VAZNO!! promijenit ovdje verziju ako treba
    out << QString("VuzdarChat1.0-AdminInfo\n").toUtf8()
        << adminInfo.salt << adminInfo.hash
        << adminInfo.bannedNicknames
        << adminInfo.totalTime
        << adminInfo.closingDate
        << adminInfo.timeDay << adminInfo.timeWeek << adminInfo.timeMonth << adminInfo.timeYear
        << adminInfo.minClientsDay << adminInfo.minClientsWeek << adminInfo.minClientsMonth << adminInfo.minClientsYear
        << adminInfo.maxClientsDay << adminInfo.maxClientsWeek << adminInfo.maxClientsMonth << adminInfo.maxClientsYear
        << adminInfo.avgClientsDay << adminInfo.avgClientsWeek << adminInfo.avgClientsMonth << adminInfo.avgClientsYear
        << adminInfo.totalMessagesDay << adminInfo.totalMessagesWeek << adminInfo.totalMessagesMonth << adminInfo.totalMessagesYear
        << adminInfo.totalClientsLogoutDay << adminInfo.totalClientsLogoutWeek << adminInfo.totalClientsLogoutMonth << adminInfo.totalClientsLogoutYear
        << adminInfo.totalClientsTimeoutDay << adminInfo.totalClientsTimeoutWeek << adminInfo.totalClientsTimeoutMonth << adminInfo.totalClientsTimeoutYear;

    return out;
}


QDataStream &operator>>(QDataStream &in, AdminInfo &adminInfo)
{
    QByteArray versionStringUtf8;
    QByteArray salt, hash;
    QList<QString> bannedNicknames;
    quint64 totalTime;
    QDate closingDate;
    quint32 timeDay, timeWeek, timeMonth, timeYear,
            totalMessagesDay, totalMessagesWeek, totalMessagesMonth, totalMessagesYear,
            totalClientsLogoutDay, totalClientsLogoutWeek, totalClientsLogoutMonth, totalClientsLogoutYear,
            totalClientsTimeoutDay, totalClientsTimeoutWeek, totalClientsTimeoutMonth, totalClientsTimeoutYear;
    quint16 minClientsDay, minClientsWeek, minClientsMonth, minClientsYear,
            maxClientsDay, maxClientsWeek, maxClientsMonth, maxClientsYear,
            avgClientsDay, avgClientsWeek, avgClientsMonth, avgClientsYear;

    in >> versionStringUtf8
       >> salt >> hash
       >> bannedNicknames
       >> totalTime
       >> closingDate
       >> timeDay >> timeWeek >> timeMonth >> timeYear
       >> minClientsDay >> minClientsWeek >> minClientsMonth >> minClientsYear
       >> maxClientsDay >> maxClientsWeek >> maxClientsMonth >> maxClientsYear
       >> avgClientsDay >> avgClientsWeek >> avgClientsMonth >> avgClientsYear
       >> totalMessagesDay >> totalMessagesWeek >> totalMessagesMonth >> totalMessagesYear
       >> totalClientsLogoutDay >> totalClientsLogoutWeek >> totalClientsLogoutMonth >> totalClientsLogoutYear
       >> totalClientsTimeoutDay >> totalClientsTimeoutWeek >> totalClientsTimeoutMonth >> totalClientsTimeoutYear;

    if (QString::fromUtf8(versionStringUtf8) == QString("VuzdarChat1.0-AdminInfo\n").toUtf8()) {
        // trebalo bi sve bit ok
        adminInfo = AdminInfo(salt, hash,
                              bannedNicknames,
                              totalTime,
                              closingDate,
                              timeDay, timeWeek, timeMonth, timeYear,
                              minClientsDay, minClientsWeek, minClientsMonth, minClientsYear,
                              maxClientsDay, maxClientsWeek, maxClientsMonth, maxClientsYear,
                              avgClientsDay, avgClientsWeek, avgClientsMonth, avgClientsYear,
                              totalMessagesDay, totalMessagesWeek, totalMessagesMonth, totalMessagesYear,
                              totalClientsLogoutDay, totalClientsLogoutWeek, totalClientsLogoutMonth, totalClientsLogoutYear,
                              totalClientsTimeoutDay, totalClientsTimeoutWeek, totalClientsTimeoutMonth, totalClientsTimeoutYear
                              );
    } else {
        // kriva verzija il nesto je lose
        // za sad samo vrati prazan AdminInfo
        // (ako mu damo prazan password to je kao da nema passworda,
        // tj stavit ce hash i salt na QByteArray() kojem ni jedan
        // hash(salt + password) nece biti jednak)
        adminInfo = AdminInfo("");
    }

    return in;
}
