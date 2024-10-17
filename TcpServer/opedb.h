#ifndef OPEDB_H
#define OPEDB_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    ~OpeDB();

    static OpeDB &getInstance();

    // init db connection
    void init();
    // register a new user account
    bool registerAccount(const char* username, const char* password);
    // login
    bool login(const char* username, const char* password);
    // login out
    bool logout(const char* username);
    // get all online users
    QStringList getAllOnlineUsers();
    // get a user by name
    int searchUser(const char* name);

signals:

private:
    QSqlDatabase m_db; //connect to database
};

#endif // OPEDB_H
