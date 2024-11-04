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
    void updateFriendList(PDU *pdu); // update friend list (both online and offline)
    void updateGroupChatMsg(PDU *pdu); // add the new message to group chat
    QListWidget *getFriendList();

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
    void searchUser(); // check if the user is online
    void flushFriends(); // flush friend list
    void deleteFriend(); // remove the user from friend list
    void privateChat(); // open a private chat room
    void groupChat(); // send group message
};

#endif // FRIEND_H
