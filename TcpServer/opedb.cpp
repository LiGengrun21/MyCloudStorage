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
            //qDebug() << data;
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
    //qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if (query.next()){ // set online to 1
        data = QString("update userinfo set online=1 where name=\'%1\'").arg(username);
        //qDebug()<<data;
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

QStringList OpeDB::getAllOnlineUsers()
{
    QString data = QString("select name from userInfo where online=1");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next()){
        result.append(query.value(0).toString()); // value(0) get QVariant, so it should be casted to QString; 0 because only name is returned in the query result.
    }
    return result;
}

int OpeDB::searchUser(const char *name)
{
    if (name == NULL){
        return -1;
    }
    QString data = QString("select online from userInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if (query.next()){
        return query.value(0).toInt(); // 1: online; 0: offline
    }
    else{
        return -1;
    }
}

// return values: -1, 0, 1, 2, 3
int OpeDB::addFriend(const char *myName, const char *hisName)
{
    if (myName==NULL || hisName == NULL || strcmp(myName, hisName) == 0){
        return -1; // input error
    }
    QString data = QString("select * from friend where (id = (select id from userInfo where name = \'%1\') and friendId = (select id from userInfo where name = \'%2\')) "
                           "or (id = (select id from userInfo where name = \'%3\') and friendId = (select id from userInfo where name = \'%4\'))").arg(myName).arg(hisName).arg(hisName).arg(myName);
    //qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if (query.next()){
        return 0; // they are aleady friends
    }
    else{ // not friends
        // whether the request receiver is online or not, online ==1 or 0.
        int online = searchUser(hisName);
        if (online == 0){
            return 2; // not online
        }
        else if (online == 1){
            return 1; // online
        }
        else{
            return 3; // the user does not exist
        }
    }
}

bool OpeDB::insertFriend(const char *name1, const char *name2)
{
    int userId1 = getIdByName(name1);
    int userId2 = getIdByName(name2);
    if (userId1<=0 || userId2<=0){
        return false;
    }
    QString data = QString("insert into friend (id, friendId) values (%1,%2);").arg(userId1).arg(userId2);
    QSqlQuery query;
    return query.exec(data);
}

int OpeDB::getIdByName(const char *name)
{
    if (name==NULL){
        return -1;
    }
    QString data = QString("select id from userInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if (query.next()){
        return query.value(0).toInt();
    }
    else{
        return -1;
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}
