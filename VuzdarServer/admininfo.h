#ifndef ADMININFO_H
#define ADMININFO_H

#include <QSet>
#include <QDateTime>
#include <QMap>

class AdminInfo
{
public:
    // postoje dva constructora,
    // jedan kad se stvara novi AdminInfo koji zahtjeca iskljucivo
    // stvari koje treba eksplicitno zadat (password) a ostalo sam postavi
    // i drugi constructor, koji prima sve bitno od starog AdminInfo-a
    // (onog koji je spremljen u datoteku) i iz njega vraca stare podatke
    AdminInfo();

    AdminInfo(QByteArray salt, QByteArray hash,
              QList<QString> bannedNicknames,
              quint64 totalTime,
              QDate closingDate,
              quint32 timeDay, quint32 timeWeek, quint32 timeMonth, quint32 timeYear,
              quint16 minClientsDay, quint16 minClientsWeek, quint16 minClientsMonth, quint16 minClientsYear,
              quint16 maxClientsDay, quint16 maxClientsWeek, quint16 maxClientsMonth, quint16 maxClientsYear,
              quint16 avgClientsDay, quint16 avgClientsWeek, quint16 avgClientsMonth, quint16 avgClientsYear,
              quint32 totalMessagesDay, quint32 totalMessagesWeek, quint32 totalMessagesMonth, quint32 totalMessagesYear,
              quint32 totalClientsLogoutDay, quint32 totalClientsLogoutWeek, quint32 totalClientsLogoutMonth, quint32 totalClientsLogoutYear,
              quint32 totalClientsTimeoutDay, quint32 totalClientsTimeoutWeek, quint32 totalClientsTimeoutMonth, quint32 totalClientsTimeoutYear
              );

    void clear();
    bool isLoadSuccessful();

    void setPassword(QString password);
    bool validatePassword(QString password);

    bool isNicknameBanned(QString nickname);
    void addBannedNickname(QString nickname);
    void removeBannedNickname(QString nickname);

    // startTime() postavlja lastStartingTime na trenutni broj milisekunda od ponoci 1.1.1970.
    // stopTime() oduzima vrijednost lastStartingTime-a od trenutnog broja milisekunda od ponoci 1.1.1970.
    // i to pridodaje totalTime-u
    void startTime();
    void stopTime();

    void increaseClients();
    void decreaseClients(bool timeout);

    void calculateAverageClients();

    void increaseMessages();

    // reset funkcije veceg perioda poziva reset funkciju direktno manjeg perioda
    // npr resetYear() poziva i resetMonth(), koja poziva resetWeek(), koja poziva resetDay()
    void resetDay();
    void resetWeek();
    void resetMonth();
    void resetYear();

    QList<QString> getBannedNicknames();
    QString getStatisticsString();

    friend QDataStream &operator<<(QDataStream &out, AdminInfo &adminInfo);
    friend QDataStream &operator>>(QDataStream &in, AdminInfo &adminInfo);

private:
    // postavlja se kod ucitavanja i cita sa isLoadSuccessful()
    bool loadSuccessful;

    // salt i hash, oboje dugacki 256 bita (32 bytea)
    // salt automatski (slucajno) generiran u prvom constructoru
    // uvijek eksplicitno zadani odnosno generirani
    QByteArray salt;
    QByteArray hash;

    // lista bannanih nadimaka (pocetno prazna, potrebno zadati kod ucitavanja)
    // set bi mozda bio bolji po performansama ako se cesto klijenti spajaju i
    // postoji puno bannanih nadimaka, ali listu je jednostavnije serijalizirat
    // i bolja je ako nema previse bannanih nadimaka pa dok to ne postane problem
    // bannedNicknames ce biti lista
    QList<QString> bannedNicknames;

    // totalTime sadrzi sveukupni broj milisekunda kolko je server radio
    // pocetno je 0, potrebno zadati kod ucitavanja
    // lastStartingTime sadrzi broj milisekunda od ponoci 1.1.1970. i postavlja se kad server
    // starta pomocu funkcije startTime(), a u funkciji stopTime() se pomocu nje racuna
    // koliko je server radio i to se pribraja totalTime-u
    // lastStartingTime nije potrebo ucitati i nije bitna vrijednost do poziva starTime()
    quint64 totalTime;
    quint64 lastStartingTime;

    // potreban za provjerit jel se promijenio dan/tjedan/mjesec/godina ako se AdminInfo
    // ucitava od prije kako bi se znalo jel treba pozvati reset<period>() funkcije.
    // zapisuje se trenutni DateTime u to pozivom funkcije prepareForSaving()
    // koja se automatski poziva u opreatoru <<
    QDate closingDate;

    // numClients i time<period> su potrebni za pracenje prosjecnog broja klijenata
    // numClients je trenutno broj klijenata a time<period> je broj vremenskih jedinica
    // (odredenih u Timer klasi) koje su prosle od pocetka perioda.
    // time<period> se kod svakog poziva funkcije calculateAverageClients() povecavaju
    // za jedan i racuna se prosjecni broj klijenata u periodu pomocu
    // numClients, time<period> i avgClients<period>.
    // time<period> se resetira u reset<period> funkcijama
    // pocetno su svi 0, potrebno je ucitati time<period> dok je numClients
    // uvijek 0 kad server starta (i kod ucitavanja)
    quint16 numClients;

    quint32 timeDay;
    quint32 timeWeek;
    quint32 timeMonth;
    quint32 timeYear;

    // min, max i avg klijenti u periodu, svi su pocetno 0 i potrebno ih je ucitati
    quint16 minClientsDay;
    quint16 minClientsWeek;
    quint16 minClientsMonth;
    quint16 minClientsYear;

    quint16 maxClientsDay;
    quint16 maxClientsWeek;
    quint16 maxClientsMonth;
    quint16 maxClientsYear;

    quint16 avgClientsDay;
    quint16 avgClientsWeek;
    quint16 avgClientsMonth;
    quint16 avgClientsYear;

    // ukupni broj poruka, pocetno 0, ucitava se od prije
    quint32 totalMessagesDay;
    quint32 totalMessagesWeek;
    quint32 totalMessagesMonth;
    quint32 totalMessagesYear;

    // broj odlogiranih i timeoutanih klijenta te omjer
    // brojevi su pocetno 0 i potrebno ih je ucitati,
    // omjer koji nam je potreban se racuna kod ispisa
    quint32 totalClientsLogoutDay;
    quint32 totalClientsLogoutWeek;
    quint32 totalClientsLogoutMonth;
    quint32 totalClientsLogoutYear;

    quint32 totalClientsTimeoutDay;
    quint32 totalClientsTimeoutWeek;
    quint32 totalClientsTimeoutMonth;
    quint32 totalClientsTimeoutYear;

    void prepareForSaving();
};

#endif // ADMININFO_H
