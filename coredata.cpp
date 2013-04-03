#include "coredata.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>

#include <QDebug>

CoreData::CoreData()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    personModel = 0;
    signInModel = 0;
}

CoreData::~CoreData()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool CoreData::createDBFile(QString dbname)
{
    if (QFile::exists(dbname)) {
        QFile::remove(dbname);
    }
    db.setDatabaseName(dbname);
    if (!db.open()) {
        QMessageBox::critical(0, QString::fromUtf8("出错了！"), QString::fromUtf8("无法打开数据库文件"));
        return false;
    }

    QSqlQuery query;
    query.exec("drop table people ");
    query.exec("create table people (id int primary key, name varchar(64), gender char(8), join_date date, status tinyint, comment text)");
    qDebug() << db.lastError().type()  << " + " << db.lastError().text();
    query.exec("drop table date");
    query.exec("create table date (id int primary key, date date)");
    query.exec("pragma synchronous=0");
    qDebug() << db.lastError().type()  << " + " << db.lastError().text();
    QDate date = QDate::currentDate();
    date = date.addYears(-1);
    query.prepare("insert into date (id, date) values (?,?)");
    int i = 0;
    QVariantList ids;
    QStringList dateStrings;

    for (i=0; i<3000; ++i) {
        ids<<i;
        date = date.addDays(1);
        dateStrings<<date.toString("yyyy-MM-dd");
    }
    query.addBindValue(ids);
    query.addBindValue(dateStrings);
    if (!query.execBatch())
         qDebug() << query.lastError();

    query.exec("pragma synchronous=2");
    qDebug()<<query.lastError();
    query.exec("create table person_date (id int primary key, person_id integer, date_id integer, foreign key(person_id) references people(id), foreign key(date_id) references date(id))");
    qDebug() << query.lastError();
    personModel = new QSqlTableModel(0, db);
    setPersonModel();
    signInModel = new SignInModel(0);
    setSignInModel(QDate::currentDate(), db);

    query.exec("create table contact (id int primary key, person_id integer, method varchar(64), value varchar(128), foreign key(person_id) references people(id))");
    qDebug() << query.lastError();
    return true;
}

bool CoreData::openDBFile(QString dbname)
{
    db.setDatabaseName(dbname);
    if (!db.open()) {
        QMessageBox::critical(0, QString::fromUtf8("出错了！"), QString::fromUtf8("无法打开数据库文件"));
        return false;
    }
    personModel = new QSqlTableModel(0, db);
    setPersonModel();
    signInModel = new SignInModel(0);
    setSignInModel(QDate::currentDate(), db);
    return true;
}

QSqlDatabase *CoreData::getDataBase()
{
    return &db;
}

QSqlTableModel *CoreData::getPersonModel()
{
    return personModel;
}

SignInModel *CoreData::getSignInModel()
{
    return signInModel;
}

int CoreData::getMaxPersonId()
{
    QSqlQuery query("SELECT max(id) FROM people");
    int maxId = 0;
    if (query.next()) {
        maxId = query.value(0).toInt();
    }
    return maxId;
}

void CoreData::setSignInModel(const QDate &date, QSqlDatabase &db)
{
    QString str = date.toString("yyyy-MM-dd");
    signInModel->init(str, db);
}

void CoreData::setSignInDate(const QDate &date)
{
    signInModel->setDate(date.toString("yyyy-MM-dd"));
}

void CoreData::setPersonModel()
{
    personModel->setTable("people");
    personModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    personModel->select();
    personModel->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("编号"));
    personModel->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("姓名"));
    personModel->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("性别"));
    personModel->setHeaderData(3, Qt::Horizontal, QString::fromUtf8("入会日期"));
    personModel->setHeaderData(4, Qt::Horizontal, QString::fromUtf8("状态"));
    personModel->setHeaderData(5, Qt::Horizontal, QString::fromUtf8("备注"));
}

