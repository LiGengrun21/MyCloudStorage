#ifndef FRIEND_H
#define FRIEND_H
#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsers(PDU *pdu); // show all online users

    QString m_searchName; // tmp, for showing name in QMessageBox while searching

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget; // friend list
    QLineEdit *m_pInputMsgLe;
    QPushButton *m_pDelFriendPB; // delete friend
    QPushButton *m_pFlushFriendPB; // flush friend list
    QPushButton *m_pShowOnlineUsersPB; // show online users
    QPushButton *m_pSearchUsersPB; //search users
    QPushButton *m_pMsgSendPB; // send message
    QPushButton *m_pPrivateChatPB; // private chat
    Online *m_pOnline; // show online users
signals:

public slots:
    void showOnline(); // show online users
    void searchUser();
};

#endif // FRIEND_H
