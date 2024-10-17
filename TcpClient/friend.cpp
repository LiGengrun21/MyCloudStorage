#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit();
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

void Friend::showAllOnlineUsers(PDU *pdu)
{
    if (pdu==NULL){
        return;
    }
    m_pOnline->showUsers(pdu);
}
