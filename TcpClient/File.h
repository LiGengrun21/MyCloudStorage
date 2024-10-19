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

signals:

public slots:
    void createDir();

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
};

#endif // FILE_H
