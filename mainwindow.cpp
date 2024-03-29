#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addpersondialog.h"

#include <QFileDialog>
#include <QSqlRecord>
#include <QLineEdit>
#include <QComboBox>
#include <QDate>
#include <QSqlRecord>
#include <QSqlField>
#include <QCalendarWidget>

#include <QDebug>
#include <QMap>

extern int personId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = new CoreData();
    currentReport = -1;
    connect(ui->actionNew, SIGNAL(activated()), this, SLOT(newDatabase()));
    connect(ui->actionOpen, SIGNAL(activated()), this, SLOT(openDatabase()));
    connect(ui->actionClose, SIGNAL(activated()), this, SLOT(closeDatabase()));
    connect(ui->personTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(showPersonDetail(QModelIndex)));
    connect(ui->editDetailButton, SIGNAL(clicked()), this, SLOT(showPersonDetailEditForm()));
    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(updatePersonDetail()));
    connect(ui->delButton, SIGNAL(clicked()), this, SLOT(deletePeople()));
    connect(ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(updateSignedFilter(QString)));
    connect(ui->signInButton, SIGNAL(clicked()), this, SLOT(unlockSignInTable()));
    connect(ui->lockButton, SIGNAL(clicked()), this, SLOT(lockSignInTable()));
    connect(ui->refreshReportButton, SIGNAL(clicked()), this, SLOT(updateReport()));
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    ui->delButton->setEnabled(false);
    ui->signDateEdit->setDate(QDate::currentDate());
    ui->signDateEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    hidePersonDetailEditForm();
    currentPersonRow = 0;
    ui->personTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::newDatabase()
{
    QString filename = QFileDialog::getSaveFileName(this, QString::fromUtf8("新建文件"), "", "Data File (*.ymr)");
    if (filename.isEmpty()) {
        return false;
    }
    qDebug()<<filename;
    ui->statusBar->showMessage(QString::fromUtf8("数据库初始化中，请等待……"));
    if(core->createDBFile(filename)) {
        ui->statusBar->showMessage(QString::fromUtf8("数据库初始化完成。"));
        ui->personTableView->setModel(core->getPersonModel());
        proxySignInModel = new QSortFilterProxyModel(this);
        proxySignInModel->setSourceModel(core->getSignInModel());
        ui->activePersonView->setModel(proxySignInModel);
        setPersonTableHidden();
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setTabEnabled(3, true);
        ui->tabWidget->setCurrentIndex(1);
        connect(ui->addButton, SIGNAL(clicked()), this, SLOT(createAddPersonDialog()));
        connect(ui->signDateEdit, SIGNAL(dateChanged(QDate)), core, SLOT(setSignInDate(QDate)));
        connect(ui->refreshButton, SIGNAL(clicked()), core, SLOT(refreshModel()));
        personId = 0;
        ui->actionClose->setEnabled(true);
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled((false));
        connect(ui->reportWidget, SIGNAL(currentRowChanged(int)), this, SLOT(displayReport(int)));
    }
    return true;
}

void MainWindow::openDatabase()
{
    QString filename = QFileDialog::getOpenFileName(this, QString::fromUtf8("选择文件"), "", "Data File (*.ymr)");
    if (filename.isEmpty()) {
        return;
    }
    qDebug()<<filename;
    if (core->openDBFile(filename)) {
        ui->personTableView->setModel(core->getPersonModel());
        proxySignInModel = new QSortFilterProxyModel(this);
        proxySignInModel->setSourceModel(core->getSignInModel());
        ui->activePersonView->setModel(proxySignInModel);
        setPersonTableHidden();
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setTabEnabled(3, true);
        ui->tabWidget->setCurrentIndex(1);
        personId = core->getMaxPersonId();
        qDebug()<<"Max Person ID: "<<personId;
        connect(ui->addButton, SIGNAL(clicked()), this, SLOT(createAddPersonDialog()));
        connect(ui->signDateEdit, SIGNAL(dateChanged(QDate)), core, SLOT(setSignInDate(QDate)));
        connect(ui->refreshButton, SIGNAL(clicked()), core, SLOT(refreshModel()));
        ui->actionClose->setEnabled(true);
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled((false));
        connect(ui->reportWidget, SIGNAL(currentRowChanged(int)), this, SLOT(displayReport(int)));
    }
}

void MainWindow::closeDatabase()
{
    ui->personTableView->setModel(0);
    ui->activePersonView->setModel(0);
    delete proxySignInModel;
    proxySignInModel = 0;
    core->getDataBase()->close();
    core->clearStats();
    ui->actionClose->setEnabled(false);
    ui->actionNew->setEnabled(true);
    ui->actionOpen->setEnabled(true);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
}

void MainWindow::createAddPersonDialog()
{
    AddPersonDialog * dialog = new AddPersonDialog(core->getPersonModel());
    if (dialog->exec() == QDialog::Accepted) {
        core->getPersonModel()->submitAll();
    }
}

