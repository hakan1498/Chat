#include "sqlitedb.h"

SQLiteDB::SQLiteDB(QObject *parent) : QObject(parent)
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");

    QString pathToDB = QString("/Users/antmakar/Projects/Qt/mydatabase.sqlite");
    myDB.setDatabaseName(pathToDB);

    QFileInfo checkFile(pathToDB);
    if (checkFile.isFile())
        if (myDB.open())
            qDebug() << "[+] Connected to Database File";

        else
            qDebug() <<"[!] Database File does not exist";
}


SQLiteDB::~SQLiteDB()
{
    qDebug() << "Closing the connection to Database file on exist";
    myDB.close();
}

void SQLiteDB::AddContact(QString UserName, QString Sex, int Age, QString City, QString Pas, QString PubK, QString Salt)
{
    QSqlQuery query(myDB);
    query.prepare("INSERT INTO Users (UserName, Sex, Age, City, Password, PubKey, Salt, OnlineStatus) "
                  "VALUES (:UserName, :Sex, :Age, :City, :Password, :Pub, :Salt, :OnlineStatus)");
    query.bindValue(":UserName", UserName);
    query.bindValue(":Sex", Sex);
    query.bindValue(":Age", Age);
    query.bindValue(":City", City);
    query.bindValue(":Password", Pas);
    query.bindValue(":Pub", PubK);
    query.bindValue(":Salt", Salt);
    query.bindValue(":OnlineStatus", QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    query.exec();

    QSqlQuery queryCreate(myDB);
    queryCreate.exec("CREATE TABLE Friend" + UserName + " (name text NOT NULL, sex text NOT NULL, key text NOT NULL)");
}

QString SQLiteDB::FindInDB(QString UserName, QString whoFind)
{
    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Sex, PubKey, OnlineStatus, EmailPhone, LiveStatus FROM Users WHERE UserName=\'" +UserName+ "\'"))
        if(query.next())
            if (query.value(0).toString()==UserName)
            {
                if(!whoFind.isEmpty())
                {
                    QSqlQuery queryAdFr(myDB);
                    queryAdFr.prepare("INSERT INTO Friend"+whoFind+" (name, sex, key) VALUES (:UserName, :Sex, :Key)");
                    queryAdFr.bindValue(":UserName", UserName);
                    queryAdFr.bindValue(":Sex", query.value(1).toString());
                    queryAdFr.bindValue(":Key", query.value(2).toString());
                    queryAdFr.exec();
                }
                return query.value(1).toString() + " /s " +
                       query.value(2).toString() + " /s " +
                       query.value(3).toString() + " /s " +
                       query.value(4).toString() + " /s " +
                       query.value(5).toString();
            }

    query.exec();
    return "false";
}

QString SQLiteDB::CheckPass(QString pass)
{
    QCryptographicHash CalculateMD5(QCryptographicHash::Md5);
    QByteArray in;
    in.append(pass);

    CalculateMD5.addData(in);
    QByteArray res = CalculateMD5.result().toHex();
    QString checkpass = QString::fromUtf8(res.constData());
    return checkpass;
}

QString SQLiteDB::CorrectInput(QString _login, QString _password)
{
    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Password, PubKey, Salt FROM Users WHERE UserName=\'" +_login+ "\'"))
        if(query.next())
            if (query.value(0).toString() == _login)
            {
                QString Salt = query.value(3).toString();
                QString _checkpass = CheckPass(_password + Salt);

                if(_checkpass == query.value(1).toString())
                    return query.value(2).toString();
                else return "false";
            }

    query.exec();
    return "false";
}

QList <QPair<QString, QString>> SQLiteDB::FriendList(QString user)
{
    QSqlQuery query(myDB);
    QList <QPair <QString, QString>> FriendSex;

    if(query.exec("SELECT name, sex FROM Friend" + user))
        while (query.next())
        {
            FriendSex.push_back(qMakePair(query.value(0).toString(), query.value(1).toString()));
        }

    return FriendSex;
}

QString SQLiteDB::getChatHistory(QString user)
{
    QSqlQuery queryUser(myDB);
    QSqlQuery queryChat(myDB);
    QString chatHistory;

    if(queryUser.exec("SELECT name FROM Friend" + user))
        while (queryUser.next())
        {
            if(queryChat.exec("SELECT Message, Who, Time FROM Chat" + user + queryUser.value(0).toString()))
            {
                if (queryChat.next())
                {
                    chatHistory.append(" //s " + queryUser.value(0).toString());
                    chatHistory.append(" /pm " + queryChat.value(1).toString() + " /s " + queryChat.value(0).toString() + " /s " + queryChat.value(2).toString());
                }
                while (queryChat.next())
                {
                    chatHistory.append(" /pm " + queryChat.value(1).toString() + " /s " + queryChat.value(0).toString() + " /s " + queryChat.value(2).toString());
                }
            }
        }

    return chatHistory;
}

QList <QPair <QString, QString> > SQLiteDB::FriendKeys(QString user)
{
    QSqlQuery query(myDB);
    QList <QPair <QString, QString>> FriendKey;

    if(query.exec("SELECT name, key FROM Friend" + user))
        while (query.next())
        {
            FriendKey.push_back(qMakePair(query.value(0).toString(), query.value(1).toString()));
        }

    return FriendKey;
}

void SQLiteDB::addChatTable(QString who, QString find)
{
    QSqlQuery queryCreate(myDB);
    queryCreate.exec("CREATE TABLE Chat" + who + find + " (Message text NOT NULL, Who text NOT NULL, Time text NOT NULL)");
}

void SQLiteDB::createGroup(QString group_name, QString group_description, QStringList user_list)
{
    QSqlQuery queryCreate(myDB);
    group_name = group_name.simplified();
    group_name.replace(" ", "_");

    queryCreate.exec("CREATE TABLE Group" + group_name + " (User text NOT NULL)");

    queryCreate.prepare("INSERT INTO Group" + group_name +  " (User) VALUES (:User)");
    queryCreate.bindValue(":User", group_description);
    queryCreate.prepare("INSERT INTO Group" + group_name +  " (User) VALUES (:User)");
    queryCreate.bindValue(":User", "-----------------");

    for(int i=0; i<user_list.size(); i++)
    {
        queryCreate.prepare("INSERT INTO Group" + group_name +  " (User) VALUES (:User)");
        queryCreate.bindValue(":User", user_list.at(i));
    }
}

void SQLiteDB::addMessInChat(QString who, QString find, QString message, QString log)
{
    QSqlQuery query(myDB);
    query.prepare("INSERT INTO Chat" + who + find +  " (Message, Who, Time) VALUES (:Mes, :Who, :Time)");
    query.bindValue(":Mes", message);
    query.bindValue(":Who", log);
    query.bindValue(":Time", QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
    query.exec();
}

void SQLiteDB::ClearHistory(QString from, QString to)
{
    QSqlQuery query(myDB);

    query.prepare("DELETE FROM Chat" + from + to);
    if(!query.exec())
        qDebug() << query.lastError();
}


void SQLiteDB::delFriend(QString from, QString delfriend)
{
    QSqlQuery query(myDB);

    // Удалении происходит только на "исходном" клиенте. Удаляемый пользователь потом может написать тому, кто удалил, и у того
    // вновь добавится человек, которого удалил. СложнА :))

    query.prepare("DELETE FROM Friend" + from +  " WHERE name=\'" +delfriend+ "\'");
    if(!query.exec())
        qDebug() << query.lastError();

}

void SQLiteDB::UpOnlineStatus(const QString &status, const QString &user_name)
{
    QSqlQuery query(myDB);

    if(status=="Online")
        query.prepare("UPDATE Users SET OnlineStatus='Online' WHERE UserName='" + user_name + "'");

    else if (status=="Offline")
        query.prepare(QString("UPDATE Users SET OnlineStatus='%1' WHERE UserName='" + user_name + "'").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm")));

    query.exec();
}

void SQLiteDB::getOnlineStatus(const QString & user_name, PairStringList &status, QStringList &StatusForFriends)
{
    QSqlQuery query_search(myDB);
    QSqlQuery query_add(myDB);
    if(query_search.exec("SELECT name FROM Friend" + user_name))
        while (query_search.next())
        {
            query_add.exec("SELECT UserName, OnlineStatus, EmailPhone, LiveStatus FROM Users WHERE UserName=\'" + query_search.value(0).toString() + "\'");
            query_add.next();
            status.push_back(qMakePair(query_add.value(3).toString() + " _ " + query_add.value(1).toString(), query_add.value(2).toString()));
            StatusForFriends.push_back(query_search.value(0).toString());
        }
}

void SQLiteDB::updateAllDataOfUser(QStringList dataset)
{
    QSqlQuery query(myDB);
    qDebug() << dataset;
    QString q = QString("UPDATE Users SET FirstName='%1', LastName='%2', EmailPhone='%3', Sex='%4', ").arg(dataset[1]).arg(dataset[2]).arg(dataset[3]).arg(dataset[4]);
    q.append(QString("Age='%1', City='%2', LiveStatus='%3' WHERE UserName='%4'").arg(dataset[5]).arg(dataset[6]).arg(dataset[7]).arg(dataset[0]));

    query.prepare(q);
    query.exec();
}

QString SQLiteDB::getFullUserInformations(QString userName)
{
    QString uInfo = "INFP";
    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Sex, Age, City, OnlineStatus, EmailPhone, LiveStatus, FirstName, LastName FROM Users WHERE UserName=\'" +userName+ "\'"))
        if(query.next())
            if (query.value(0).toString() == userName)
            {
                uInfo.append(query.value(1).toString() + " /s " +
                             query.value(2).toString() + " /s " +
                             query.value(3).toString() + " /s " +
                             query.value(4).toString() + " /s " +
                             query.value(5).toString() + " /s " +
                             query.value(6).toString() + " /s " +
                             query.value(7).toString() + " /s " +
                             query.value(8).toString() + " /s " );

                return uInfo;
            }

    query.exec();
    return "INFN";
}


QStringList SQLiteDB::UserData(QString name)
{
    QStringList uList;

    QSqlQuery query(myDB);
    if(query.exec("SELECT UserName, Sex, Age, City FROM Users WHERE UserName=\'" +name+ "\'"))
        if(query.next())
            if (query.value(0).toString() == name)
            {
                uList.push_back(name);
                uList.push_back(query.value(1).toString());
                uList.push_back(query.value(2).toString());
                uList.push_back(query.value(3).toString());
                return uList;
            }

    query.exec();
    return uList;
}

void SQLiteDB::FriendListName(QString username, QStringList &user_list)
{
    QSqlQuery query(myDB);

    if(query.exec("SELECT name FROM Friend" + username))
        while (query.next())
        {
            user_list.push_back(query.value(0).toString());
        }
}
