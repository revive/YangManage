#ifndef SIGNSTATS_H
#define SIGNSTATS_H

#include <QString>
#include <QDate>
#include <QMap>
#include <QList>

class SignStats
{
public:
    SignStats(QDate s_date, QDate e_date);

    void setTotalPerson(int tp);
    void setTotalPersonTime(int tpt);
    void addPersonTimeCount(int pt, int count);
    void addTopPersonTime(int count, QString & name);
    void addPersonTimeByWeekDay(int count);

    int getTotalPerson() const;
    int getTotalPersonTime() const;
    QMap<int, int> getPersonTimeCount() const;
    QMap<int, QString> getTopPersonTime() const;
    QList<int> getPersonTimeByWeekDay() const;
    bool isUsable();
    QString getKey() const;
    QString getStartDate() const;
    QString getEndDate() const;

    void setUsableStat(bool status);
private:
    QString key;
    QDate startDate;
    QDate endDate;
    int totalPerson;
    int totalPersonTime;
    QMap<int, int> personTimeCount;
    QMap<int, QString> topPersonTime;
    QList<int> personTimeByWeekDay;
    bool usable;
};

#endif // SIGNSTATS_H
