#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();

    void setChatName(QString strName); //set m_strChatName

    void updateMsg(PDU *pdu); // update chat room message box

    static PrivateChat& getInstance();

private slots:
    void on_sendMsgPB_clicked();

private:
    Ui::PrivateChat *ui;

    QString m_strChatName; // private chat partner

    QString m_strMyName; // me, who sent the message request to chat partner
};

#endif // PRIVATECHAT_H
