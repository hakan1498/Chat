#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include <QPair>

typedef QList <QPair <QString, QList<QPair <QString, QString> > > > ChatListVector;
typedef QList <QPair <QString, QString>> PairStringList;

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(QObject *parent = 0);
    ~SQLiteDB();

private:
    QSqlDatabase myDB;
    QString CheckPass(QString);

public slots:
    void AddContact(QString, QString, int, QString, QString, QString, QString);
    void addMessInChat(QString, QString, QString, QString);
    QList <QPair <QString, QString> > FriendList(QString);
    QList <QPair <QString, QString> > FriendKeys(QString);
    QString getChatHistory(QString user);
    void createGroup(QString , QString , QStringList);
    void addChatTable(QString, QString);
    QString FindInDB(QString, QString);
    void ClearHistory(QString, QString);
    void delFriend(QString, QString);
    void UpOnlineStatus(const QString&, const QString&);
    void updateAllDataOfUser(QStringList);
    void getOnlineStatus(const QString&, PairStringList &, QStringList &);
    QString getFullUserInformations(QString);
    QString CorrectInput(QString, QString);
    QStringList UserData(QString);
    void  FriendListName(QString, QStringList &);
};

#endif // SQLITEDB_H
