#include "vuzdarpacket.h"

VuzdarPacket::VuzdarPacket(QByteArray data)
{
    this->data = data;

    type = (PacketType) (quint8) data[0];
    length = convert(data[1], data[2]);

    if (data.size() != length - 3) {
        type = MALFORMED_PACKET;
    }
}

VuzdarPacket::~VuzdarPacket()
{
}

QString VuzdarPacket::getNickname()
{
    // relevantno za registraciju

    QString nickname(data.mid(3, length));

    if (checkName(nickname) == true)
        return nickname;
    else
        return QString();
}

quint8 VuzdarPacket::getControlCode()
{
    return (quint8) data[3];
}

QList<QPair<quint16, QString> > VuzdarPacket::getAliveClientList()
{
    QList<QPair<quint16, QString> > list;
    QPair<quint16,QString> pair;
    int end;

    for (int i = 4; i < data.size() - 3; i = end + 1) {
        pair.first = convert(data[i], data[i+1]);

        end = data.indexOf('\0', i+2);

        if (end == -1)
            return list;

        pair.second = QString(data.mid(i+2, end - (i+2)));

        list << pair;
    }

    return list;
}

QList<quint16> VuzdarPacket::getDeadClientList()
{
    QList<quint16> list;

    if (data.size() % 2 != 0)
        return QList<quint16>();

    for (int i = 4; i < data.size(); i += 2) {
        list << convert(data[i], data[i+1]);
    }

    return list;
}

QString VuzdarPacket::getGroupName()
{
    if (data.indexOf('\0', 4) == -1)
        return QString();

    int length = 4 - data.indexOf('\0', 4);

    QString name(data.mid(4, length));

    if (checkName(name) == true)
        return name;
    else
        return QString();
}

QList<quint16> VuzdarPacket::getGroupClientList(bool server)
{
    // relevantno za grupnu tekstualnu poruku

    QList<quint16> list;
    int start;

    if (server)
        start = data.indexOf('\0', 4);
    else
        start = data.indexOf('\0', 6);

    if (start == -1)
        return QList<quint16>();

    ++start;

    if ((data.size() - start) % 2 == 0)   // za slucaj da je krivo formiran paket
                                          // (nema paran broj byteova u nizu client id-ova koji su svi po 2 bytea)
        return QList<quint16>();

    for (int i = start; i < data.size(); i += 2) {
        list << convert(data[i], data[i+1]);
    }

    return list;
}

quint16 VuzdarPacket::getId()
{
    // relevantno za direktnu i grupnu tekstualnu poruku

    return convert(data[4], data[5]);
}

quint16 VuzdarPacket::getSecondId()
{
    // relevantno za grupnu tekstualnu poruku

    return convert(data[6], data[7]);
}

QString VuzdarPacket::getMessage()
{
    // relevantno za direktnu i grupno tekstualnu poruku
    // vraca poruku ako je dobra inace null qstring

    QString message;

    if (type == TEXT_PRIVATE_MESSAGE)
        message = QString(data.mid(6, length - 3));
    else if (type == TEXT_GROUP_MESSAGE)
        message = QString(data.mid(8, length - 5));
    else
        return QString();

    if (checkMessage(message) == true)
        return message;
    else
        return QString();
}

quint16 VuzdarPacket::getChangedClientId()
{
    return convert(data[6], data[7]);
}

QByteArray VuzdarPacket::generateControlCodePacket(VuzdarPacket::PacketType type, quint8 controlCode)
{
    QByteArray newData = generateEmptyPacket(type, 1);

    newData[3] = controlCode;

    return newData;
}

QByteArray VuzdarPacket::generateRegistrationPacket(QString nickname)
{
    QByteArray rawNickname = nickname.toUtf8();
    QByteArray newData = generateEmptyPacket(REGISTRATION, rawNickname.length());

    newData.replace(3, rawNickname);

    return newData;
}

QByteArray VuzdarPacket::generateAliveClientPacket(QList<QPair<quint16, QString> > list)
{
    quint16 length = 0;

    for (int i = 0; i < list.size(); ++i) {
        length += 2 + list[i].second.length() + 1;
    }

    QByteArray newData = generateEmptyPacket(CLIENT_ACTIVITY, length);

    newData[3] = 0x01;

    int pos = 4;
    QPair<quint8, quint8> pair;
    QByteArray rawNickname;

    for (int i = 0; i < list.size(); ++i) {
        pair = convert(list[i].first);

        newData[pos] = pair.first;
        newData[pos+1] = pair.second;
        pos += 2;

        rawNickname = list[i].second.toUtf8();
        newData.replace(pos, rawNickname.length(), rawNickname);
        pos += rawNickname.length();

        newData[pos] = '\0';
        ++pos;
    }

    return newData;
}

QByteArray VuzdarPacket::generateDeadClientPacket(QList<quint16> list)
{
    quint16 length = list.size() * 2;

    QByteArray newData = generateEmptyPacket(CLIENT_ACTIVITY, length);

    newData[3] = 0x00;

    int pos = 4;
    QPair<quint8, quint8> pair;

    for (int i = 0; i < list.size(); ++i) {
        pair = convert(list[i]);

        newData[pos] = pair.first;
        newData[pos+1] = pair.second;
        pos += 2;
    }

    return newData;
}

