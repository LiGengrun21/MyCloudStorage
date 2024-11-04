#include "sharefile.h"
#include "tcpclient.h"
#include "opewidget.h"

ShareFile::ShareFile(QWidget *parent)
    : QWidget{parent}
{
    m_pSelectAllPB = new QPushButton("Select All");
    m_pCancelSelectPB = new QPushButton("Cancel Selected");

    m_pOKPB = new QPushButton("Ok");
    m_pCancelPB = new QPushButton("Cancel");

    m_pSA = new QScrollArea();

    m_pFriendW = new QWidget;

    m_pFriendListVBL = new QVBoxLayout(m_pFriendW);

    m_pButtonGroup = new QButtonGroup(m_pFriendW); // add friend widget into button group
    m_pButtonGroup->setExclusive(false);

    QHBoxLayout *pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pBottomHBL=new QHBoxLayout;
    pBottomHBL->addWidget(m_pOKPB);
    pBottomHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addWidget(m_pSA);
    pMain->addLayout(pBottomHBL);

    setLayout(pMain);

    connect(m_pCancelSelectPB, SIGNAL(clicked(bool))
            , this, SLOT(cancelSelected()));
    connect(m_pSelectAllPB, SIGNAL(clicked(bool))
            , this, SLOT(selectAll()));
    connect(m_pOKPB, SIGNAL(clicked(bool))
            , this, SLOT(okShare()));
    connect(m_pCancelPB, SIGNAL(clicked(bool))
            , this, SLOT(cancelShare()));
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if (pFriendList==NULL){
        return;
    }
    QAbstractButton* tmp; // point to the item to be deleted
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons(); // previous friend list
    // remove the existed friend list
    for(int i=0;i<preFriendList.size();i++){
        tmp = preFriendList[i];
        m_pFriendListVBL->removeWidget(tmp); // remove it from widget
        m_pButtonGroup->removeButton(tmp); // remove it from button group
        preFriendList.removeOne(tmp); // remove it from the list
        delete tmp;
        tmp=NULL;
    }

    // make a new friend list
    QCheckBox *pCB=NULL;
    for(int i=0;i<pFriendList->count();i++){
        //test
        qDebug()<<i<<": "<<pFriendList->item(i)->text();
        pCB=new QCheckBox(pFriendList->item(i)->text());
        m_pFriendListVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancelSelected()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.count();i++){
        if (cbList[i]->isChecked()){
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    for(int i=0;i<cbList.count();i++){
        if (!cbList[i]->isChecked()){
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare()
{
    // shared by
    QString strName = TcpClient::getInstance().getMyLoginName();
    // path and file name
    QString strCurPath = TcpClient::getInstance().getCurrentPath();
    QString strShareFileName = OpeWidget::getInstance().getFile()->getShareFileName();
    // full path
    QString strPath = strCurPath+"/"+strShareFileName;
    // receiver
    QList<QAbstractButton*> cbList = m_pButtonGroup->buttons();
    int num = 0; // number of people selected
    for(int i=0;i<cbList.count();i++){
        if (cbList[i]->isChecked()){
            num++;
        }
    }

    // receivers and file path are stored in caMsg. 32 is one person
    // shared by and number of people to share with are stored in caData
    PDU* pdu = mkPDU(32*num + strPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData, "%s %d", strName.toStdString().c_str(), num);
    int j=0; // selected persons
    // copy receiver's names into caMsg
    for(int i=0;i<cbList.count();i++){
        if (cbList[i]->isChecked()){
            memcpy((char*)(pdu->caMsg)+32*j, cbList[i]->text().toStdString().c_str(), cbList[i]->text().size());
            j++;
        }
    }
    // copy file path into caMsg
    memcpy((char*)(pdu->caMsg)+32*num, strPath.toStdString().c_str(), strPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void ShareFile::cancelShare()
{
    hide();
}
