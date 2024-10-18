#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);

}

Online::~Online()
{
    delete ui;
}

void Online::showUsers(PDU *pdu)
{
    if (pdu==NULL){
        return;
    }
    uint uiSize = pdu->uiMsgLen/32; // get size of message
    char tmp[32];
    for (int i=0;i<uiSize;i++){
        memcpy(tmp, (char*)(pdu->caMsg)+i*32, 32);
        ui->listWidget->addItem(tmp); // update ui
    }
}

// add friend
void Online::on_pushButton_clicked()
{
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    if (pItem == NULL){
        return;
    }
    QString strName = pItem->text(); // the user to be added as a friend, who receives the add request
    QString strMyLoginName = TcpClient::getInstance().getMyLoginName(); // login user, who sends the add request
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    // the first one is my name, and the second one is his name
    memcpy(pdu->caData, strMyLoginName.toStdString().c_str(), strMyLoginName.size());
    memcpy(pdu->caData+32, strName.toStdString().c_str(), strName.size());

    // test
    // qDebug()<<"In online cpp" << pdu->uiMsgType;
    // for (int i = 0; i < 64; ++i) {
    //     qDebug() << pdu->caData[i];
    // }

    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

