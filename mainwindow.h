#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "coredata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    bool newDatabase();
    void openDatabase();
    void closeDatabase();
    void createAddPersonDialog();
    void showPersonDetail(QModelIndex index);
    void showPersonDetailEditForm();
    void hidePersonDetailEditForm();
    void updatePersonDetail();
    void deletePeople();
    void updateSignedFilter(const QString &);
    void unlockSignInTable();
    void lockSignInTable();
    void displayReport(int i);

private:
    void showReport(const SignStats * stats);
    void setPersonTableHidden();
    void showPersonDetailLabels();
    Ui::MainWindow *ui;
    CoreData * core;
    QSortFilterProxyModel * proxySignInModel;
    int currentPersonRow;
};

#endif // MAINWINDOW_H
