#ifndef FILE_H
#define FILE_H
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "protocol.h"

class File : public QWidget
{
    Q_OBJECT
public:
    explicit File(QWidget *parent = nullptr);
    void updateFileList(const PDU* pdu); // update files in the path

    // getter and setter
    void setDownload(bool status);
    bool getDownload();
    QString getSaveFilePath();
    QString getShareFileName();

    qint64 m_iTotal; // file size
    qint64 m_iReceived; // how many received so far

signals:

public slots:
    void createDir();
    void flushFDir();
    void deleteDir();
    void renameDir();
    void enterDir(const QModelIndex &index);
    void backDir();
    void uploadFile();
    void uploadFileSendData();
    void deleteFile();
    void downloadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();

private:
    QListWidget *m_pList;
    QPushButton *m_pBackPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDeleteDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushDirPB;
    QPushButton *m_pUplaodFilePB;
    QPushButton *m_pDownloadFilePB;
    QPushButton *m_pDeleteFilePB;
    QPushButton *m_pShareFilePB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;

    QString m_strEnterDir; // for entering dir. Note: there is another dir string stored in Tcp Client but they are different
    QString m_strUploadFilePath; // store the path of uploading file
    QTimer *m_pTimer;
    QString m_strSaveFilePath; // where to save the downloaded file
    bool m_bDownload; // state of dowlloading file
    QString m_strShareFileName; // the file to share
    QString m_strMoveFileName; // the file to move
    QString m_strMoveFilePath;
    QString m_strDestDir;
};

#endif // FILE_H
