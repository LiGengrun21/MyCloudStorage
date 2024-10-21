#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"
#include "privatechat.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSocket, SIGNAL(connected()) // signal sender and what signals it sends
            , this, SLOT(showConnect())); // signal receiver and which function to process the signal

    // receive data from server
    connect(&m_tcpSocket, SIGNAL(readyRead())
            ,this, SLOT(recvMsg()));

    m_tcpSocket.connectToHost(QHostAddress(m_StrIP), m_usPort); // connect to the server
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly)){
        QByteArray data =  file.readAll();
        QString strData = data.toStdString().c_str();
        file.close();
        // replace and split to get ip and port
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        // for(int i=0;i<strList.size();i++){
        //     qDebug() <<  strList[i];
        // }
        m_StrIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP:" << m_StrIP << "port:" << m_usPort;
    }
    else{
        QMessageBox::critical(this,"load this config","Config loading failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getMyLoginName()
{
    return m_strMyLoginName;
}

QString TcpClient::getCurrentPath()
{
    return m_strCurPath;
}

void TcpClient::setCurPath(QString path)
{
    m_strCurPath = path;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"connect to server","Connection is successful");
}

void TcpClient::recvMsg()
{
    // test
    //qDebug()<<"tcp client socket byts:" << m_tcpSocket.bytesAvailable();

    uint uiPDULen=0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen); // receive pdu from server
    m_tcpSocket.read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));

    // test
    // qDebug() << "tcp client: pdu message type: " <<pdu->uiMsgType;
    // QString output;
    // for (int i = 0; i < 64; ++i) {
    //     output.append(pdu->caData[i]);
    // }
    // qDebug() << "tcp client: pdu caData: " <<output;


    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGISTER_RESPONSE: // register respnse
    {
        if (strcmp(pdu->caData,REGISTER_OK)==0){
            QMessageBox::information(this, "register", "register successful");
        }
        else if (strcmp(pdu->caData,REGISTER_FAILED)==0){
            QMessageBox::information(this,"register","register failed");
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPONSE: // login response
    {
        if (strcmp(pdu->caData,LOGIN_OK)==0){
            m_strCurPath = QString("./storage/%1").arg(m_strMyLoginName); // if logined, store the current path
            QMessageBox::information(this, "login", "login successful");
            OpeWidget::getInstance().show(); // if login successful, show the operate widget, which is the main window of the application
            hide(); // hide tcp client, which is the login window
        }
        else if (strcmp(pdu->caData,LOGIN_FAILED)==0){
            QMessageBox::information(this,"login","login failed");
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsers(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_RESPONSE:
    {
        QString name = OpeWidget::getInstance().getFriend()->m_searchName;
        if (strcmp(SEARCH_USER_NO, pdu->caData)==0){
            QMessageBox::information(this, "search", QString("%1 not exist").arg(name));
        }
        else if (strcmp(SEARCH_USER_ONLINE, pdu->caData)==0){
            QMessageBox::information(this, "search", QString("%1 is online").arg(name));
        }
        else if (strcmp(SEARCH_USER_OFFLINE, pdu->caData)==0){
            QMessageBox::information(this, "search", QString("%1 is offline").arg(name));
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: // from the client (forwarded by server)who wants to add you as a friend
    {
        char caName[32] = {'\0'};
        char caMyName[32] = {'\0'};
        strncpy(caName, pdu->caData, 32); // the user who sent you request
        strncpy(caMyName, pdu->caData+32, 32); // me who received the request
        PDU *respPdu = mkPDU(0);
        // first: me; second: the user who send the request
        memcpy(respPdu->caData, caMyName, 32);
        memcpy(respPdu->caData+32, caName, 32);
        int result = QMessageBox::information(this, "Friend Request", QString("%1 sent friend request to you.").arg(caName),QMessageBox::Yes, QMessageBox::No); // two options: yes or no
        if (result == QMessageBox::Yes){
            respPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT;
        }
        else{
            respPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
        }
        m_tcpSocket.write((char*) respPdu, respPdu->uiPDULen);
        free(respPdu);
        respPdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE: // from the server, your add request is declined due to some reason
    {
        QMessageBox::information(this, "Friend Requst", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE:
    {
        QMessageBox::information(this, "Remove Friend", "removed successfully");
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        PrivateChat::getInstance().setWindowTitle(m_strMyLoginName); // set window title for the user
        if (PrivateChat::getInstance().isHidden()){ // if the chat window is closed, show it first, and then update the chat messages.
            char senderName[32]={'\0'};
            memcpy(senderName, pdu->caData, 32);
            QString strSenderName = senderName;
            PrivateChat::getInstance().setChatName(strSenderName); // tell this chat window who sent the message
            PrivateChat::getInstance().show();
        }
        PrivateChat::getInstance().updateMsg(pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
    {
        OpeWidget::getInstance().getFriend()->updateGroupChatMsg(pdu); // show new message in group chat
        break;
    }
    case ENUM_MSG_TYPE_CREATE_FOLDER_RESPONSE:
    {
        QMessageBox::information(this, "Create Folder", pdu->caData);
        //OpeWidget::getInstance().getFile()->updateFileList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FOLDER_RESPONSE:
    {
        if (pdu->caData[0]!='\0'){ // caData not empty means it's from entering folder's response, therefore, cur path should be updated
            m_strCurPath=QString(pdu->caData);
            qDebug()<<"cur path in flush folder response"<<m_strCurPath;
        }
        OpeWidget::getInstance().getFile()->updateFileList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FOLDER_RESPONSE:
    {
        QMessageBox::information(this, "Delete Folder", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FOLDER_RESPONSE:
    {
        QMessageBox::information(this, "Rename Folder", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTER_FOLDER_RESPONSE:
    {
        QMessageBox::information(this, "Enter Folder", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE:
    {
        QMessageBox::information(this, "Upload file", pdu->caData);
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
}

// user login
void TcpClient::on_login_btn_clicked()
{
    QString strName = ui->username_le->text();
    QString strPwd = ui->password_le->text();
    if (!strName.isEmpty()&&!strPwd.isEmpty()){
        // store the login name
        m_strMyLoginName = strName;
        PDU *pdu = mkPDU(0); //because message does not contain anything
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        // username and password are stored in caData, not caMsg.
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(32+pdu->caData, strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else{
        QMessageBox::critical(this,"Login","login failed");
    }
}

// user register
void TcpClient::on_register_btn_clicked()
{
    QString strName = ui->username_le->text();
    QString strPwd = ui->password_le->text();
    if (!strName.isEmpty()&&!strPwd.isEmpty()){
        PDU *pdu = mkPDU(0); //because message does not contain anything
        pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;
        // username and password are stored in caData, not caMsg.
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(32+pdu->caData, strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else{
        QMessageBox::critical(this,"Register a new account","failed to register: username or password is empty");
    }
}

