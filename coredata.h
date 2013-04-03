#ifndef COREDATA_H
#define COREDATA_H

#include "signinmodel.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDate>

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

public slots:
    void setSignInDate (const QDate &date);
    void refreshModel();

private slots:

private:
    void setPersonModel();
    QSqlDatabase db;
    QSqlTableModel * personModel;
    SignInModel * signInModel;
};

#endif // COREDATA_H
