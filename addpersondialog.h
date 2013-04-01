#ifndef ADDPERSONDIALOG_H
#define ADDPERSONDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class AddPersonDialog;
}

class AddPersonDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddPersonDialog(QSqlTableModel * model, QWidget *parent = 0);
    ~AddPersonDialog();
    
private slots:
    void submit();

private:
    int generatePersonId();

    QSqlTableModel * model;
    Ui::AddPersonDialog *ui;
};

#endif // ADDPERSONDIALOG_H
