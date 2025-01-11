#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addcontactwindow.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // Создаем таблицу
    contactTable = new QTableView(this);

    resize(1000, 600);

    // Создаем модель данных
    model = new QStandardItemModel();

    // Устанавливаем заголовки столбцов
    model->setHorizontalHeaderLabels({"Фамилия", "Имя", "Отчество", "Дата рождения", "Адрес", "Email", "Номера телефонов"});


    // Привязываем модель к QTableView
    contactTable->setModel(model);

    QFont font("Arial");  // Можно выбрать другой шрифт, поддерживающий кириллицу
    font.setStyleHint(QFont::SansSerif);  // Выбор шрифта без засечек
    contactTable->setFont(font);
    //contactTable->setStyleSheet("QTableView { font-family: 'Arial'; font-size: 12pt; }");

    // Разрешаем сортировку по столбцам
    contactTable->setSortingEnabled(true);
    contactTable->setSelectionBehavior(QAbstractItemView::SelectRows);


    // Настроим вид заголовков для сортировки
    QHeaderView *header = contactTable->horizontalHeader();
    header->setSectionsClickable(true);
    header->setSortIndicatorShown(true);



    // Настроим растягивание столбцов
    // Настроим растягивание столбцов в зависимости от содержания
    header->setSectionResizeMode(QHeaderView::Interactive);  // Столбцы можно вручную растягивать
    header->setSectionResizeMode(QHeaderView::ResizeToContents);  // Столбцы растягиваются в зависимости от содержимого
    header->setStretchLastSection(true);  // Растягиваем последний столбец

    // Создаем кнопки для добавления, редактирования, удаления контакта
    QPushButton *addButton = new QPushButton("Добавить контакт", this);
    QPushButton *editButton = new QPushButton("Изменить контакт", this);
    QPushButton *deleteButton = new QPushButton("Удалить контакт", this);

    // Устанавливаем фиксированный размер и округленные углы для кнопок
    addButton->setFixedSize(150, 40);
    editButton->setFixedSize(150, 40);
    deleteButton->setFixedSize(150, 40);

    // Устанавливаем стиль для округления кнопок
    QString buttonStyle = "QPushButton {"
                          "border-radius: 20px;"
                          "background-color: #4CAF50;"
                          "color: white;"
                          "font-size: 14px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #45a049;"
                          "}";
    addButton->setStyleSheet(buttonStyle);
    editButton->setStyleSheet(buttonStyle);
    deleteButton->setStyleSheet(buttonStyle);

    // Создаем кнопки для поиска и сброса фильтра
    QPushButton *findButton = new QPushButton("Найти контакт", this);
    QPushButton *resetFilterButton = new QPushButton("Сбросить фильтр", this);

    // Устанавливаем фиксированный размер и округленные углы для кнопок
    findButton->setFixedSize(150, 40);
    resetFilterButton->setFixedSize(150, 40);

    // Устанавливаем стиль для поиска и сброса фильтра
    findButton->setStyleSheet(buttonStyle);
    resetFilterButton->setStyleSheet(buttonStyle);

    // Создаем вертикальный лэйаут для кнопок слева
    QVBoxLayout *leftButtonLayout = new QVBoxLayout();
    leftButtonLayout->addWidget(addButton);
    leftButtonLayout->addWidget(editButton);
    leftButtonLayout->addWidget(deleteButton);

    // Создаем вертикальный лэйаут для кнопок справа
    QVBoxLayout *rightButtonLayout = new QVBoxLayout();
    rightButtonLayout->addWidget(findButton);
    rightButtonLayout->addWidget(resetFilterButton);

    // Создаем горизонтальный лэйаут для размещения кнопок слева и справа
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addLayout(leftButtonLayout);  // Кнопки слева
    buttonLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed)); // Промежуток
    buttonLayout->addLayout(rightButtonLayout);  // Кнопки справа

    // Основная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(contactTable);
    mainLayout->addLayout(buttonLayout);  // Кнопки размещены под таблицей

    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Подключаем слоты для кнопок
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteContact);
    connect(findButton, &QPushButton::clicked, this, &MainWindow::findContact);
    connect(resetFilterButton, &QPushButton::clicked, this, &MainWindow::resetFilter);

    loadListFromBook();
}

void MainWindow::setReadOnlyModel() {
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < model->columnCount(); ++column) {
            QStandardItem *item = model->item(row, column);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Убираем флаг редактирования
        }
    }
}

