#include "tcpserver.h"
#include "./ui_tcpserver.h"
#include "mytcpserver.h"
#include <QFile>
#include <QMessageBox>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    // listen
    MyTcpServer::getInstance().listen(QHostAddress(m_StrIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly)){
        QByteArray data =  file.readAll();
        QString strData = data.toStdString().c_str();
        file.close();
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        m_StrIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP:" << m_StrIP << "port:" << m_usPort;
    }
    else{
        QMessageBox::critical(this,"load this config","Config loading failed");
    }
}