QByteArray VuzdarPacket::generateTextPrivateMessagePacket(quint8 controlCode, quint16 id, QString message)
{
    QByteArray rawMessage = message.toUtf8();

    QByteArray newData = generateEmptyPacket(TEXT_PRIVATE_MESSAGE, 1 + 2 + rawMessage.length());

    newData[3] = controlCode;

    QPair<quint8, quint8> pair = convert(id);
    newData[4] = pair.first;
    newData[5] = pair.second;

    newData.replace(6, rawMessage.length(), rawMessage);

    return newData;
}

QByteArray VuzdarPacket::generateTextGroupMessagePacket(quint8 controlCode, quint16 groupId, quint16 clientId, QString message)
{
    QByteArray rawMessage = message.toUtf8();

    QByteArray newData = generateEmptyPacket(TEXT_GROUP_MESSAGE, 1 + 2 + 2 + rawMessage.length());

    newData[3] = controlCode;

    QPair<quint8, quint8> pair = convert(groupId);
    newData[4] = pair.first;
    newData[5] = pair.second;

    pair = convert(clientId);
    newData[6] = pair.first;
    newData[7] = pair.second;

    newData.replace(8, rawMessage.length(), rawMessage);

    return newData;
}

QByteArray VuzdarPacket::generateGroupMemberChangePacket(quint8 controlCode, quint16 groupId, quint16 clientId)
{
    QByteArray newData = generateEmptyPacket(TEXT_GROUP_MESSAGE, 5);

    newData[3] = controlCode;

    QPair <quint8, quint8> pair = convert(groupId);
    newData[4] = pair.first;
    newData[5] = pair.second;

    pair = convert(clientId);
    newData[6] = pair.first;
    newData[7] = pair.second;

    return newData;
}

QByteArray VuzdarPacket::generateCreateGroupMessagePacket(QString name, QList<quint16> list)
{
    QByteArray rawName = name.toUtf8();

    int length = 1 + rawName.length() + 1 + list.size() * 2;

    QByteArray newData = generateEmptyPacket(TEXT_GROUP_MESSAGE, length);

    newData[3] = 0x10;

    int pos = 4;

    newData.replace(pos, rawName.length(), rawName);
    pos += rawName.length();

    newData[pos] = '\0';
    ++pos;

    QPair <quint8, quint8> pair;

    for (int i = 0; i < list.size(); ++i) {
        pair = convert(list[i]);

        newData[pos] = pair.first;
        newData[pos+1] = pair.second;
        pos += 2;
    }

    return newData;
}

QByteArray VuzdarPacket::generateCreateGroupReplyPacket(quint8 controlCode, quint16 id)
{
    QByteArray newData = generateEmptyPacket(TEXT_GROUP_MESSAGE, 3);

    newData[3] = controlCode;

    QPair <quint8, quint8> pair = convert(id);
    newData[4] = pair.first;
    newData[5] = pair.second;

    return newData;
}

QByteArray VuzdarPacket::generateNewGroupMessagePacket(quint16 id, QString name, QList<quint16> list)
{
    QByteArray rawName = name.toUtf8();

    int length = 1 + 2 + rawName.length() + 1 + list.size() * 2;

    QByteArray newData = generateEmptyPacket(TEXT_GROUP_MESSAGE, length);

    newData[3] = 0x10;

    QPair <quint8, quint8> pair = convert(id);
    newData[4] = pair.first;
    newData[5] = pair.second;

    int pos = 6;

    newData.replace(pos, rawName.length(), rawName);
    pos += rawName.length();

    newData[pos] = '\0';
    ++pos;

    for (int i = 0; i < list.size(); ++i) {
        pair = convert(list[i]);

        newData[pos] = pair.first;
        newData[pos+1] = pair.second;
        pos += 2;
    }

    return newData;
}

quint16 VuzdarPacket::convert(char first, char second)
{
    return 256 * (quint16) first + (quint16) second;
}

QPair<quint8, quint8> VuzdarPacket::convert(quint16 x)
{
    QPair<quint8, quint8> pair;

    pair.first = x / 256;
    pair.second = x % 256;

    return pair;
}

bool VuzdarPacket::checkName(QString name)
{
    for (int i = 0; i < name.size(); ++i) {
        if (!(name[i].isLetterOrNumber() || name[i] == ' ' || name[i] == '_'))
            return false;
    }

    return true;
}

bool VuzdarPacket::checkMessage(QString message)
{
    for (int i = 0; i < message.size(); ++i) {
        if (message[i].category() == QChar::Other_Control ||
                message[i].category() == QChar::Other_Format ||
                message[i].category() == QChar::Other_Surrogate ||
                message[i].category() == QChar::Other_PrivateUse ||
                message[i].category() == QChar::Other_NotAssigned)
            return false;
    }

    return true;
}

QByteArray VuzdarPacket::generateEmptyPacket(VuzdarPacket::PacketType type, quint16 length)
{
    QByteArray newData(length + 3, '\0');

    newData[0] = (quint8) type;

    QPair<quint8, quint8> pair = convert(length);

    newData[1] = pair.first;
    newData[2] = pair.second;

    return newData;
}