void MainWindow::loadListFromBook(){
    std::vector<Contact> contacts = phoneBook.loadContacts();
    // Очищаем текущие данные в таблице
    model->removeRows(0, model->rowCount());

    // Проходим по всем контактам
    for (const auto& contact : contacts) {
        // Создаем список для хранения ячеек
        QList<QStandardItem*> rowItems;

        // Фамилия
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.lastName.c_str())));

        // Имя
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.firstName.c_str())));

        // Отчество
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.middleName.c_str())));

        // Дата рождения
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.birthDate.c_str())));

        // Адрес
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.address.c_str())));

        // Email
        rowItems.append(new QStandardItem(QString::fromUtf8(contact.email.c_str())));

        // Номера телефонов (объединяем их в одну строку, разделенную запятой)
        QString phoneNumbers = QString::fromUtf8(contact.phoneNumbers.empty() ? "" : contact.phoneNumbers[0].c_str());
        for (size_t i = 1; i < contact.phoneNumbers.size(); ++i) {
            phoneNumbers += ", " + QString::fromUtf8(contact.phoneNumbers[i].c_str());
        }
        rowItems.append(new QStandardItem(phoneNumbers));

        // Добавляем строку в модель
        model->appendRow(rowItems);
    }

    setReadOnlyModel();
}

void MainWindow::deleteContact()
{
    // Получаем индекс выбранной строки
    QModelIndexList selectedIndexes = contactTable->selectionModel()->selectedRows();

    // Если не выбран ни один контакт, ничего не делаем
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // Получаем индекс первой выбранной строки
    QModelIndex selectedIndex = selectedIndexes.first();

    phoneBook.deleteContact(selectedIndex.row());
    // Удаляем строку из модели
    model->removeRow(selectedIndex.row());


}

void MainWindow::addContact()
{
     AddContactWindow addContactWindow(this);

    if (addContactWindow.exec() == QDialog::Accepted) {
        // После закрытия окна получаем данные
        Contact contact;

        contact.lastName = addContactWindow.getLastName().toStdString();
        contact.firstName = addContactWindow.getFirstName().toStdString();
        contact.middleName = addContactWindow.getMiddleName().toStdString();
        contact.birthDate = addContactWindow.getBirthDate().toStdString();
        contact.address = addContactWindow.getAddress().toStdString();
        contact.email = addContactWindow.getEmail().toStdString();
        QString phoneNumber = addContactWindow.getPhoneNumber().replace(" ", "");


        contact.phoneNumbers = phoneBook.split(phoneNumber.toStdString(), ";");
        if(contact.phoneNumbers[0] == "")
            contact.phoneNumbers.erase(contact.phoneNumbers.begin());

        //Contact test = phoneBook.loadContacts()[0];

        if(ContactValidator::chekFullContact(contact)){
            phoneBook.addContact(contact, false);


            // Создаем список для хранения ячеек
             QList<QStandardItem*> rowItems;

            rowItems.append(new QStandardItem(QString::fromUtf8(contact.lastName.c_str())));
            rowItems.append(new QStandardItem(QString::fromUtf8(contact.firstName.c_str())));
            rowItems.append(new QStandardItem(QString::fromUtf8(contact.middleName.c_str())));
            rowItems.append(new QStandardItem(QString::fromUtf8(contact.birthDate.c_str())));
            rowItems.append(new QStandardItem(QString::fromUtf8(contact.address.c_str())));
            rowItems.append(new QStandardItem(QString::fromUtf8(contact.email.c_str())));
            QString phoneNumbers = QString::fromUtf8(contact.phoneNumbers.empty() ? "NoPhone" : contact.phoneNumbers[0].c_str());

            for (size_t i = 1; i < contact.phoneNumbers.size(); ++i) {
                phoneNumbers += ", " + QString::fromUtf8(contact.phoneNumbers[i].c_str());
         }
         rowItems.append(new QStandardItem(phoneNumbers));
            model->appendRow(rowItems);
        }
    }
}

