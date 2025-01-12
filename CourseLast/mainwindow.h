#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include "phonebook.h"
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void loadListFromBook();
    ~MainWindow();
    void setReadOnlyModel();

private:
    PhoneBook phoneBook;
    Ui::MainWindow *ui;
    QTableView *contactTable;        // Указатель на таблицу
    QStandardItemModel *model;       // Модель данных для таблицы
    QSortFilterProxyModel *proxyModel = nullptr;
    QAbstractItemModel *originalModel = nullptr;

private slots:
    void addContact();    // Метод для добавления контакта
    void editContact();   // Метод для редактирования контакта
    void deleteContact(); // Метод для удаления контакта
    void findContact();
    void resetFilter();
};

#endif // MAINWINDOW_H
