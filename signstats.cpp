#include "signstats.h"

SignStats::SignStats(QDate s_date, QDate e_date)
{
    key = s_date.toString("yyyyMMdd").append("-").append(e_date.toString("yyyyMMdd"));
    startDate = s_date;
    endDate = e_date;
}

void SignStats::setTotalPerson(int tp)
{
    totalPerson = tp;
}

void SignStats::setTotalPersonTime(int tpt)
{
    totalPersonTime = tpt;
}

void SignStats::addPersonTimeCount(int pt, int count)
{
    personTimeCount.insert(pt, count);
}

void SignStats::addTopPersonTime(QString &name, int count)
{
    topPersonTime.append(QPair<QString, int> (name, count));
}

void SignStats::addPersonTimeByWeekDay(int count)
{
    personTimeByWeekDay.append(count);
}

int SignStats::getTotalPerson() const
{
    return totalPerson;
}

int SignStats::getTotalPersonTime() const
{
    return totalPersonTime;
}

QMap<int, int> SignStats::getPersonTimeCount() const
{
    return personTimeCount;
}

QList<QPair<QString, int> > SignStats::getTopPersonTime() const
{
    return topPersonTime;
}

QList<int> SignStats::getPersonTimeByWeekDay() const
{
    return personTimeByWeekDay;
}

bool SignStats::isUsable()
{
    return usable;
}

QString SignStats::getKey() const
{
    return key;
}

QString SignStats::getStartDate() const
{
    return startDate.toString("yyyy-MM-dd");
}

QString SignStats::getEndDate() const
{
    return endDate.toString("yyyy-MM-dd");
}

void SignStats::setUsableStat(bool status)
{
    usable = status;
}

void SignStats::reset()
{
    totalPerson = 0;
    totalPersonTime = 0;
    personTimeCount.clear();
    personTimeByWeekDay.clear();
    topPersonTime.clear();
}
