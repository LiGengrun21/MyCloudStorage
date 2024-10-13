#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSocket, SIGNAL(connected()) // signal sender and what signals it sends
            , this, SLOT(showConnect())); // signal receirver and which function to process the signal
    m_tcpSocket.connectToHost(QHostAddress(m_StrIP), m_usPort); // connect to the server
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly)){
        QByteArray data =  file.readAll();
        QString strData = data.toStdString().c_str();
        file.close();
        // replace and split to get ip and port
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        // for(int i=0;i<strList.size();i++){
        //     qDebug() <<  strList[i];
        // }
        m_StrIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP:" << m_StrIP << "port:" << m_usPort;
    }
    else{
        QMessageBox::critical(this,"load this config","Config loading failed");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"connect to server","Connection is successful");
}
