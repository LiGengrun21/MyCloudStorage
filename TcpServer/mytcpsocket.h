#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();

signals:
    void offline(MyTcpSocket* socket); // offline signal

public slots:
    void recvMsg();
    void clientOffline(); // client offline

private:
    QString m_strName;
};

#endif // MYTCPSOCKET_H
