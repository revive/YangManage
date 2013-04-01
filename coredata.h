#ifndef COREDATA_H
#define COREDATA_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>

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
    int getMaxPersonId();

public slots:

private:
    void setPersonModel();
    QSqlDatabase db;
    QSqlTableModel * personModel;
};

#endif // COREDATA_H
