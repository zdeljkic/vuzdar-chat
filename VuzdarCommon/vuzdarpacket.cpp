#include "vuzdarpacket.h"

VuzdarPacket::VuzdarPacket(QByteArray data)
{
    this->data = data;

    type = (PacketType) (quint8) data[0];
    length = convert(data[1], data[2]);

    if (length != data.size() - 3) {
        type = MALFORMED_PACKET;
    }
}

VuzdarPacket::~VuzdarPacket()
{
}

QByteArray VuzdarPacket::getRawData()
{
    return data;
}

VuzdarPacket::PacketType VuzdarPacket::getType()
{
    return type;
}

quint16 VuzdarPacket::getLength()
{
    return length;
}

QString VuzdarPacket::getNickname()
{
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

    for (int pos = 4; pos < data.size() - 3; pos = end + 1) {
        pair.first = convert(data[pos], data[pos+1]);
        pos += 2;

        end = data.indexOf('\0', pos);

        if (end == -1)
            return list;

        pair.second = QString(data.mid(pos, end - pos));

        list << pair;
    }

    return list;
}

QList<quint16> VuzdarPacket::getDeadClientList()
{
    QList<quint16> list;

    for (int i = 4; i < data.size() - 1; i += 2) {
        list << convert(data[i], data[i+1]);
    }

    return list;
}

QString VuzdarPacket::getGroupName()
{
    if (data.indexOf('\0', 6) == -1)
        return QString();

    int length = 6 - data.indexOf('\0', 6);

    QString name(data.mid(6, length));

    if (checkName(name) == true)
        return name;
    else
        return QString();
}

QList<quint16> VuzdarPacket::getGroupClientList()
{
    // relevantno za grupnu tekstualnu poruku

    QList<quint16> list;
    int start;

    start = data.indexOf('\0', 6);

    if (start == -1)
        return QList<quint16>();

    ++start;

    for (int i = start; i < data.size() - 1; i += 2) {
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
    // !!VAZNO!! pogledat kak se ovo ponasa sa slanjem praznih poruka i olducit jel to dobro

    QString message("");

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
    return getSecondId();
}

QList<QString> VuzdarPacket::getBannedNicknameList()
{
    QList<QString> list;
    int end;

    for (int i = 4; i < data.size() - 1; i = end + 1) {
        end = data.indexOf('\0', i);
        if (end == -1)
            return list;

        list.append(QString(data.mid(i, end - i)));
    }

    return list;
}

QString VuzdarPacket::getAdminString()
{
    return QString(data.mid(4, length - 1));
}

quint16 VuzdarPacket::getExpectedLength(QByteArray data)
{
    return convert(data[1], data[2]);
}

VuzdarPacket VuzdarPacket::generateControlCodePacket(VuzdarPacket::PacketType type, quint8 controlCode)
{
    QByteArray newData = generateEmptyPacketData(type, 1);

    newData[3] = controlCode;

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateRegistrationPacket(QString nickname)
{
    QByteArray rawNickname = nickname.toUtf8();
    QByteArray newData = generateEmptyPacketData(REGISTRATION, rawNickname.length());

    newData.replace(3, rawNickname.length(), rawNickname);

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateAliveClientPacket(QList<QPair<quint16, QString> > list)
{
    quint16 length = 1;

    for (int i = 0; i < list.size(); ++i) {
        length += 2 + list[i].second.toUtf8().length() + 1;
    }

    QByteArray newData = generateEmptyPacketData(CLIENT_ACTIVITY, length);

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

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateDeadClientPacket(QList<quint16> list)
{
    quint16 length = 1 + list.size() * 2;

    QByteArray newData = generateEmptyPacketData(CLIENT_ACTIVITY, length);

    newData[3] = 0x00;

    int pos = 4;
    QPair<quint8, quint8> pair;

    for (int i = 0; i < list.size(); ++i) {
        pair = convert(list[i]);

        newData[pos] = pair.first;
        newData[pos+1] = pair.second;
        pos += 2;
    }

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateTextPrivateMessagePacket(quint8 controlCode, quint16 id, QString message)
{
    QByteArray rawMessage = message.toUtf8();

    QByteArray newData = generateEmptyPacketData(TEXT_PRIVATE_MESSAGE, 1 + 2 + rawMessage.length());

    newData[3] = controlCode;

    QPair<quint8, quint8> pair = convert(id);
    newData[4] = pair.first;
    newData[5] = pair.second;

    newData.replace(6, rawMessage.length(), rawMessage);

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateTextGroupMessagePacket(quint8 controlCode, quint16 groupId, quint16 clientId, QString message)
{
    QByteArray rawMessage = message.toUtf8();

    QByteArray newData = generateEmptyPacketData(TEXT_GROUP_MESSAGE, 1 + 2 + 2 + rawMessage.length());

    newData[3] = controlCode;

    QPair<quint8, quint8> pair = convert(groupId);
    newData[4] = pair.first;
    newData[5] = pair.second;

    pair = convert(clientId);
    newData[6] = pair.first;
    newData[7] = pair.second;

    newData.replace(8, rawMessage.length(), rawMessage);

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateGroupMemberChangePacket(quint8 controlCode, quint16 groupId, quint16 clientId)
{
    QByteArray newData = generateEmptyPacketData(TEXT_GROUP_MESSAGE, 5);

    newData[3] = controlCode;

    QPair <quint8, quint8> pair = convert(groupId);
    newData[4] = pair.first;
    newData[5] = pair.second;

    pair = convert(clientId);
    newData[6] = pair.first;
    newData[7] = pair.second;

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateBannedListPacket(QList<QString> list)
{
    int length = 1;

    for (int i = 0; i < list.size(); ++i) {
        length += list[i].toUtf8().length() + 1;
    }

    QByteArray newData = generateEmptyPacketData(ADMIN, length);

    newData[3] = 0x20;

    QByteArray rawNickname;

    int pos = 4;

    for (int i = 0; i < list.size(); ++i) {
        rawNickname = list[i].toUtf8();
        newData.replace(pos, rawNickname.length(), rawNickname);
        pos += rawNickname.length();

        newData[pos] = '\0';
        ++pos;
    }

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateAdminPacket(quint8 controlCode, QString string)
{
    QByteArray rawString = string.toUtf8();
    QByteArray newData = generateEmptyPacketData(ADMIN, 1 + rawString.length());

    newData[3] = controlCode;

    newData.replace(4, rawString.length(), rawString);

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateControlCodeIdPacket(PacketType type, quint8 controlCode, quint16 id)
{
    QByteArray newData = generateEmptyPacketData(type, 3);

    newData[3] = controlCode;

    QPair <quint8, quint8> pair = convert(id);
    newData[4] = pair.first;
    newData[5] = pair.second;

    return VuzdarPacket(newData);
}

VuzdarPacket VuzdarPacket::generateNewGroupPacket(quint16 id, QString name, QList<quint16> list)
{
    QByteArray rawName = name.toUtf8();

    int length = 1 + 2 + rawName.length() + 1 + list.size() * 2;

    QByteArray newData = generateEmptyPacketData(TEXT_GROUP_MESSAGE, length);

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

    return VuzdarPacket(newData);
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

QByteArray VuzdarPacket::generateEmptyPacketData(VuzdarPacket::PacketType type, quint16 length)
{
    QByteArray newData(length + 3, '\0');

    newData[0] = (quint8) type;

    QPair<quint8, quint8> pair = convert(length);

    newData[1] = pair.first;
    newData[2] = pair.second;

    return newData;
}



