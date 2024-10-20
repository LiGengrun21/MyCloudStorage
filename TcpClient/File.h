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

signals:

public slots:
    void createDir();
    void flushFDir();
    void deleteDir();
    void renameDir();
    void enterDir(const QModelIndex &index);
    void backDir();

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
    QPushButton *m_pShareFileFilePB;

    QString m_strEnterDir; // for entering dir. Note: there is another dir string stored in Tcp Client but they are different
};

#endif // FILE_H
