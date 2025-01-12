#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <vector>
#include "phoneBook.h"

class DatabaseManager {
public:
    DatabaseManager() {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("contacts.db");

        if (!db.open()) {
            qWarning() << "Не удалось подключиться к базе данных:" << db.lastError().text();
        }

        createTables();
    }

    ~DatabaseManager() {
        db.close();
    }

    void insertContact(const Contact& contact) {
        QSqlQuery query;
        query.prepare(
            "INSERT INTO contacts (firstName, lastName, middleName, address, birthDate, email, phoneNumbers) "
            "VALUES (:firstName, :lastName, :middleName, :address, :birthDate, :email, :phoneNumbers)"
            );

        query.bindValue(":firstName", QString::fromStdString(contact.firstName));
        query.bindValue(":lastName", QString::fromStdString(contact.lastName));
        query.bindValue(":middleName", QString::fromStdString(contact.middleName));
        query.bindValue(":address", QString::fromStdString(contact.address));
        query.bindValue(":birthDate", QString::fromStdString(contact.birthDate));
        query.bindValue(":email", QString::fromStdString(contact.email));
        query.bindValue(":phoneNumbers", QString::fromStdString(join(contact.phoneNumbers, ", ")));

        if (!query.exec()) {
            qWarning() << "Ошибка при добавлении контакта:" << query.lastError().text();
        }
    }

    std::vector<Contact> loadContacts() {
        std::vector<Contact> contacts;

        QSqlQuery query("SELECT * FROM contacts");
        while (query.next()) {
            Contact contact;
            contact.ID = query.value("id").toULongLong();
            contact.firstName = query.value("firstName").toString().toStdString();
            contact.lastName = query.value("lastName").toString().toStdString();
            contact.middleName = query.value("middleName").toString().toStdString();
            contact.address = query.value("address").toString().toStdString();
            contact.birthDate = query.value("birthDate").toString().toStdString();
            contact.email = query.value("email").toString().toStdString();

            // Разделяем строку номеров телефонов на вектор
            QStringList phones = query.value("phoneNumbers").toString().split(", ");
            for (const QString& phone : phones) {
                contact.phoneNumbers.push_back(phone.toStdString());
            }

            contacts.push_back(contact);
        }

        return contacts;
    }

    void replaceAllContacts(const std::vector<Contact>& contacts) {
        QSqlQuery query;

        // Удаляем все записи из таблицы контактов
        if (!query.exec("DELETE FROM contacts")) {
            qWarning() << "Ошибка при очистке таблицы контактов:" << query.lastError().text();
            return;
        }

        for (const auto& contact : contacts) {
            insertContact(contact);
        }
    }

private:
    QSqlDatabase db;

    std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
        std::ostringstream result;
        for (size_t i = 0; i < elements.size(); ++i) {
            result << elements[i];
            if (i != elements.size() - 1) {
                result << delimiter;
            }
        }
        return result.str();
    }

    void createTables() {
        QSqlQuery query;
        query.exec(
            "CREATE TABLE IF NOT EXISTS contacts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "firstName TEXT, "
            "lastName TEXT, "
            "middleName TEXT, "
            "address TEXT, "
            "birthDate TEXT, "
            "email TEXT, "
            "phoneNumbers TEXT"
            ")"
            );
    }
};
