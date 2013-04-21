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

SignStats *CoreData::findStats(QString key)
{
    SignStats * stats = statsReports.value(key);
    return stats;
}

void CoreData::addStats(SignStats *stats)
{
    statsReports.insert(stats->getKey(), stats);
}

SignStats * CoreData::addStats(QDate &sdate, QDate &edate)
{
    SignStats * stats = new SignStats(sdate, edate);
    QString sdate_str = sdate.toString("yyyy-MM-dd");
    QString edate_str = edate.toString("yyyy-MM-dd");
    qDebug()<<"start date: " << sdate_str;
    qDebug()<<"end date: " << edate_str;
    QString query_str;
    query_str = QString("select count(*) from person_date join date on person_date.date_id=date.id where date.date>='%1' and date.date<='%2'").arg(sdate_str).arg(edate_str);
    qDebug()<<query_str;
    QSqlQuery squery;
    squery.exec(query_str);
    squery.next();
    int npt = 0;
    if (!squery.value(0).isNull()) {
       npt = squery.value(0).toInt();
       qDebug()<<"person time in period: " << npt;
    }
    stats->setTotalPersonTime(npt);

    query_str = QString("select count(distinct person_id) from person_date join date on person_date.date_id=date.id where date.date>='%1' and date.date<='%2'").arg(sdate_str).arg(edate_str);
    squery.exec(query_str);
    squery.next();
    int np = 0;
    if (!squery.value(0).isNull()) {
        np = squery.value(0).toInt();
        qDebug()<<"person in period: " << np;
    }
    stats->setTotalPerson(np);

    query_str = QString("select count(d),d from (select count(*) as d from person_date join date on person_date.date_id=date.id where date.date>='%1' and date.date<='%2' group by person_id) group by d").arg(sdate_str).arg(edate_str);
    squery.exec(query_str);
    int count = 0;
    npt = 0;
    while (squery.next()) {
        count = squery.value(0).toInt();
        npt = squery.value(1).toInt();
        qDebug()<<npt<<" -> "<<count;
        stats->addPersonTimeCount(npt, count);
    }

    query_str = QString("select count(*) as d, name from person_date join date on person_date.date_id=date.id join people on person_date.person_id = people.id where date.date>='%1' and date.date<='%2' group by person_id order by d desc limit 5").arg(sdate_str).arg(edate_str);
    squery.exec(query_str);
    QString name;
    while(squery.next()) {
        count = squery.value(0).toInt();
        name = squery.value(1).toString();
        stats->addTopPersonTime(count, name);
        qDebug() << name << " -> "<< count;
    }
    statsReports.insert(stats->getKey(), stats);

    return stats;
}

void CoreData::setSignInDate(const QDate &date)
{
    signInModel->setDate(date.toString("yyyy-MM-dd"));
}

void CoreData::refreshModel()
{
    signInModel->refresh();
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

