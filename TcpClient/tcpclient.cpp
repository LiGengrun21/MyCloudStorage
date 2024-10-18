#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"

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
    qDebug() << "tcp client: pdu message type: " <<pdu->uiMsgType;
    QString output;
    for (int i = 0; i < 64; ++i) {
        output.append(pdu->caData[i]);
    }
    qDebug() << "tcp client: pdu caData: " <<output;


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
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
}

// void TcpClient::on_sendButton_clicked()
// {
//     QString strMsg = ui->lineEdit->text();
//     if (!strMsg.isEmpty()){
//         PDU *pdu = mkPDU(strMsg.size());
//         pdu->uiMsgType = 8888;
//         memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
//         m_tcpSocket.write((char*)pdu, pdu->uiPDULen); //send the pdu
//         free(pdu);
//         pdu=NULL;
//     }
//     else{
//         QMessageBox::warning(this, "send message", "message cannot be empty");
//     }
// }

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

