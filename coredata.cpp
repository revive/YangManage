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
    personModel = new QSqlTableModel(0, db);
    setPersonModel();
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

int CoreData::getMaxPersonId()
{
    QSqlQuery query("SELECT max(id) FROM people");
    int maxId = 0;
    if (query.next()) {
        maxId = query.value(0).toInt();
    }
    return maxId;
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

