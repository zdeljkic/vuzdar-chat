#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    QHostAddress getAddress();
    quint16 getPort();
    bool connectToServer(QString hostname, quint16 port);
    void disconnectFromServer();
    void sendData(QByteArray data);

public slots:

private:
    QTcpSocket socket;

private slots:
    void receiveData();

signals:
    void newInfoText(QString text);
};

#endif // CONNECTION_H
