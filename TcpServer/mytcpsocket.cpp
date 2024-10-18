#include "mytcpsocket.h"
#include "mytcpserver.h"
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
    //qDebug() << "my tcp socket recvMsg, bytes of socket:" <<this->bytesAvailable();
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
        write((char*) respPdu, respPdu->uiPDULen);
        // free(pdu);
        // pdu=NULL;
        free(respPdu);
        respPdu = NULL;
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
        write((char*) respPdu, respPdu->uiPDULen);
        // free(pdu);
        // pdu=NULL;
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList result;
        result=OpeDB::getInstance().getAllOnlineUsers();
        uint uiMsgLen = result.size()*32; // calculate bytes of the result; 32 because name field is varchar(32) in database.
        respPdu = mkPDU(uiMsgLen);
        respPdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE;
        for (int i=0;i<result.size();i++){
            memcpy((char*)(respPdu->caMsg)+i*32
                   , result.at(i).toStdString().c_str()
                   , result.at(i).size()); // copy actual bytes of the name
        }
        write((char*) respPdu, respPdu->uiPDULen);
        // free(pdu);
        // pdu=NULL;
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
    {
        int result = OpeDB::getInstance().searchUser(pdu->caData); // go to database to find the user based on name
        respPdu = mkPDU(0);
        respPdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_RESPONSE;
        if (result == -1){
            strcpy(respPdu->caData, SEARCH_USER_NO);
        }
        else if (result == 1){
            strcpy(respPdu->caData, SEARCH_USER_ONLINE);
        }
        else{
            strcpy(respPdu->caData, SEARCH_USER_OFFLINE);
        }
        write((char*) respPdu, respPdu->uiPDULen);
        // free(pdu);
        // pdu=NULL;
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: // three scenarios: 1. already a friend; 2. not friend yet, but not online; 3. not friend yet, and online
    {
        char myName[32]={'\0'};
        char hisName[32]={'\0'};
        strncpy(myName, pdu->caData,32);
        strncpy(hisName,pdu->caData+32,32);

        //test
        // qDebug() << "myName：" <<myName;
        // qDebug() << "hisName:" <<hisName;
        // for (int i = 0; i < 64; ++i) {
        //     qDebug() << pdu->caData[i];
        // }

        int result = OpeDB::getInstance().addFriend(myName, hisName);
        if (result == -1){ // system error
            respPdu = mkPDU(0);
            respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
            strcpy(respPdu->caData, UNKNOWN_ERROR);
            write((char*) respPdu, respPdu->uiPDULen);
            // free(pdu);
            // pdu=NULL;
            free(respPdu);
            respPdu = NULL;
        }
        else if (result == 0){ // already friends
            respPdu = mkPDU(0);
            respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
            strcpy(respPdu->caData, ADD_FRIEND_ALREADY_FRIENDS);
            write((char*) respPdu, respPdu->uiPDULen);
            // free(pdu);
            // pdu=NULL;
            free(respPdu);
            respPdu = NULL;
        }
        else if (result == 1){ // not friends yet and online
            MyTcpServer::getInstance().forward(hisName, pdu); // forward the add request to the corresponding user
        }
        else if (result == 2){ // not friends yet but not online
            respPdu = mkPDU(0);
            respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
            strcpy(respPdu->caData, ADD_FRIEND_OFFLINE);
            write((char*) respPdu, respPdu->uiPDULen);
            // free(pdu);
            // pdu=NULL;
            free(respPdu);
            respPdu = NULL;
        }
        else{ // the user you want to add as friend does not exist in db
            respPdu = mkPDU(0);
            respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
            strcpy(respPdu->caData, ADD_FRIEND_NOT_EXIST);
            write((char*) respPdu, respPdu->uiPDULen);
            // free(pdu);
            // pdu=NULL;
            free(respPdu);
            respPdu = NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT:
    {
        // do nothing
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT:
    {
        char name1[32]={'\0'};
        char name2[32]={'\0'};
        strncpy(name1, pdu->caData, 32);
        strncpy(name2, pdu->caData+32, 32);
        // insert one item to friend table
        OpeDB::getInstance().insertFriend(name1, name2);
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        char name[32] = {'\0'};
        strncpy(name, pdu->caData,32);
        QStringList result = OpeDB::getInstance().getFriends(name);
        uint uiMsgLen = result.size()*32;
        respPdu = mkPDU(uiMsgLen);
        respPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE;
        // copy every element of the result list into respPdu's caMsg
        for (int i=0; i<result.size();i++){
            memcpy((char*)respPdu->caMsg+32*i, result.at(i).toStdString().c_str(), result.at(i).size());
        }
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char myName[32]={'\0'};
        char friendName[32]={'\0'};
        strncpy(myName, pdu->caData,32);
        strncpy(friendName,pdu->caData+32,32);
        bool result = OpeDB::getInstance().removeFriend(myName, friendName); // delete the friend
        qDebug() << "If deleted successfully" << result;
        // only notify the user who who sent the delete request
        respPdu = mkPDU(0);
        respPdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE;
        strcpy(respPdu->caData, DELETE_OK);
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    default:
        break;
    }
    // write((char*) respPdu, respPdu->uiPDULen);
    free(pdu);
    pdu=NULL;
    // free(respPdu);
    // respPdu = NULL;
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().logout(m_strName.toStdString().c_str());
    // once db operation is done, send the offline signal
    emit offline(this);
}
