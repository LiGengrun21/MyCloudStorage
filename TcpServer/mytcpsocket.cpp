#include "mytcpsocket.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, SIGNAL(readyRead())
            ,this, SLOT(recvMsg()));

    // if client is offline, the socket sends disconnected signal, and clientOffline will process it
    connect(this, SIGNAL(disconnected())
            ,this, SLOT(clientOffline()));
}

// getter of m_strName
QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDULen=0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen); // receive pdu from client
    this->read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));
    //qDebug() << pdu->uiMsgType << (char*)pdu->caMsg;

    PDU* respPdu; // pdu for response
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGISTER_REQUEST: //register request
    {
        char username[32]={'\0'};
        char password[32]={'\0'};
        strncpy(username, pdu->caData,32);
        strncpy(password,pdu->caData+32,32);
        //qDebug() << username << password << pdu->uiMsgType;
        bool result = OpeDB::getInstance().registerAccount(username,password);
        respPdu = mkPDU(0);
        respPdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPONSE;
        if (result){
            strcpy(respPdu->caData, REGISTER_OK);
        }
        else{
            strcpy(respPdu->caData, REGISTER_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST: // login request
    {
        char username[32]={'\0'};
        char password[32]={'\0'};
        strncpy(username, pdu->caData,32);
        strncpy(password,pdu->caData+32,32);
        bool result = OpeDB::getInstance().login(username, password);
        respPdu = mkPDU(0);
        respPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPONSE;
        if (result){
            strcpy(respPdu->caData, LOGIN_OK);
            m_strName = username; // name
        }
        else{
            strcpy(respPdu->caData, LOGIN_FAILED);
        }
        break;
    }
    default:
        break;
    }
    write((char*) respPdu, respPdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    free(respPdu);
    respPdu = NULL;
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().logout(m_strName.toStdString().c_str());
    // once db operation is done, send the offline signal
    emit offline(this);
}
