#include "online.h"
#include "ui_online.h"

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