void MainWindow::editContact()
{
    if(proxyModel){
        QMessageBox::warning(this, "Ошибка", "Невозможно изменить контакт при включенном фильтре.");
        return;
    }
    QModelIndexList selectedIndexes = contactTable->selectionModel()->selectedRows();

    // Если не выбран ни один контакт, ничего не делаем
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // Получаем выбранную строку
    QModelIndex selectedIndex = selectedIndexes.first();
    int ind = selectedIndex.row();

    QString lastName = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 0)).toString();
    QString firstName = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 1)).toString();
    QString middleName = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 2)).toString();
    QString birthDate = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 3)).toString();
    QString address = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 4)).toString();
    QString email = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 5)).toString();
    QString phoneNumbers = contactTable->model()->data(contactTable->model()->index(selectedIndex.row(), 6)).toString();


    // Открываем окно редактирования с переданными данными
    AddContactWindow editWindow = AddContactWindow(
        lastName, firstName, middleName, birthDate, address, email, phoneNumbers, this);

    // Отображаем окно
    if (editWindow.exec() == QDialog::Accepted) {
        std::vector<Contact> contactsList = phoneBook.loadContacts();

        contactsList[ind].lastName = editWindow.getLastName().toStdString();
        contactsList[ind].firstName = editWindow.getFirstName().toStdString();
        contactsList[ind].middleName = editWindow.getMiddleName().toStdString();
        contactsList[ind].birthDate = editWindow.getBirthDate().toStdString();
        contactsList[ind].address = editWindow.getAddress().toStdString();
        contactsList[ind].email = editWindow.getEmail().toStdString();
        QString phoneNumber = editWindow.getPhoneNumber().replace(" ", "");

        contactsList[ind].phoneNumbers = phoneBook.split(phoneNumber.toStdString(), ";");
        if(contactsList[ind].phoneNumbers[0] == ""){
            contactsList[ind].phoneNumbers.erase(contactsList[ind].phoneNumbers.begin());
            contactsList[ind].phoneNumbers.push_back("NoPhone");
        }

        //Contact test = phoneBook.loadContacts()[0];

        if(ContactValidator::chekFullContact(contactsList[ind])){
            phoneBook.trimFields(contactsList[ind]);
            phoneBook.saveAllContacts(contactsList);

            // Обновляем данные в модели
            contactTable->model()->setData(contactTable->model()->index(ind, 0), QString::fromUtf8(contactsList[ind].lastName.c_str()));
            contactTable->model()->setData(contactTable->model()->index(ind, 1), QString::fromUtf8(contactsList[ind].firstName.c_str()));
            contactTable->model()->setData(contactTable->model()->index(ind, 2), QString::fromUtf8(contactsList[ind].middleName.c_str()));
            contactTable->model()->setData(contactTable->model()->index(ind, 3), QString::fromUtf8(contactsList[ind].birthDate.c_str()));
            contactTable->model()->setData(contactTable->model()->index(ind, 4), QString::fromUtf8(contactsList[ind].address.c_str()));
            contactTable->model()->setData(contactTable->model()->index(ind, 5), QString::fromUtf8(contactsList[ind].email.c_str()));

            // Обновляем телефоны
            QString updatedPhoneNumbers = QString::fromUtf8(contactsList[ind].phoneNumbers.empty() ? "NoPhone" : contactsList[ind].phoneNumbers[0].c_str());
            for (size_t i = 1; i < contactsList[ind].phoneNumbers.size(); ++i) {
                updatedPhoneNumbers += ", " + QString::fromUtf8(contactsList[ind].phoneNumbers[i].c_str());
            }
            contactTable->model()->setData(contactTable->model()->index(ind, 6), updatedPhoneNumbers);
        }
    }
}

void MainWindow::findContact()
{
    resetFilter();

    AddContactWindow addContactWindow(this);

    if (addContactWindow.exec() == QDialog::Accepted) {
        QString lastName = addContactWindow.getLastName();
        QString firstName = addContactWindow.getFirstName();
        QString middleName = addContactWindow.getMiddleName();
        QString birthDate = addContactWindow.getBirthDate();
        QString address = addContactWindow.getAddress();
        QString email = addContactWindow.getEmail();
        QString phoneNumbers = addContactWindow.getPhoneNumber();

        // Сохраняем исходную модель
        if (!originalModel) {
            originalModel = contactTable->model();
        }

        // Создаем новую прокси модель
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(originalModel);  // Устанавливаем исходную модель для прокси

        // Устанавливаем фильтры для каждой колонки
        if (!lastName.isEmpty()) {
            proxyModel->setFilterKeyColumn(0);  // Колонка для lastName (предположим, что это первая колонка)
            proxyModel->setFilterFixedString(lastName);
        }

        if (!firstName.isEmpty()) {
            proxyModel->setFilterKeyColumn(1);  // Колонка для firstName (предположим, что это вторая колонка)
            proxyModel->setFilterFixedString(firstName);
        }

        if (!middleName.isEmpty()) {
            proxyModel->setFilterKeyColumn(2);  // Колонка для middleName
            proxyModel->setFilterFixedString(middleName);
        }

        if (!birthDate.isEmpty()) {
            proxyModel->setFilterKeyColumn(3);  // Колонка для birthDate
            proxyModel->setFilterFixedString(birthDate);
        }

        if (!address.isEmpty()) {
            proxyModel->setFilterKeyColumn(4);  // Колонка для address
            proxyModel->setFilterFixedString(address);
        }

        if (!email.isEmpty()) {
            proxyModel->setFilterKeyColumn(5);  // Колонка для email
            proxyModel->setFilterFixedString(email);
        }

        if (!phoneNumbers.isEmpty()) {
            proxyModel->setFilterKeyColumn(6);  // Колонка для phoneNumbers
            proxyModel->setFilterFixedString(phoneNumbers);
        }

        // Устанавливаем новую прокси модель в таблицу
        contactTable->setModel(proxyModel);
    }
}

void MainWindow::resetFilter()
{
    // Проверяем, если существует прокси модель, то удаляем её
    if (proxyModel) {
        delete proxyModel;
        proxyModel = nullptr;  // Обнуляем указатель, чтобы избежать дальнейшего использования
    }

    // Возвращаем таблицу к исходному состоянию (устанавливаем оригинальную модель)
    if (originalModel) {
        contactTable->setModel(originalModel);  // Восстанавливаем исходную модель
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
