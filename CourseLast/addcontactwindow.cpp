#include "AddContactWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QString>

AddContactWindow::AddContactWindow(QWidget *parent)
    : QDialog(parent)
{

    // Создаем виджеты для ввода данных
    lastNameEdit = new QLineEdit(this);
    firstNameEdit = new QLineEdit(this);
    middleNameEdit = new QLineEdit(this);
    birthDateEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    phoneNumberEdit = new QLineEdit(this);

    // Создаем кнопку "Сохранить"
    saveButton = new QPushButton("Сохранить", this);

    saveButton->setFixedSize(280, 30);

    // Настроим компоновку
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Фамилия:"));
    layout->addWidget(lastNameEdit);
    layout->addWidget(new QLabel("Имя:"));
    layout->addWidget(firstNameEdit);
    layout->addWidget(new QLabel("Отчество:"));
    layout->addWidget(middleNameEdit);
    layout->addWidget(new QLabel("Дата рождения: (формат YYYY-MM-DD)"));
    layout->addWidget(birthDateEdit);
    layout->addWidget(new QLabel("Адрес:"));
    layout->addWidget(addressEdit);
    layout->addWidget(new QLabel("Email:"));
    layout->addWidget(emailEdit);
    layout->addWidget(new QLabel("Номер телефона: (через ;)"));
    layout->addWidget(phoneNumberEdit);

    //lastNameEdit->setText("Рачков");
    //firstNameEdit->setText("Рачков");
    //middleNameEdit->setText("Рачков");
    //birthDateEdit->setText("2020-10-10");
    //addressEdit->setText("ул. Ленина 5");
    //emailEdit->setText("serg@gmail.com");
    //phoneNumberEdit->setText("+79531715071");

    layout->addWidget(saveButton);

    // Подключаем слот для сохранения
    connect(saveButton, &QPushButton::clicked, this, &AddContactWindow::onSaveButtonClicked);

    // Настроим окно
    setWindowTitle("Добавить контакт");
    setFixedSize(300, 450);  // Размер окна
}

AddContactWindow::AddContactWindow(const QString &lastName,
                                   const QString &firstName,
                                   const QString &middleName,
                                   const QString &birthDate,
                                   const QString &address,
                                   const QString &email,
                                   const QString &phoneNumbers,
                                   QWidget *parent)
    : AddContactWindow(parent)  // Вызываем первый конструктор
{
    // Устанавливаем переданные значения в поля ввода
    lastNameEdit->setText(lastName);
    firstNameEdit->setText(firstName);
    middleNameEdit->setText(middleName);
    birthDateEdit->setText(birthDate);
    addressEdit->setText(address);
    emailEdit->setText(email);
    QString modifiablePhoneNumbers = phoneNumbers;
    modifiablePhoneNumbers.replace(",", ";");
    phoneNumberEdit->setText(modifiablePhoneNumbers);
}

void AddContactWindow::onSaveButtonClicked(){
    accept();
}

QString AddContactWindow::getLastName() const
{
    return lastNameEdit->text();
}

QString AddContactWindow::getFirstName() const
{
    return firstNameEdit->text();
}

QString AddContactWindow::getMiddleName() const
{
    return middleNameEdit->text();
}

QString AddContactWindow::getBirthDate() const
{
    return birthDateEdit->text();
}

QString AddContactWindow::getAddress() const
{
    return addressEdit->text();
}

QString AddContactWindow::getEmail() const
{
    return emailEdit->text();
}

QString AddContactWindow::getPhoneNumber() const
{
    return phoneNumberEdit->text();
}

AddContactWindow::~AddContactWindow()
{
}
