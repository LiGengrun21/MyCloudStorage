#include "friend.h"

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit();
    m_pFriendListWidget = new QListWidget();
    m_pInputMsgLe = new QLineEdit();
    m_pDelFriendPB = new QPushButton("Delete Friend");
    m_pFlushFriendPB = new QPushButton("Flush Friend List");
    m_pShowOnlineUsersPB = new QPushButton("Show Online Users");
    m_pSearchUsersPB = new QPushButton("Search Users");
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
}

void Friend::showOnline()
{
    if (m_pOnline->isHidden()){ // click the button, show the online users
        m_pOnline->show();
    }
    else{
        m_pOnline->hide(); // click again, hide the online users
    }
}
