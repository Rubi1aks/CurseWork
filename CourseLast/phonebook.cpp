// PhoneBook.cpp
#include "PhoneBook.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QStringConverter>
#include <vector>
#include <string>
#include <fstream>
#include <QRegularExpression>
#include <QDate>
#include <QDateTime>

bool ContactValidator::chekFullContact(const Contact& contact) {
    if (!isValidName(contact.firstName)) {
        std::cerr << "Ошибка: Неверное имя.\n";
        return false;
    }
    if (!isValidName(contact.lastName)) {
        std::cerr << "Ошибка: Неверная фамилия.\n";
        return false;
    }
    if (!contact.middleName.empty() && !isValidName(contact.middleName)) {
        std::cerr << "Ошибка: Неверное отчество.\n";
        return false;
    }
    if (!isValidDate(contact.birthDate)) {
        std::cerr << "Ошибка: Неверная дата рождения.\n";
        return false;
    }
    if (!isNoComma(contact.address)) {
        std::cerr << "Ошибка: Неверный адрес.\n";
        return false;
    }
    if (!isValidEmail(contact.email) || !isNoComma(contact.email)) {
        std::cerr << "Ошибка: Неверный email.\n";
        return false;
    }
    for (const auto& phone : contact.phoneNumbers) {
        if (!isValidPhone(phone)) {
            std::cerr << "Ошибка: Неверный номер телефона: " << phone << "\n";
            return false;
        }
    }
    return true;
}

std::string ContactValidator::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

bool ContactValidator::isValidName(const std::string& name) {
    static const QRegularExpression nameRegex("^[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ\\-\\s]*[a-zA-Zа-яА-ЯёЁ]$");
    QRegularExpressionMatch match = nameRegex.match(QString::fromStdString(name));
    return match.hasMatch();
}

bool ContactValidator::isValidPhone(const std::string& phone) {
    static const QRegularExpression phoneRegex(R"(^(\+7|8)\(?\d{3}\)?\d{3}[-]?\d{2}[-]?\d{2}$)");
    QRegularExpressionMatch match = phoneRegex.match(QString::fromStdString(phone));
    return match.hasMatch();
}

bool ContactValidator::isValidEmail(const std::string& email) {
    static const QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    QRegularExpressionMatch match = emailRegex.match(QString::fromStdString(email));
    return match.hasMatch();
}

bool ContactValidator::isNoComma(const std::string& address) {
    return address.find(',') == std::string::npos;
}

bool ContactValidator::isValidDate(const std::string& date) {
    static const QRegularExpression dateRegex(
        R"(^(\d{4})-(\d{2})-(\d{2})$)"
        );

    // Проверка формата
    QRegularExpressionMatch match = dateRegex.match(QString::fromStdString(date));
    if (!match.hasMatch()) {
        return false;
    }

    // Извлечение значений года, месяца и дня
    int year = match.captured(1).toInt();
    int month = match.captured(2).toInt();
    int day = match.captured(3).toInt();

    // Проверка диапазона месяца
    if (month < 1 || month > 12) {
        return false;
    }

    // Проверка диапазона дня
    static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int maxDay = daysInMonth[month - 1];

    // Учет високосного года для февраля
    if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
        maxDay = 29;
    }

    if (day < 1 || day > maxDay) {
        return false;
    }

    // Проверка на будущее
    QDate currentDate = QDate::currentDate();
    QDate inputDate(year, month, day);
    if (!inputDate.isValid() || inputDate > currentDate) {
        return false;
    }

    return true;
}

void PhoneBook::addContact(Contact contact, bool isNeedValidation) {
    trimFields(contact);

    if (isNeedValidation && !ContactValidator::chekFullContact(contact)) return;

    if (contact.phoneNumbers.empty()) {
        contact.phoneNumbers.push_back("NoPhone");
    }

    QFile file(QString::fromStdString(filename));

    // Открываем файл для записи в конец
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning("Не удалось открыть файл для записи");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Encoding::Utf8);

    out << QString::fromStdString(contact.firstName) << ","
        << QString::fromStdString(contact.lastName) << ","
        << QString::fromStdString(contact.middleName) << ","
        << QString::fromStdString(contact.address) << ","
        << QString::fromStdString(contact.birthDate) << ","
        << QString::fromStdString(contact.email) << ",";

    // Записываем номера телефонов, разделяя их точкой с запятой
    for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
        out << QString::fromStdString(contact.phoneNumbers[i]);
        if (i != contact.phoneNumbers.size() - 1) {
            out << ";";
        }
    }
    out << "\n";

    file.close();

    /*std::ofstream outFile(filename, std::ios::app);
    outFile << contact.firstName << "," << contact.lastName << "," << contact.middleName << ","
            << contact.address << "," << contact.birthDate << "," << contact.email << ",";
    for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
        outFile << contact.phoneNumbers[i];
        if (i != contact.phoneNumbers.size() - 1) {
            outFile << ";";
        }
    }
    outFile << std::endl;*/
}

