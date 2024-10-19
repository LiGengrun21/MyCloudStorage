#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected.";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    // to connect sinal and slot for offline
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*))
            , this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::forward(const char *hisName, PDU *pdu)
{
    if (hisName==NULL || pdu==NULL){
        qDebug() << "Null exists in forward function parameters";
        return;
    }
    // get his socket based on his name
    QString strName = hisName;
    // test
    for (int i=0;i<m_tcpSocketList.size();i++){
        if (strName == m_tcpSocketList.at(i)->getName())
        {
            //test
            //qDebug() << "In forward function, print pdu caData to be sent to client";
            // for (int i = 0; i < 64; ++i) {
            //     qDebug() << pdu->caData[i];
            // }
            // qDebug()<<pdu->uiPDULen;
            // qDebug() << m_tcpSocketList.at(i)->getName();

            m_tcpSocketList.at(i)->write((char*) pdu, pdu->uiPDULen); // His socket sends pdu to his client
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *socket)
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin(); // use iterator to get the first element of the list
    for(; iter!=m_tcpSocketList.end();iter++){
        if (socket==*iter){
            delete *iter; // delete the object
            *iter=NULL;
            m_tcpSocketList.erase(iter); // remove the element of the list
            break;
        }
    }

    // test if deleted
    // for (int i=0;i<m_tcpSocketList.size();i++){
    //     qDebug() << m_tcpSocketList.at(i)->getName();
    // }
}
