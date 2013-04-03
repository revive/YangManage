#include "signinmodel.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QStandardItem>
#include <QDebug>


SignInModel::SignInModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

Qt::ItemFlags SignInModel::flags(const QModelIndex &index) const
{

    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if ( index.column() == 4 ) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}

QVariant SignInModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 4 && role == Qt::CheckStateRole) {
        QModelIndex dindex = QSqlQueryModel::index(index.row(), 2);
        QVariant value = QSqlQueryModel::data(dindex);
        bool ok = true;
        int date_id = value.toInt(&ok);
        if (ok && date_id!=0) {
            return Qt::Checked;
        } else {
            return Qt::Unchecked;
        }
    }
    return QSqlQueryModel::data(index, role);
}

bool SignInModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()!=4)
        return false;
    if (role == Qt::CheckStateRole) {
        QModelIndex personIndex = QSqlQueryModel::index(index.row(), 0);
        int personId = data(personIndex).toInt();
        QModelIndex dateIndex = QSqlQueryModel::index(index.row(), 3);
        int dateId = data(dateIndex).toInt();
        clear();
        bool ok;
        if (value.toInt()==Qt::Checked) {
            ok = signIn(personId, dateId);
        } else {
            ok = unSignIn(personId, dateId);
        }
        refresh();
        return ok;
    }
    return false;
}

void SignInModel::init(const QString str, const QSqlDatabase &db)
{
    QString query("select a.id as person_id, a.name, date_id as signedIn, (select id from date where date='%1') as date_id from people a left join (select person_id, date_id from person_date join date on person_date.date_id = date.id where date.date='%2') b on a.id=b.person_id");
    query = query.arg(str).arg(str);
    qDebug()<<query;
    queryString = query;
    setQuery(queryString, db);
}

void SignInModel::setQuery(const QString &query, const QSqlDatabase &db)
{
    QSqlQueryModel::setQuery(query, db);
    insertColumn(4);
    setHeaderData(1, Qt::Horizontal, QString::fromUtf8("姓名"));
    setHeaderData(4, Qt::Horizontal, QString::fromUtf8("签到状态"));
}

void SignInModel::setDate(const QString dateString)
{
    QString query("select a.id as person_id, a.name, date_id as signedIn, (select id from date where date='%1') as date_id from people a left join (select person_id, date_id from person_date join date on person_date.date_id = date.id where date.date='%2') b on a.id=b.person_id");
    query = query.arg(dateString).arg(dateString);
    queryString = query;
    setQuery(queryString);
}

bool SignInModel::signIn(int personId, int dateId)
{
    QSqlQuery query;
    int max_id = maxId();
    query.prepare("insert into person_date (id, person_id, date_id) values (:id, :person_id, :date_id)");
    query.bindValue(":id", max_id + 1);
    query.bindValue(":person_id", personId);
    query.bindValue(":date_id", dateId);
    return query.exec();
}

bool SignInModel::unSignIn(int personId, int dateId)
{
    QSqlQuery query;
    query.prepare("delete from person_date where person_id = :pid and date_id = :did");
    query.bindValue(":pid", personId);
    query.bindValue(":did", dateId);
    return query.exec();
}

void SignInModel::refresh()
{
    setQuery(queryString);
}


int SignInModel::maxId()
{
    QSqlQuery query;
    int max_id = 0;
    query.exec("select max(id) from person_date");
    query.next();
    if (!query.value(0).isNull()) {
        max_id = query.value(0).toInt();
        qDebug()<<"max id in person_date: "<<max_id;
    }
    return max_id;
}
