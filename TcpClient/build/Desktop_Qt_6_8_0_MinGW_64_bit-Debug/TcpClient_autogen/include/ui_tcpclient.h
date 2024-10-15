/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *username_lb;
    QLineEdit *username_le;
    QHBoxLayout *horizontalLayout_2;
    QLabel *password_lbl;
    QLineEdit *password_le;
    QVBoxLayout *verticalLayout;
    QPushButton *login_btn;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *register_btn;
    QPushButton *close_btn;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName("TcpClient");
        TcpClient->resize(1069, 600);
        QFont font;
        font.setPointSize(20);
        TcpClient->setFont(font);
        widget = new QWidget(TcpClient);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(170, 90, 701, 411));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        username_lb = new QLabel(widget);
        username_lb->setObjectName("username_lb");
        QFont font1;
        font1.setPointSize(18);
        username_lb->setFont(font1);

        horizontalLayout->addWidget(username_lb);

        username_le = new QLineEdit(widget);
        username_le->setObjectName("username_le");

        horizontalLayout->addWidget(username_le);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        password_lbl = new QLabel(widget);
        password_lbl->setObjectName("password_lbl");
        password_lbl->setFont(font1);

        horizontalLayout_2->addWidget(password_lbl);

        password_le = new QLineEdit(widget);
        password_le->setObjectName("password_le");

        horizontalLayout_2->addWidget(password_le);


        verticalLayout_2->addLayout(horizontalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        login_btn = new QPushButton(widget);
        login_btn->setObjectName("login_btn");
        login_btn->setFont(font1);

        verticalLayout->addWidget(login_btn);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        register_btn = new QPushButton(widget);
        register_btn->setObjectName("register_btn");
        QFont font2;
        font2.setPointSize(12);
        register_btn->setFont(font2);

        horizontalLayout_3->addWidget(register_btn);

        close_btn = new QPushButton(widget);
        close_btn->setObjectName("close_btn");
        close_btn->setFont(font2);

        horizontalLayout_3->addWidget(close_btn);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        username_lb->setText(QCoreApplication::translate("TcpClient", "username", nullptr));
        password_lbl->setText(QCoreApplication::translate("TcpClient", "password", nullptr));
        login_btn->setText(QCoreApplication::translate("TcpClient", "Login", nullptr));
        register_btn->setText(QCoreApplication::translate("TcpClient", "Register a new account", nullptr));
        close_btn->setText(QCoreApplication::translate("TcpClient", "close an account", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
