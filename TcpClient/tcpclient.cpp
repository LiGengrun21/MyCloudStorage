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

void TcpClient::showConnect()
{
    QMessageBox::information(this,"connect to server","Connection is successful");
}

void TcpClient::recvMsg()
{
    qDebug()<<m_tcpSocket.bytesAvailable();
    uint uiPDULen=0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen); // receive pdu from client
    m_tcpSocket.read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));

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

