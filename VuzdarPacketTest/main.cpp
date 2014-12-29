#include <QCoreApplication>
#include <QDebug>
#include "../VuzdarCommon/vuzdarpacket.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<quint16 > list;

    list.append(1);
    list.append(2);
    list.append(5);

    QByteArray data = VuzdarPacket::generateGroupMemberChangePacket(0x12, 9, 9);

    //qDebug() << "Data size:         " << data.size();
    qDebug() << "Data (generated):  " << data.toHex();

    VuzdarPacket packet(data);

    //qDebug() << "Data size:         " << packet.getRawData().size();
    qDebug() << "Data (packet):     " << packet.getRawData().toHex();
    qDebug() << "Type:              " << packet.getType();
    qDebug() << "Length:            " << packet.getLength();

    qDebug() << "Control code:      " << packet.getControlCode();
    qDebug() << "Id:                " << packet.getId();
    qDebug() << "Id2:               " << packet.getSecondId();
    //qDebug() << "Nickname:          " << packet.getNickname();
    //qDebug() << "Group name:        " << packet.getGroupName();
    //qDebug() << "Message:           " << packet.getMessage();
    //qDebug() << "Lists equal:       " << (packet.getGroupClientList() == list);

    return a.exec();
}
