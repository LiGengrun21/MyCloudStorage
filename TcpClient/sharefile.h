#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QListWidget>
#include <QCheckBox>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);

    static ShareFile &getInstance();

    void updateFriend(QListWidget *pFriendList);

signals:

public slots:
    void cancelSelected();
    void selectAll();
    void okShare();
    void cancelShare();

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;

    QScrollArea *m_pSA;

    QWidget *m_pFriendW;

    QButtonGroup *m_pButtonGroup;

    QVBoxLayout *m_pFriendListVBL;
};

#endif // SHAREFILE_H
