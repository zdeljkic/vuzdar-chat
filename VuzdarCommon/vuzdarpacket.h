#ifndef VUZDARPACKET_H
#define VUZDARPACKET_H

#include <QByteArray>
#include <QString>
#include <QChar>
#include <QList>
#include <QPair>

class VuzdarPacket
{
public:
    enum PacketType : quint8 { REGISTRATION = 0, CLIENT_ACTIVITY = 1, TEXT_PRIVATE_MESSAGE = 2,
                               TEXT_GROUP_MESSAGE = 3, ADMIN = 4, PING = 5, DISCONNECT = 6,
                               ERROR = 7, MALFORMED_PACKET = 255 };

    VuzdarPacket(QByteArray data);
    ~VuzdarPacket();

    QByteArray getRawData();
    PacketType getType();
    quint16 getLength();
    QString getNickname();
    quint8 getControlCode();
    QList<QPair<quint16,QString> > getAliveClientList();
    QList<quint16> getDeadClientList();
    QString getGroupName();
    QList<quint16> getGroupClientList();
    quint16 getId();
    quint16 getSecondId();
    QString getMessage();
    quint16 getChangedClientId();
    QList<QString> getBannedNicknameList();
    QString getAdminString();

    static quint16 getExpectedLength(QByteArray data);
    static VuzdarPacket generateControlCodePacket(PacketType type, quint8 controlCode);
    static VuzdarPacket generateControlCodeIdPacket(PacketType type, quint8 controlCode, quint16 id);
    static VuzdarPacket generateRegistrationPacket(QString nickname);
    static VuzdarPacket generateAliveClientPacket(QList<QPair<quint16,QString> > list);
    static VuzdarPacket generateDeadClientPacket(QList<quint16> list);
    static VuzdarPacket generateTextPrivateMessagePacket(quint8 controlCode, quint16 id, QString message);
    static VuzdarPacket generateNewGroupPacket(quint16 id, QString name, QList<quint16> list);
    static VuzdarPacket generateTextGroupMessagePacket(quint8 controlCode, quint16 groupId, quint16 clientId, QString message);
    static VuzdarPacket generateGroupMemberChangePacket(quint8 controlCode, quint16 groupId, quint16 clientId);
    static VuzdarPacket generateAuthorizationPacket(QString password);
    static VuzdarPacket generateBannedListPacket(QList<QString> list);
    static VuzdarPacket generateAdminPacket(quint8 controlCode, QString string);

private:
    QByteArray data;
    PacketType type;
    quint16 length;

    static quint16 convert(char first, char second);
    static QPair<quint8, quint8> convert(quint16 x);
    static bool checkName(QString name);
    static bool checkMessage(QString message);
    static QByteArray generateEmptyPacketData(PacketType type, quint16 length);
};

#endif // VUZDARPACKET_H
