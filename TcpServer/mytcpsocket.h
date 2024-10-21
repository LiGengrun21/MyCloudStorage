#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QFile>
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

    // uploading files related attr
    QFile m_file;
    qint64 m_totalSize; // total size of uploaed file
    qint64 m_receivedSize; // how many received so far
    bool m_upload; // if the state is uploading file
};

#endif // MYTCPSOCKET_H
