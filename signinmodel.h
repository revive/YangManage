#ifndef SIGNINMODEL_H
#define SIGNINMODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class SignInModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit SignInModel(QObject *parent = 0);
    Qt::ItemFlags flags (const QModelIndex & index ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    void init (const QString str, const QSqlDatabase & db = QSqlDatabase());
    void setQuery (const QString & query, const QSqlDatabase & db = QSqlDatabase());
    void setDate(const QString dateString);

signals:
    
public slots:
    
private:
    bool signIn(int personId, int dateId);
    bool unSignIn(int personId, int dateId);
    void refresh();
    int maxId();

    QString queryString;
};

#endif // SIGNINMODEL_H
