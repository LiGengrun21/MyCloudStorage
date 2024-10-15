#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();

    // sigleton pattern
    static MyTcpServer &getInstance();

    void incomingConnection(qintptr socketDescriptor);

public slots:
    void deleteSocket(MyTcpSocket* socket); // the parameter of the slot must be same as the signal, which is offline in mytcpsocket

private:
    QList <MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