void MainWindow::showPersonDetail(QModelIndex index)
{
    currentPersonRow = index.row();
    QSqlRecord record = core->getPersonModel()->record(currentPersonRow);
    QString name = record.value("name").toString();
    QString gender = record.value("gender").toString();
    QString joinDate = record.value("join_date").toString();
    int status = record.value("status").toInt();
    QString statusString;
    if (status==0) {
        statusString = QString::fromUtf8("在校");
    } else if (status == 1) {
        statusString = QString::fromUtf8("离校");
    } else {
        statusString = QString::fromUtf8("不明");
    }
    QString comment = record.value("comment").toString();
    ui->nameLabel->setText(name);
    ui->genderLabel->setText(gender);
    ui->joinDateLabel->setText(joinDate);
    ui->statusLabel->setText(statusString);
    ui->commentBrowser->setText(comment);
    ui->editDetailButton->setEnabled(true);
    ui->delButton->setEnabled(true);
}

void MainWindow::showPersonDetailEditForm()
{
    QString name = ui->nameLabel->text();
    ui->nameEdit->setText(name);
    ui->nameEdit->setVisible(true);
    ui->genderBox->setVisible(true);
    ui->dateEdit->setDate(QDate::fromString(ui->joinDateLabel->text(), "yyyy-MM-dd"));
    ui->dateEdit->setVisible(true);
    ui->statusBox->setVisible(true);
    ui->commentEdit->setText(ui->commentBrowser->toHtml());
    ui->commentEdit->setVisible(true);
    ui->confirmButton->setVisible(true);
    ui->confirmButton->setEnabled(true);

    ui->nameLabel->setVisible(false);
    ui->genderLabel->setVisible(false);
    ui->joinDateLabel->setVisible(false);
    ui->statusLabel->setVisible(false);
    ui->commentBrowser->setVisible(false);
    ui->editDetailButton->setVisible(false);
    ui->editDetailButton->setEnabled(false);

}

void MainWindow::hidePersonDetailEditForm()
{
    ui->nameEdit->setVisible(false);
    ui->genderBox->setVisible(false);
    ui->dateEdit->setVisible(false);
    ui->statusBox->setVisible(false);
    ui->commentEdit->setVisible(false);
    ui->confirmButton->setVisible(false);
    ui->confirmButton->setEnabled(false);
}

void MainWindow::updatePersonDetail()
{
    QString name = ui->nameEdit->text();
    QString gender = ui->genderBox->currentText();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");
    int status = ui->statusBox->currentIndex();
    QString comment = ui->commentEdit->toHtml();
    ui->nameLabel->setText(name);
    ui->genderLabel->setText(gender);
    ui->joinDateLabel->setText(date);
    if (status == 0) {
        ui->statusLabel->setText(QString::fromUtf8("在校"));
    } else if (status == 1) {
        ui->statusLabel->setText(QString::fromUtf8("离校"));
    } else {
        ui->statusLabel->setText(QString::fromUtf8("不明"));
    }
    ui->commentBrowser->setText(comment);
    showPersonDetailLabels();
    hidePersonDetailEditForm();
    QSqlRecord record;
    QSqlField f2 ("name", QVariant::String);
    QSqlField f3 ("gender", QVariant::String);
    QSqlField f4 ("join_date", QVariant::String);
    QSqlField f5 ("status", QVariant::Int);
    QSqlField f6 ("comment", QVariant::String);
    f2.setValue(name);
    f3.setValue(gender);
    f4.setValue(date);
    f5.setValue(status);
    f6.setValue(comment);
    record.append(f2);
    record.append(f3);
    record.append(f4);
    record.append(f5);
    record.append(f6);
    if (core->getPersonModel()->setRecord(currentPersonRow, record)) {
        core->getPersonModel()->submitAll();
        qDebug()<<"recorder at "<< currentPersonRow << " is updated!";
    }
}

void MainWindow::deletePeople()
{
    qDebug()<<"delete people.";
    QModelIndexList indices = ui->personTableView->selectionModel()->selectedRows();
    qDebug()<<indices.count();
    QModelIndex previousRowIndex = core->getPersonModel()->index(0, 0);
    QModelIndex lastSelectedIndex;
    for (int i=0; i<indices.count(); ++i) {
        QModelIndex index = indices.at(i);
        if (i==0 && index.row()!=0) {
            previousRowIndex = core->getPersonModel()->index(index.row()-1, 0);
        }
        qDebug()<<index.row();
        core->getPersonModel()->removeRow(index.row());
        if (i==indices.count()-1) {
            lastSelectedIndex = index;
        }
    }
    core->getPersonModel()->submitAll();
    ui->personTableView->setCurrentIndex(previousRowIndex);
    showPersonDetail(previousRowIndex);
}

