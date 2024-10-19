#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::on_sendMsgPB_clicked()
{
    QString inputMsg  = ui->inputMsgLE->text();
    // after sending, display the sent message and clear the line edit
    QString inputMsgShown = QString("I said: "+inputMsg);
    ui->showMsgTE->append(inputMsgShown);
    ui->inputMsgLE->clear();

    if (!inputMsg.isEmpty()){
        PDU *pdu = mkPDU(inputMsg.size()+1); // +1 means '\0'
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        // QString myName = TcpClient::getInstance().getMyLoginName();
        // caData contains my name and chat partner's name
        memcpy(pdu->caData, m_strMyName.toStdString().c_str(), m_strMyName.size());
        memcpy(pdu->caData+32, m_strChatName.toStdString().c_str(), m_strChatName.size());
        strcpy((char*)(pdu->caMsg), inputMsg.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else{
        QMessageBox::information(this, "Private Chat", "Enter the Message please!");
    }
}

void PrivateChat::setChatName(QString strName){
    m_strChatName = strName;
    m_strMyName = TcpClient::getInstance().getMyLoginName();
}

void PrivateChat::updateMsg(PDU *pdu)
{
    if (pdu == NULL){
        return;
    }
    char senderName[32] = {'\0'};
    memcpy(senderName, pdu->caData, 32);
    QString strMsg = QString("%1 said: %2").arg(senderName).arg((char*) pdu->caMsg);
    ui->showMsgTE->append(strMsg); // append the new message
}

PrivateChat& PrivateChat::getInstance(){
    static PrivateChat instance;
    return instance;
}

