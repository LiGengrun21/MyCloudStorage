#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void loadConfig();

    // TcpClient is a singleton, because tcp socket needs to be used in other place, like friend widget, to send messages.
    static TcpClient &getInstance();

    // get m_tcpSocket, to be used in anywhere
    QTcpSocket &getTcpSocket();

    // getter
    QString getMyLoginName();
    QString getCurrentPath();

    // setter
    void setCurPath(QString path);

public slots:
    void showConnect();
    void recvMsg();

private slots:
    // void on_sendButton_clicked();

    void on_login_btn_clicked();

    void on_register_btn_clicked();

private:
    Ui::TcpClient *ui;
    QString m_StrIP;
    quint16 m_usPort;
    QTcpSocket m_tcpSocket; // to connect server
    QString m_strMyLoginName; // login name of the user
    QString m_strCurPath; // current path of user's folder
    QFile m_file; // the file to open and read from server, for downloading function
};
#endif // TCPCLIENT_H
