#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include "privatechat.h"
#include <QMessageBox>
#include "opewidget.h"

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit();
    m_pShowMsgTE->setReadOnly(true); // group chat board is readonly
    m_pFriendListWidget = new QListWidget();
    m_pInputMsgLe = new QLineEdit();
    m_pDelFriendPB = new QPushButton("Delete Friend");
    m_pFlushFriendPB = new QPushButton("Flush Friend List");
    m_pShowOnlineUsersPB = new QPushButton("Show Online Users");
    m_pSearchUsersPB = new QPushButton("Search User");
    m_pMsgSendPB = new QPushButton("Send Message");
    m_pPrivateChatPB = new QPushButton("Private Chat");
    m_pOnline = new Online();

    QVBoxLayout *pRightVBL = new QVBoxLayout();
    pRightVBL->addWidget(m_pDelFriendPB);
    pRightVBL->addWidget(m_pFlushFriendPB);
    pRightVBL->addWidget(m_pShowOnlineUsersPB);
    pRightVBL->addWidget(m_pSearchUsersPB);
    pRightVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLe);
    pMsgHBL->addWidget(m_pMsgSendPB);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL,5);
    pMain->addLayout(pMsgHBL,3);

    QHBoxLayout *pOnlineHBL = new QHBoxLayout;
    pOnlineHBL->addWidget(m_pOnline);
    pMain->addWidget(m_pOnline,1); // stretch factor
    m_pOnline->hide(); // hide the online widget at the beginning

    setLayout(pMain);

    connect(m_pShowOnlineUsersPB, SIGNAL(clicked(bool))
            , this, SLOT(showOnline()));
    connect(m_pSearchUsersPB, SIGNAL(clicked(bool))
            , this, SLOT(searchUser()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool))
            , this, SLOT(flushFriends()));
    connect(m_pDelFriendPB, SIGNAL(clicked(bool))
            , this, SLOT(deleteFriend()));
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool))
            , this, SLOT(privateChat()));
    connect(m_pMsgSendPB, SIGNAL(clicked(bool))
            , this, SLOT(groupChat()));
}

void Friend::showOnline()
{
    if (m_pOnline->isHidden()){ // click the button, show the online users
        m_pOnline->show();
        PDU *pdu=mkPDU(0); // 0 because it's a request to get all users, does not contain any parameter
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else{
        m_pOnline->hide(); // click again, hide the online users
    }
}

void Friend::searchUser()
{
    m_searchName = QInputDialog::getText(this, "Search", "username:");
    if (!m_searchName.isEmpty()){
        qDebug() << m_searchName;
    }
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
    memcpy(pdu->caData, m_searchName.toStdString().c_str(), m_searchName.size()); // name is stored in caData
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::flushFriends()
{
    QString myName = TcpClient::getInstance().getMyLoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, myName.toStdString().c_str(), myName.size());
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::deleteFriend()
{
    if (m_pFriendListWidget->currentItem()==NULL)
    {
        // no item is selected
        return;
    }
    QString friendName = m_pFriendListWidget->currentItem()->text();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    QString myName = TcpClient::getInstance().getMyLoginName();
    // caData contains my name and name of the friend I'd like to remove
    memcpy(pdu->caData, myName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, friendName.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::privateChat()
{
    if (m_pFriendListWidget->currentItem()==NULL){
        QMessageBox::information(this, "Private Chat", "Please select a user to chat!");
    }
    QString chatName = m_pFriendListWidget->currentItem()->text();
    PrivateChat::getInstance().setChatName(chatName); // set sender's name and receiver's name
    PrivateChat::getInstance().setWindowTitle(TcpClient::getInstance().getMyLoginName()); // set window title according to the user
    if (PrivateChat::getInstance().isHidden()){ // click the button, if private chat window is hidden, then make it visable
        PrivateChat::getInstance().show();
    }
}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLe->text();
    // show the message I sent in the group chat board, and clear line eidt
    QString myMsgShown = QString("I said: ")+strMsg;
    m_pShowMsgTE->append(myMsgShown);
    m_pInputMsgLe->clear();
    if (strMsg.isEmpty()){
        QMessageBox::warning(this, "Group Chat", "Please enter the message to send");
    }
    PDU *pdu=mkPDU(strMsg.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
    QString myName = TcpClient::getInstance().getMyLoginName();
    strncpy(pdu->caData, myName.toStdString().c_str(), myName.size()); // caData contains name of user who started the group chat
    strncpy((char*)pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size()); // caMsg contains the group message
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::showAllOnlineUsers(PDU *pdu)
{
    if (pdu==NULL){
        return;
    }
    m_pOnline->showUsers(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if (pdu==NULL){
        return;
    }
    m_pFriendListWidget->clear(); // clear the original data
    uint size = pdu->uiMsgLen/32; // number of names
    char name[32]={'\0'};
    for (uint i=0;i<size;i++){
        memcpy(name, (char*)(pdu->caMsg)+32*i,32);
        m_pFriendListWidget->addItem(name); // add the data to the widget
    }
}

void Friend::updateGroupChatMsg(PDU* pdu){
    QString msg = QString("%1 said: %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(msg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}