void PhoneBook::saveAllContacts(const std::vector<Contact>& contacts) const {
    QFile file(QString::fromStdString(filename));

    // Открываем файл для записи
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Не удалось открыть файл для записи");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Encoding::Utf8);

    // Записываем каждый контакт
    for (const auto& contact : contacts) {
        out << QString::fromStdString(contact.firstName) << ","
            << QString::fromStdString(contact.lastName) << ","
            << QString::fromStdString(contact.middleName) << ","
            << QString::fromStdString(contact.address) << ","
            << QString::fromStdString(contact.birthDate) << ","
            << QString::fromStdString(contact.email) << ",";

        // Записываем номера телефонов, разделяя их точкой с запятой
        for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
            out << QString::fromStdString(contact.phoneNumbers[i]);
            if (i != contact.phoneNumbers.size() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    // Закрываем файл
    file.close();
    /*std::ofstream outFile(filename);

    for (const auto& contact : contacts) {
        outFile << contact.firstName << "," << contact.lastName << "," << contact.middleName << ","
                << contact.address << "," << contact.birthDate << "," << contact.email << ",";
        for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
            outFile << contact.phoneNumbers[i];
            if (i != contact.phoneNumbers.size() - 1) {
                outFile << ";";
            }
        }
        outFile << std::endl;
    }*/
}

void PhoneBook::deleteContact(size_t index) {
    std::vector<Contact> contacts = loadContacts();
    if (index < contacts.size()) {
        contacts.erase(contacts.begin() + index);
        saveAllContacts(contacts);
    }
}

void PhoneBook::searchContacts(const std::vector<std::string>& fields, const std::vector<std::string>& values) {
    std::vector<Contact> contacts = loadContacts();
    int counter = 1;
    const int columnWidth = 18;

    for (const auto& contact : contacts) {
        bool match = true;

        for (size_t i = 0; i < fields.size(); i++) {
            if (fields[i] == "firstName" && contact.firstName.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "lastName" && contact.lastName.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "middleName" && contact.middleName.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "address" && contact.address.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "birthDate" && contact.birthDate.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "email" && contact.email.find(values[i]) == std::string::npos) {
                match = false;
                break;
            }
            if (fields[i] == "phone" && !checkPhoneMatch(contact.phoneNumbers, values[i])) {
                match = false;
                break;
            }
        }

        if (match) {
            //displayContact(contact, counter++);
        }
    }
}

bool PhoneBook::checkPhoneMatch(const std::vector<std::string>& phoneNumbers, const std::string& phoneQuery) const {
    for (const auto& phone : phoneNumbers) {
        if (phone.find(phoneQuery) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<Contact> PhoneBook::loadContacts() const {
   /* std::vector<Contact> contacts;
    std::ifstream inFile(filename);
    std::string line;
    unsigned long long ID = 1;

    while (std::getline(inFile, line)) {
        Contact contact;
        size_t pos = 0;
        size_t field = 0;
        while ((pos = line.find(",")) != std::string::npos) {
            std::string token = line.substr(0, pos);
            switch (field) {
            case 0: contact.firstName = token; break;
            case 1: contact.lastName = token; break;
            case 2: contact.middleName = token; break;
            case 3: contact.address = token; break;
            case 4: contact.birthDate = token; break;
            case 5: contact.email = token; break;
            }
            line.erase(0, pos + 1);
            ++field;
        }
        contact.phoneNumbers = split(line, ";");
        contact.ID = ID++;
        contacts.push_back(contact);
    }
    return contacts;
*/
    std::vector<Contact> contacts;
    QFile inFile(QString::fromStdString(filename));

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Не удалось открыть файл: %s", qPrintable(inFile.errorString()));
        return contacts;
    }

    QTextStream in(&inFile);
    in.setEncoding(QStringConverter::Encoding::Utf8);

    QString line;
    unsigned long long ID = 1;

    while (in.readLineInto(&line)) {
        Contact contact;
        QStringList fields = line.split(',');

        if (fields.size() >= 6) { // Проверяем, что есть хотя бы 6 полей
            contact.firstName = fields[0].toStdString();
            contact.lastName = fields[1].toStdString();
            contact.middleName = fields[2].toStdString();
            contact.address = fields[3].toStdString();
            contact.birthDate = fields[4].toStdString();
            contact.email = fields[5].toStdString();
            if (fields.size() > 6) {
                contact.phoneNumbers = split(fields[6].toStdString(), ";");
            }
            contact.ID = ID++;
            contacts.push_back(contact);
        }
    }

    inFile.close();
    return contacts;
}

void PhoneBook::trimFields(Contact& contact) const {
    contact.firstName = ContactValidator::trim(contact.firstName);
    contact.lastName = ContactValidator::trim(contact.lastName);
    contact.middleName = ContactValidator::trim(contact.middleName);
    contact.address = ContactValidator::trim(contact.address);
    contact.birthDate = ContactValidator::trim(contact.birthDate);
    contact.email = ContactValidator::trim(contact.email);
    for (auto& phone : contact.phoneNumbers) {
        phone = ContactValidator::trim(phone);
    }
}

std::vector<std::string> PhoneBook::split(const std::string& str, const std::string& delimiter) const {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
        return tokens;
}
