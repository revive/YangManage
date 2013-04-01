#include "addpersondialog.h"
#include "ui_addpersondialog.h"

#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>

int personId;

AddPersonDialog::AddPersonDialog(QSqlTableModel * model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPersonDialog)
{
    this->model = model;
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
}

AddPersonDialog::~AddPersonDialog()
{
    delete ui;
}

void AddPersonDialog::submit()
{
    QString name = ui->nameLineEdit->text();
    QString gender = ui->genderComboBox->currentText();
    QString joinDate = ui->joinDateEdit->date().toString("yyyy-MM-dd");
    int status = ui->statusComboBox->currentIndex();
    QString comment = ui->commentTextEdit->toPlainText();
    if (name.isEmpty()) {
        QString message(QString::fromUtf8("姓名不能为空"));
        QMessageBox::information(this, QString::fromUtf8("错误！"), message);
    } else {
        int id = generatePersonId();
        QSqlRecord record;
        QSqlField f1 ("id", QVariant::Int);
        QSqlField f2 ("name", QVariant::String);
        QSqlField f3 ("gender", QVariant::String);
        QSqlField f4 ("join_date", QVariant::String);
        QSqlField f5 ("status", QVariant::Int);
        QSqlField f6 ("comment", QVariant::String);
        f1.setValue(id);
        f2.setValue(name);
        f3.setValue(gender);
        f4.setValue(joinDate);
        f5.setValue(status);
        f6.setValue(comment);
        record.append(f1);
        record.append(f2);
        record.append(f3);
        record.append(f4);
        record.append(f5);
        record.append(f6);

        model->insertRecord(-1, record);
        accept();
    }
}

int AddPersonDialog::generatePersonId()
{
    personId++;
    return personId;
}
