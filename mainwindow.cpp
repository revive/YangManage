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

#include <QDebug>

extern int personId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    core = new CoreData();
    connect(ui->actionNew, SIGNAL(activated()), this, SLOT(newDatabase()));
    connect(ui->actionOpen, SIGNAL(activated()), this, SLOT(openDatabase()));
    connect(ui->actionClose, SIGNAL(activated()), this, SLOT(closeDatabase()));
    connect(ui->personTableView, SIGNAL(activated(QModelIndex)), this, SLOT(showPersonDetail(QModelIndex)));
    connect(ui->editDetailButton, SIGNAL(clicked()), this, SLOT(showPersonDetailEditForm()));
    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(updatePersonDetail()));
    connect(ui->delButton, SIGNAL(clicked()), this, SLOT(deletePeople()));
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->delButton->setEnabled(false);
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
    if(core->createDBFile(filename)) {
        ui->personTableView->setModel(core->getPersonModel());
        setPersonTableHidden();
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(1);
        connect(ui->addButton, SIGNAL(clicked()), this, SLOT(createAddPersonDialog()));
        personId = 0;
        ui->actionClose->setEnabled(true);
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled((false));
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
        setPersonTableHidden();
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setCurrentIndex(1);
        personId = core->getMaxPersonId();
        qDebug()<<"Max Person ID: "<<personId;
        connect(ui->addButton, SIGNAL(clicked()), this, SLOT(createAddPersonDialog()));
        ui->actionClose->setEnabled(true);
        ui->actionNew->setEnabled(false);
        ui->actionOpen->setEnabled((false));
    }
}

void MainWindow::closeDatabase()
{
    ui->personTableView->setModel(0);
    core->getDataBase()->close();
    ui->actionClose->setEnabled(false);
    ui->actionNew->setEnabled(true);
    ui->actionOpen->setEnabled(true);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
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

void MainWindow::setPersonTableHidden()
{
    ui->personTableView->setColumnHidden(0, true);
    ui->personTableView->setColumnHidden(4, true);
    ui->personTableView->setColumnHidden(5, true);
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
