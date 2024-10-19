#include "File.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent)
    : QWidget{parent}
{
    m_pList = new QListWidget();
    m_pBackPB = new QPushButton("Back");
    m_pCreateDirPB = new QPushButton("Create Directory");
    m_pDeleteDirPB = new QPushButton("Delete Directory");
    m_pRenamePB = new QPushButton("Rename Directory");
    m_pFlushDirPB = new QPushButton("Flush Directory");

    QVBoxLayout *pDirVBL = new QVBoxLayout();
    pDirVBL->addWidget(m_pBackPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDeleteDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushDirPB);

    m_pUplaodFilePB = new QPushButton("Upload File");
    m_pDownloadFilePB = new QPushButton("Downlaod File");
    m_pDeleteFilePB = new QPushButton("Delete File");
    m_pShareFileFilePB = new QPushButton("Share File");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUplaodFilePB);
    pFileVBL->addWidget(m_pDownloadFilePB);
    pFileVBL->addWidget(m_pDeleteFilePB);
    pFileVBL->addWidget(m_pShareFileFilePB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pList);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool))
            , this, SLOT(createDir()));
}

void File::createDir()
{
    QString newDirName = QInputDialog::getText(this, "Create a new folder", "Enter new folder name:"); // input new folder name
    if (newDirName.isEmpty()){
        QMessageBox::warning(this, "Create folder", "Please enter folder name");
    }
    if (newDirName.size()>32){
        QMessageBox::warning(this, "Create folder", "Folder name too long"); // size no longer than 32
    }
    QString myName = TcpClient::getInstance().getMyLoginName();
    QString curPath = TcpClient::getInstance().getCurrentPath();
    PDU *pdu = mkPDU(curPath.size()+1); // +1 is for '\0'
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_FOLDER_REQUEST;
    // new dir name and username is stored in caData, while current path is stored in caMsg
    strncpy(pdu->caData, myName.toStdString().c_str(), myName.size());
    strncpy(pdu->caData+32, newDirName.toStdString().c_str(), newDirName.size());
    memcpy(pdu->caMsg, curPath.toStdString().c_str(), curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