void MainWindow::updateSignedFilter(const QString & data)
{
    if (proxySignInModel) {
        proxySignInModel->setFilterRegExp(QRegExp(data, Qt::CaseInsensitive));
    }
}

void MainWindow::unlockSignInTable()
{
    ui->filterEdit->setEnabled(true);
    ui->activePersonView->setEnabled(true);
    ui->lockButton->setEnabled(true);
    ui->signInButton->setEnabled(false);
}

void MainWindow::lockSignInTable()
{
    ui->filterEdit->setEnabled(false);
    ui->activePersonView->setEnabled(false);
    ui->lockButton->setEnabled(false);
    ui->signInButton->setEnabled(true);
}

void MainWindow::updateReport()
{
    core->updateReport();
    if (currentReport != -1) {
        displayReport(currentReport);
    }
}

void MainWindow::displayReport(int i)
{
    qDebug()<<i;
    currentReport = i;
    QDate date = QDate::currentDate();
    QDate sdate, edate;
    QString key;
    int dow, dom, doy, dot;

    bool validReport = true;
    if (i==0) {
        dow = date.dayOfWeek();
        sdate = date.addDays(-(dow-1));
        edate = date;
    } else if (i==1) {
        dow = date.dayOfWeek();
        sdate = date.addDays(-(dow-1)-7);
        edate = date.addDays(-dow);
    } else if (i==2) {
        dom = date.day();
        sdate = date.addDays(-(dom-1));
        edate = date;
    } else if (i==3) {
        dom = date.day();
        edate = date.addDays(-dom);
        sdate = edate.addDays(-edate.daysInMonth()+1);
    } else if (i==4) {
        doy = date.dayOfYear();
        sdate = date.addDays(-doy+1);
        edate = date;
    } else {
        validReport = false;
    }
    if (!validReport)
        return;
    key = sdate.toString("yyyyMMdd").append("-").append(edate.toString("yyyyMMdd"));
    SignStats * stats = core->findStats(key);
    if (stats!=0) {
        qDebug()<<"find stats.";
        showReport(stats);
    } else {
        stats = core->addStats(sdate, edate);
        qDebug()<<"new stats";
        showReport(stats);
    }
}

void MainWindow::showReport(const SignStats *stats)
{
    QString text;
    text.append(QString::fromUtf8("<h1>报表</h1>"));
    text.append("<h2>").append(stats->getStartDate()).append(QString::fromUtf8(" 至 ")).append(stats->getEndDate()).append("</h2>");
    text.append("<hr>");
    text.append("<p>");
    int totalPersonTime = stats->getTotalPersonTime();
    text.append(QString::fromUtf8("时段总人次：")).append(QString::number(totalPersonTime));
    text.append("</p>");
    text.append("<hr>");
    text.append("<p>");
    int totalPerson = stats->getTotalPerson();
    text.append(QString::fromUtf8("时段总人数：")).append(QString::number(totalPerson));
    text.append("</p>");
    text.append("<hr>");
    text.append("<p>").append(QString::fromUtf8("时段到达次数统计")).append("</p>");
    text.append(QString::fromUtf8("<div><table><tr><td>次数</td><td>人数</td></tr>"));
    QMap<int, int> ptc = stats->getPersonTimeCount();
    QMapIterator<int, int> im(ptc);
    while (im.hasNext()) {
        im.next();
        text.append("<tr><td>").append(QString::number(im.key())).append("</td><td>").append(QString::number(im.value())).append("</td></tr>");
    }
    text.append("</table></div>");
    text.append("<p>").append(QString::fromUtf8("最多到达次数排序")).append("</p>");
    text.append(QString::fromUtf8("<div><table><tr><td>姓名</td><td>次数</td></tr>"));
    QList<QPair<QString, int> > tpt = stats->getTopPersonTime();
    for (int i=0; i<tpt.size(); ++i) {
        text.append("<tr><td>").append(tpt.at(i).first).append("</td><td>").append(QString::number(tpt.at(i).second)).append("</td></tr>");
    }
    text.append("</table></div>");
    ui->reportBrowser->setHtml(text);
}

void MainWindow::setPersonTableHidden()
{
    ui->personTableView->setColumnHidden(0, true);
    ui->personTableView->setColumnHidden(4, true);
    ui->personTableView->setColumnHidden(5, true);
    ui->activePersonView->setColumnHidden(0, true);
    ui->activePersonView->setColumnHidden(2, true);
    ui->activePersonView->setColumnHidden(3, true);
    proxySignInModel->setFilterKeyColumn(1);
}

void MainWindow::showPersonDetailLabels()
{
    ui->nameLabel->setVisible(true);
    ui->genderLabel->setVisible(true);
    ui->joinDateLabel->setVisible(true);
    ui->statusLabel->setVisible(true);
    ui->commentBrowser->setVisible(true);
    ui->editDetailButton->setVisible(true);
    ui->editDetailButton->setEnabled(true);
}
