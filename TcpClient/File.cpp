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
    connect(m_pFlushDirPB, SIGNAL(clicked(bool))
            , this, SLOT(flushFDir()));
    connect(m_pDeleteDirPB, SIGNAL(clicked(bool))
            , this, SLOT(deleteDir()));
}

void File::updateFileList(const PDU *pdu)
{
    if (NULL==pdu){
        return;
    }
    m_pList->clear(); // clear the list first
    // get file name and type from pdu
    FileInfo* pFileInfo = NULL;
    int fileCount = pdu->uiMsgLen/sizeof(FileInfo);
    for (int i=2;i<fileCount;i++){ // we don't want . and .. so starting from i=2
        pFileInfo=(FileInfo*)(pdu->caMsg)+i;
        //qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        // set icon for widget item
        if (0==pFileInfo->iFileType){
            pItem->setIcon(QIcon(QPixmap(":/images/folder.png")));
        }
        else{
            pItem->setIcon(QIcon(QPixmap(":/images/file.png")));
        }
        pItem->setText(pFileInfo->caFileName); // set file name
        m_pList->addItem(pItem);
    }

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

// send path
void File::flushFDir()
{
    // get current dir path
    QString curPath = TcpClient::getInstance().getCurrentPath();
    // store path to caMsg
    PDU* pdu = mkPDU(curPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FOLDER_REQUEST;
    strncpy((char*)pdu->caMsg, curPath.toStdString().c_str(), curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

// send path and file name
void File::deleteDir()
{
    // get current dir path
    QString curPath = TcpClient::getInstance().getCurrentPath();
    // get dir name
    QListWidgetItem *pItem = m_pList->currentItem();
    if (NULL==pItem){
        QMessageBox::information(this, "Delete a folder", "Please select a folder to delete!");
        return;
    }
    QString strFileName = pItem->text();
    PDU* pdu = mkPDU(curPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FOLDER_REQUEST;
    // caMsg stores path and caData stores file name
    strncpy(pdu->caData, strFileName.toStdString().c_str(), strFileName.size());
    strncpy((char*)pdu->caMsg, curPath.toStdString().c_str(), curPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
