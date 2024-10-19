#include "opewidget.h"
#include "tcpclient.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle(TcpClient::getInstance().getMyLoginName()); // replace "TcpClient" with the actual client name

    m_pListWidget=new QListWidget(this); // this is the parent of m_pListWidget. If the parent is destroyed, it will also destroy QListWidget.
    m_pListWidget->addItem("Friends");
    m_pListWidget->addItem("Files");

    m_pFriend = new Friend;
    m_pBook = new File;
    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListWidget);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    // click item in list widget and switch to the corresponding widget, book or firend
    connect(m_pListWidget, SIGNAL(currentRowChanged(int))
            , m_pSW, SLOT(setCurrentIndex(int)));
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}
