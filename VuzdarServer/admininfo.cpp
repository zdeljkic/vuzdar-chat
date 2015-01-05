#include "admininfo.h"

AdminInfo::AdminInfo(QString password)
{

}

AdminInfo::~AdminInfo()
{

}



QDataStream &operator<<(QDataStream &out, const AdminInfo &adminInfo)
{
    adminInfo.prepareForSaving();

    // !!VAZNO!! promijenit ovdje verziju ako treba
    out << QString("VuzdarChat1.0-AdminInfo\n").toUtf8()
        << adminInfo.salt << adminInfo.hash
        << adminInfo.bannedNicknames
        << adminInfo.totalTime
        << adminInfo.closingDateTime
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
    QDateTime closingDateTime;
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
       >> closingDateTime
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
                              closingDateTime,
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
