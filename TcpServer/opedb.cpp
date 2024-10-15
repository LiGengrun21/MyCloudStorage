#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE"); // db is sqlite3
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D://C++//Cloud Drive//TcpServer//cloud.db");
    if (m_db.open()){
        QSqlQuery query;
        query.exec("select * from userInfo");
        while (query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else{
        QMessageBox::critical(NULL,"Open the database","failed to open the database");
    }
}

bool OpeDB::registerAccount(const char *username, const char *password)
{
    if (username ==NULL || password==NULL){
        qDebug() << "username or password is null";
        return false;
    }
    QString data = QString("insert into userInfo(name,password) values (\'%1\',\'%2\')").arg(username).arg(password); // type is varchar, so '' is needed
    //qDebug()<<data;
    QSqlQuery query;
    return query.exec(data);
}

bool OpeDB::login(const char *username, const char *password)
{
    if (username ==NULL || password==NULL){
        qDebug() << "username or password is null";
        return false;
    }
    QString data = QString("select * from userInfo where name=\'%1\' and password=\'%2\' and online=0").arg(username).arg(password); // online = 0 means not logined
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if (query.next()){ // set online to 1
        data = QString("update userinfo set online=1 where name=\'%1\'").arg(username);
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else{
        return false;
    }
}

bool OpeDB::logout(const char *username)
{
    if (username ==NULL){
        qDebug() << "username is null";
        return false;
    }
    QString data = QString("update userinfo set online=0 where name=\'%1\'").arg(username);
    QSqlQuery query;
    query.exec(data);
    return true;
}

OpeDB::~OpeDB()
{
    m_db.close();
}
