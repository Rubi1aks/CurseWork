#ifndef ADDCONTACTWINDOW_H
#define ADDCONTACTWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "phonebook.h"

class AddContactWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactWindow(QWidget *parent = nullptr);
    AddContactWindow(const QString &lastName,
                     const QString &firstName,
                     const QString &middleName,
                     const QString &birthDate,
                     const QString &address,
                     const QString &email,
                     const QString &phoneNumbers,
                     QWidget *parent = nullptr);
    ~AddContactWindow();

    QString getLastName() const;
    QString getFirstName() const;
    QString getMiddleName() const;
    QString getBirthDate() const;
    QString getAddress() const;
    QString getEmail() const;
    QString getPhoneNumber() const;

private slots:
    void onSaveButtonClicked();

private:
    QLineEdit *lastNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *birthDateEdit;
    QLineEdit *addressEdit;
    QLineEdit *emailEdit;
    QLineEdit *phoneNumberEdit;
    QPushButton *saveButton;
};

#endif // ADDCONTACTWINDOW_H
