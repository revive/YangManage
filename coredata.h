#ifndef COREDATA_H
#define COREDATA_H

#include "signinmodel.h"
#include "signstats.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDate>
#include <QMap>

class CoreData : public QObject
{
    Q_OBJECT
public:
    CoreData();
    ~CoreData();
    bool createDBFile(QString dbname);
    bool openDBFile(QString dbname);
    QSqlDatabase * getDataBase();
    QSqlTableModel * getPersonModel();
    SignInModel * getSignInModel();
    int getMaxPersonId();
    void setSignInModel(const QDate & date, QSqlDatabase & db);
    SignStats * findStats(QString key);
    void addStats(SignStats * stats);
    SignStats *addStats(QDate &sdate, QDate &edate);
    void clearStats();
    void updateReport();
    void removeStats(SignStats *stats);
public slots:
    void setSignInDate (const QDate &date);
    void refreshModel();

private slots:

private:
    void setPersonModel();
    void updateStats(SignStats * stats);
    QSqlDatabase db;
    QSqlTableModel * personModel;
    SignInModel * signInModel;
    QMap<QString, SignStats *> statsReports;
};

#endif // COREDATA_H
