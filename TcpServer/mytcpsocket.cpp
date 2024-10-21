#include "mytcpsocket.h"
#include "mytcpserver.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QFile>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, SIGNAL(readyRead())
            ,this, SLOT(recvMsg()));

    // if client is offline, the socket sends disconnected signal, and clientOffline will process it
    connect(this, SIGNAL(disconnected())
            ,this, SLOT(clientOffline()));

    m_upload = false; // normally, the server is not in the state of uploading something
}

// getter of m_strName
QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    // Not PDU, it's uploading file
    if (m_upload){
        //test
        qDebug()<<"receiving file data";

        PDU *filePdu = mkPDU(0);
        filePdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_receivedSize+=buff.size();
        if (m_totalSize==m_receivedSize){
            strcpy(filePdu->caData, "Uploaded successfully!");
        }
        else if (m_totalSize<m_receivedSize){
            strcpy(filePdu->caData, "Uploaded failed!");
        }
        write((char*) filePdu, filePdu->uiPDULen);
        free(filePdu);
        filePdu=NULL;
        m_file.close();
        m_upload=false;
        return;
    }

    // PDU

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
            // if registered sucessfully, create a folder for the user, where all his files will be stored
            QDir dir;
            qDebug()<<"if the user's folder is created:" << dir.mkdir(QString("./storage/%1").arg(username));
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
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        char chatName[32] = {'\0'};
        memcpy(chatName, pdu->caData+32, 32); // get chat partner's name
        // qDebug() << chatName;
        // qDebug() << (char*)pdu->caMsg;
        MyTcpServer::getInstance().forward(chatName, pdu); // forward the request to the chat user
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName, pdu->caData, 32);
        QStringList friendList = OpeDB::getInstance().getFriends(caName); //including offfline friends
        for(int i=0;i<friendList.size();i++){ // forward the request to all friends
            MyTcpServer::getInstance().forward(friendList.at(i).toStdString().c_str(), pdu);
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FOLDER_REQUEST:
    {
        respPdu=mkPDU(0);
        respPdu->uiMsgType=ENUM_MSG_TYPE_CREATE_FOLDER_RESPONSE;
        //qDebug()<<respPdu->uiMsgType;

        // before creating, first check if the current path exists.
        QDir dir;
        QString curPath = QString("%1").arg((char*)pdu->caMsg);
        bool result = dir.exists(curPath); // if the base dir exists?
        if (!result){
            // base dir not exists
            //qDebug()<< "base dir not exists";
            //qDebug()<<"base path:"<<curPath;
            strcpy(respPdu->caData, BASE_DIR_NOT_EXIST);
            write((char*) respPdu, respPdu->uiPDULen);
            free(respPdu);
            respPdu = NULL;
            break;
        }
        // base dir exist
        char newDir[32] = {'\0'};
        memcpy(newDir, pdu->caData+32, 32);
        QString newCurPath = curPath+"/"+newDir;
        bool result2 = dir.exists(newCurPath); // if the new dir exists?
        if (result2){
            // if exists, change to another name
            qDebug()<< "change to another name";
            strcpy(respPdu->caData, DIR_ALREADY_EXITS);
            write((char*) respPdu, respPdu->uiPDULen);
            free(respPdu);
            respPdu = NULL;
            break;
        }
        // not exist, create it
        dir.mkdir(newCurPath);
        // qDebug()<<"base path:"<<curPath;
        // qDebug()<<"new dir name:" <<newDir;
        // qDebug()<<"new current path:"<<newCurPath;
        strcpy(respPdu->caData, CREATE_DIR_OK);
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FOLDER_REQUEST:
    {
        char* pCurPath = new char[pdu->uiMsgLen];
        memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLen); // get the current path
        QDir dir(pCurPath); // give it the path to operate
        QFileInfoList fileList = dir.entryInfoList(); // get the files in this path
        int fileCount = fileList.size();
        // each file is stored in the format of FileInfo struct in caMsg of pdu
        respPdu = mkPDU(sizeof(FileInfo)*fileCount);
        respPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FOLDER_RESPONSE;
        FileInfo* pFileInfo = NULL; // the pointer to help store Qfile info list elements to file info struct
        QString strFileName;
        //qDebug()<<"(FileInfo*)(respPdu->caMsg)"<<(FileInfo*)(respPdu->caMsg);
        for (int i=0;i<fileList.size();i++){
            // access FileInfo struct in respPdu
            pFileInfo = (FileInfo*)(respPdu->caMsg)+i;
            //qDebug()<<i<<" pFileInfo: "<<pFileInfo;
            // get file name and type to store in FileInfo
            strFileName = fileList[i].fileName();
            memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());
            if (fileList[i].isDir()){
                pFileInfo->iFileType=0; // folder
            }
            else if (fileList[i].isFile()){
                pFileInfo->iFileType = 1; // normal file
            }
        }
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FOLDER_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName, pdu->caData,32);
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        QString strFilePath = QString("%1/%2").arg(pPath).arg(caName); //concat the string to make file path
        qDebug()<<strFilePath;
        QFileInfo file(strFilePath);
        bool result = false;
        if (file.isDir()){ // it is a dir
            QDir dir;
            dir.setPath(strFilePath);
            result = dir.removeRecursively(); // remove the dir recursively, meaning all files inside the dir deleted as well
        }
        else if (file.isFile()){ // not a dir, just a normal file
            result=false;
        }
        respPdu=mkPDU(0);
        respPdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FOLDER_RESPONSE;
        if (result){
            strcpy(respPdu->caData, "Deleted successfully!");
        }
        else{
            strcpy(respPdu->caData, "Failed to delete the directory!");
        }
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FOLDER_REQUEST:
    {
        char oldName[32]={'\0'};
        char newName[32]={'\0'};
        strncpy(oldName, pdu->caData, 32);
        strncpy(newName, pdu->caData+32, 32);
        char* pPath=new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        // concat two paths
        QString strOldFilePath = QString("%1/%2").arg(pPath).arg(oldName);
        QString strNewFilePath = QString("%1/%2").arg(pPath).arg(newName);
        // rename it with QDir
        QDir dir;
        bool result = dir.rename(strOldFilePath, strNewFilePath);
        respPdu=mkPDU(0);
        respPdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FOLDER_RESPONSE;
        if (result){
            strcpy(respPdu->caData, "Renamed successfully!");
        }
        else{
            strcpy(respPdu->caData, "Failed to rename the directory!");
        }
        write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ENTER_FOLDER_REQUEST:
    {
        char dirName[32]={'\0'};
        strncpy(dirName, pdu->caData, 32);
        char *pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        QString strDirPath = QString("%1/%2").arg(pPath).arg(dirName);
        QFileInfo fileInfo(strDirPath);
        if (fileInfo.isDir()){ // if dir, flush the files
            QDir dir(strDirPath); // give it the path to operate
            QFileInfoList fileList = dir.entryInfoList(); // get the files in this path
            int fileCount = fileList.size();
            // each file is stored in the format of FileInfo struct in caMsg of pdu
            respPdu = mkPDU(sizeof(FileInfo)*fileCount);
            // for upfating cur path in TcpClient
            strncpy(respPdu->caData, strDirPath.toStdString().c_str(), strDirPath.size()); // also send path to update current path in tcp client
            respPdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FOLDER_RESPONSE;
            FileInfo* pFileInfo = NULL; // the pointer to help store Qfile info list elements to file info struct
            QString strFileName;
            //qDebug()<<"(FileInfo*)(respPdu->caMsg)"<<(FileInfo*)(respPdu->caMsg);
            for (int i=0;i<fileList.size();i++){
                // access FileInfo struct in respPdu
                pFileInfo = (FileInfo*)(respPdu->caMsg)+i;
                //qDebug()<<i<<" pFileInfo: "<<pFileInfo;
                // get file name and type to store in FileInfo
                strFileName = fileList[i].fileName();
                memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());
                if (fileList[i].isDir()){
                    pFileInfo->iFileType=0; // folder
                }
                else if (fileList[i].isFile()){
                    pFileInfo->iFileType = 1; // normal file
                }
            }
            write((char*) respPdu, respPdu->uiPDULen);
            free(respPdu);
            respPdu = NULL;
        }
        else{ // not a dir
            respPdu = mkPDU(0);
            respPdu->uiMsgType=ENUM_MSG_TYPE_ENTER_FOLDER_RESPONSE;
            strcpy(respPdu->caData, "Failed to enter the folder!");
            write((char*) respPdu, respPdu->uiPDULen);
            free(respPdu);
            respPdu = NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
    {
        //test
        //qDebug()<<"ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST in server";
        // QString output;
        // for (int i = 0; i < 64; ++i) {
        //     output.append(pdu->caData[i]);
        // }
        // qDebug() << "server uplaod file: pdu caData: " <<output;

        char caFileName[32]={'\0'};
        qint64 fileSize =0;
        // get file name and size
        int ret = sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);
        //qDebug()<<"scanf: "<<ret;
        char* pPath = new char[pdu->uiMsgLen];
        memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
        QString strFilePath = QString("%1/%2").arg(pPath).arg(caFileName); //concat the string to make file path
        //test
        //qDebug()<<"ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST"<<"file name: "<<caFileName<<"file size: "<<fileSize;
        delete [] pPath;
        pPath = NULL;

        m_file.setFileName(strFilePath);
        // if not exists, create a new one
        if (m_file.open(QIODevice::WriteOnly)){
            qDebug()<<"file created and opened!";
            m_upload=true;
            m_totalSize=fileSize;
            m_receivedSize=0;
        }
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
