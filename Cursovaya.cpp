#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <Windows.h>
#include <map>
#include <optional>

std::string currentSortField;

struct Contact {
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string address;
    std::string birthDate; // Format: YYYY-MM-DD
    std::string email;
    std::vector<std::string> phoneNumbers;
    unsigned long long ID;

    //Contact& operator=(const Contact& other) {
    //    if (this != &other) { // Проверка на самоприсваивание
    //        firstName = other.firstName;
    //        lastName = other.lastName;
    //        middleName = other.middleName;
    //        address = other.address;
    //        birthDate = other.birthDate;
    //        email = other.email;
    //        phoneNumbers = other.phoneNumbers;
    //        ID = other.ID;
    //    }
    //    return *this;  // Возвращаем текущий объект для цепочки присваиваний
    //}
};

class ContactValidator {
public:
    static bool chekFullContact(const Contact& contact) {
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
        if (!isValidEmail(contact.email) or !isNoComma(contact.email)) {
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

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    static bool isValidName(const std::string& name) {
        std::regex nameRegex("^[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ\\-\\s]*[a-zA-Zа-яА-ЯёЁ]$");
        return std::regex_match(name, nameRegex);
    }

    static bool isValidPhone(const std::string& phone) {
        std::regex phoneRegex(R"(^(\+7|8)\(?\d{3}\)?\d{3}[-]?\d{2}[-]?\d{2}$)");
        return std::regex_match(phone, phoneRegex);
    }

    static bool isValidEmail(const std::string& email) {
        std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return std::regex_match(email, emailRegex);
    }

    static bool isNoComma(const std::string& address) {
        // Проверка на отсутствие запятых в адресе
        if (address.find(',') != std::string::npos) {
            return false;
        }
        return true;
    }

    static bool isValidDate(const std::string& date) {
        std::regex dateRegex("^(\\d{4})-(\\d{2})-(\\d{2})$");
        std::smatch match;
        if (!std::regex_match(date, match, dateRegex)) {
            return false;
        }

        int year = std::stoi(match[1]);
        int month = std::stoi(match[2]);
        int day = std::stoi(match[3]);

        if (month < 1 || month > 12) return false;

        static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int maxDay = daysInMonth[month - 1];

        // Leap year check
        if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
            maxDay = 29;
        }

        if (day < 1 || day > maxDay) return false;

        // Check if date is in the past
        std::time_t t = std::time(nullptr);
        std::tm* currentDate = std::localtime(&t);
        int currentYear = currentDate->tm_year + 1900;
        int currentMonth = currentDate->tm_mon + 1;
        int currentDay = currentDate->tm_mday;

        if (year > currentYear || (year == currentYear && month > currentMonth) ||
            (year == currentYear && month == currentMonth && day > currentDay)) {
            return false;
        }

        return true;
    }
};

class PhoneBook {
private:
    const std::string filename = "contacts.txt";

public:
    void addContact(Contact contact, bool isNeedValidation = true) {
        trimFields(contact);

        if(isNeedValidation)
            if (!ContactValidator::chekFullContact(contact)) return;

        if (contact.phoneNumbers.empty()) {
            contact.phoneNumbers.push_back("NoPhone");
        }

        std::ofstream outFile(filename, std::ios::app);
        outFile << contact.firstName << "," << contact.lastName << "," << contact.middleName << ","
            << contact.address << "," << contact.birthDate << "," << contact.email << ",";
        for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
            outFile << contact.phoneNumbers[i];
            if (i != contact.phoneNumbers.size() - 1) {
                outFile << ";";
            }
        }
        outFile << std::endl;
    }

    void printTitle(int columnWidth) {
        std::cout << std::setw(3) << std::left << "№" << " "
            << std::setw(4) << std::left << "ID"
            << std::setw(columnWidth) << std::left << "Фамилия"
            << std::setw(columnWidth) << std::left << "Имя"
            << std::setw(columnWidth) << std::left << "Отчество"
            << std::setw(columnWidth) << std::left << "Email"
            << std::setw(columnWidth) << std::left << "Дата рождения"
            << std::setw(columnWidth) << std::left << "Адрес"
            << std::setw(15) << std::left << "Телефоны" << std::endl;

        std::cout << std::setw(3) << std::left << "---"
            << std::setw(columnWidth) << std::left << "---------"
            << std::setw(columnWidth) << std::left << "----"
            << std::setw(columnWidth) << std::left << "--------"
            << std::setw(columnWidth) << std::left << "-----"
            << std::setw(columnWidth) << std::left << "------------"
            << std::setw(columnWidth) << std::left << "------"
            << std::setw(15) << std::left << "--------" << std::endl;
    }

    void listContacts() {
        std::vector<Contact> contacts = loadContacts();

        sortContacts(contacts);

        const int columnWidth = 18;  // Ширина столбца для выравнивания

        // Заголовок
        printTitle(columnWidth);

        // Вывод данных контактов
        for (size_t i = 0; i < contacts.size(); ++i) {
            displayContact(contacts[i], i+1);
        }
        std::cout << std::endl;
    }

    void deleteContact(size_t index) {
        std::vector<Contact> contacts = loadContacts();
        if (index < contacts.size()) {
            contacts.erase(contacts.begin() + index);
            saveAllContacts(contacts);
        }
    }

    void editContact(size_t index) {
        std::vector<Contact> contacts = loadContacts();
        if (index >= contacts.size()) {
            std::cout << "Неверный индекс!";
            return;
        }

        std::cout << "\nРедактируемый контакт: \n";
        printTitle(18);
        displayContact(contacts[index], 0);
        Contact newContact = contacts[index];

        std::cout << "\nВыберите поля для редактирования:\n";
        std::cout << "1. Имя\n";
        std::cout << "2. Фамилия\n";
        std::cout << "3. Отчество\n";
        std::cout << "4. Адрес\n";
        std::cout << "5. Дата рождения\n";
        std::cout << "6. Email\n";
        std::cout << "7. Телефон\n";
        std::cout << "0. Сохранить\n\n";

        while (true) {
            int index;
            
            std::cout << "Номер поля для изменения: ";

            std::cin >> index;

            switch (index) {
            case 0:
            {
                if (ContactValidator::chekFullContact(newContact)) {
                    contacts[index] = newContact;
                    saveAllContacts(contacts);
                    return;
                }
            }
            break;
            case 1:
            {
                std::cout << "Введите имя: ";
                std::cin >> newContact.firstName;
            }
            break;
            case 2:
            {
                std::cout << "Введите фамилию: ";
                std::cin >> newContact.lastName;
            }
            break;
            case 3:
            {
                std::cout << "Введите отчество (или оставьте пустым): ";
                std::cin.ignore(); // Сбросить оставшийся символ новой строки
                std::getline(std::cin, newContact.middleName);
            }
            break;
            case 4:
            {
                std::cout << "Введите адрес: ";
                std::getline(std::cin, newContact.address);
            }
            break;
            case 5:
            {
                std::cout << "Введите дату рождения (формат YYYY-MM-DD): ";
                std::getline(std::cin, newContact.birthDate);
            }
            break;
            case 6:
            {
                std::cout << "Введите email: ";
                std::getline(std::cin, newContact.email);
            }
            break;
            case 7:
            {
                newContact.phoneNumbers.clear();
                std::cout << "Введите номера телефонов (0 - закончить ввод):\n";
                while (true) {
                    std::string phone;
                    std::getline(std::cin, phone);  // Ввод строки с номерами телефонов
                    if (phone == "0") {
                        newContact.phoneNumbers.erase(newContact.phoneNumbers.begin());
                        break;
                    }
                    newContact.phoneNumbers.push_back(phone);  // Добавляем каждый телефон в вектор
                }
            }
            break;
            default:
                std::cout << "Неверный ввод!\n";
                break;
            }
        }
    }

    void sortContacts(std::vector<Contact>& contacts) {
        if (currentSortField == "firstName") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.firstName < b.firstName;
                });
        }
        else if (currentSortField == "lastName") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.lastName < b.lastName;
                });
        }
        else if (currentSortField == "middleName") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.middleName < b.middleName;
                });
        }
        else if (currentSortField == "address") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.address < b.address;
                });
        }
        else if (currentSortField == "birthDate") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.birthDate < b.birthDate;
                });
        }
        else if (currentSortField == "email") {
            std::sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
                return a.email < b.email;
                });
        }
        else {
            std::cerr << "Ошибка: Неверное поле для сортировки.\n";
            return;
        }

        //std::cout << "Контакты отсортированы по полю: " << currentSortField << std::endl;
    }

    void setSortFields(int index) {
        std::string variants[6] = { "firstName", "lastName", "middleName","address", "birthDate", "email" };
        currentSortField = variants[index];
        std::cout << "Контакты отсортированы по полю: " << currentSortField << std::endl;
    }

    void searchContacts(const std::vector<std::string>& fields, const std::vector<std::string>& values) {
        std::vector<Contact> contacts = loadContacts();
        int counter = 1;
        const int columnWidth = 18;

        printTitle(columnWidth);

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
                //std::cout << "Pass" << std::endl;
                displayContact(contact, counter++);
            }
        }
    }

    void displayContact(const Contact& contact, int index) const {
        const int columnWidth = 18;

        std::cout << std::setw(3) << std::left << index << " "
            << std::setw(4) << std::left << contact.ID
            << std::setw(columnWidth) << std::left << contact.lastName
            << std::setw(columnWidth) << std::left << contact.firstName
            << std::setw(columnWidth) << std::left << contact.middleName
            << std::setw(columnWidth) << std::left << contact.email
            << std::setw(columnWidth) << std::left << contact.birthDate
            << std::setw(columnWidth) << std::left << contact.address;

        if (contact.phoneNumbers.empty()) {
            std::cout << std::setw(15) << std::left << "No phone";
        }
        else {
            for (size_t j = 0; j < contact.phoneNumbers.size(); ++j) {
                std::cout << contact.phoneNumbers[j];
                if (j < contact.phoneNumbers.size() - 1) {
                    std::cout << ", ";
                }
            }
        }
        std::cout << std::endl;
    }


private:
    bool checkPhoneMatch(const std::vector<std::string>& phoneNumbers, const std::string& phoneQuery) const {
        for (const auto& phone : phoneNumbers) {
            if (phone.find(phoneQuery) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    void saveAllContacts(const std::vector<Contact>& contacts) const {
        std::ofstream outFile(filename);
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
        }
    }

    void trimFields(Contact& contact) const {
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

    std::vector<Contact> loadContacts() const {
        std::vector<Contact> contacts;
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
            contact.ID = ID; ID++;
            contacts.push_back(contact);
        }
        return contacts;
    }

    static std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
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
 
};


void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");
    PhoneBook phoneBook;
    int choice;
    bool flagToStop = false;

    // Add some initial data
    //phoneBook.addContact({ "Иван", "Иванов", "Иванович", "ул. Ленина 10", "2020-01-01", "ivanov@mail.ru", {"+78121234567", "+7(812)1234567"} });

    while (true) {
        flagToStop = false;
        clearScreen();

        // Display the contact list
        phoneBook.listContacts();

        // Display available actions
        std::cout << "\nВыберите действие:\n";
        std::cout << "1. Добавить контакт\n";
        std::cout << "2. Удалить контакт\n";
        std::cout << "3. Изменить контакт\n";
        std::cout << "4. Поиск контактов\n";
        std::cout << "5. Сортировка\n";
        std::cout << "0. Выход\n";

        // Get the user's choice
        std::cin >> choice;

        if (choice == 0) {
            break;
        }

        switch (choice) {
        case 1:
            // Add a new contact
        {
            Contact newContact;
            bool isContactValid = false;

            while (!isContactValid) {
                std::cout << "Введите имя: ";
                std::cin >> newContact.firstName;

                std::cout << "Введите фамилию: ";
                std::cin >> newContact.lastName;

                std::cout << "Введите отчество (или оставьте пустым): ";
                std::cin.ignore(); // Сбросить оставшийся символ новой строки
                std::getline(std::cin, newContact.middleName);

                std::cout << "Введите адрес: ";
                std::getline(std::cin, newContact.address);

                std::cout << "Введите дату рождения (формат YYYY-MM-DD): ";
                std::getline(std::cin, newContact.birthDate);

                std::cout << "Введите email: ";
                std::getline(std::cin, newContact.email);

                std::cout << "Введите номера телефонов (0 - закончить ввод): \n";
                while (true) {
                    std::string phone;
                    std::getline(std::cin, phone);  // Ввод строки с номерами телефонов
                    if (phone == "0") break;
                    newContact.phoneNumbers.push_back(phone);  // Добавляем каждый телефон в вектор
                }

                // Проверяем валидность контакта
                isContactValid = ContactValidator::chekFullContact(newContact);

                if (isContactValid) {
                    phoneBook.addContact(newContact, false);
                    std::cout << "Контакт успешно добавлен!\n";
                }
                else {
                    std::cout << "Пожалуйста, введите корректные данные.\n";
                }
            }
        }
        break;
        case 2:
            // Delete a contact (ask for index)
        {
            size_t indexToDelete;
            std::cout << "Введите ID контакта для удаления: ";
            std::cin >> indexToDelete;
            phoneBook.deleteContact(indexToDelete - 1);
        }
        break;
        case 3:
            // Edit a contact (ask for index and new data)
        {
            size_t indexToEdit;
            std::cout << "Введите индекс контакта для редактирования: ";
            std::cin >> indexToEdit;   
            phoneBook.editContact(indexToEdit - 1);
            flagToStop = true;
        }
        break;
        case 4:
        {
            std::cout << "\nВыберите поля для поиска:\n";
            std::cout << "1. Имя\n";
            std::cout << "2. Фамилия\n";
            std::cout << "3. Отчество\n";
            std::cout << "4. Адрес\n";
            std::cout << "5. Дата рождения\n";
            std::cout << "6. Email\n";
            std::cout << "7. Телефон\n";

            std::vector<std::string> fields;
            std::vector<std::string> values;
            int fieldChoice;
            std::string value;

            while (true) {
                std::cout << "Введите номер поля для поиска (0 - начать поиск): ";
                std::cin >> fieldChoice;
                if (fieldChoice == 0) break;

                std::cout << "Введите значение для поиска: ";
                std::cin >> value;

                switch (fieldChoice) {
                case 1: fields.push_back("firstName"); break;
                case 2: fields.push_back("lastName"); break;
                case 3: fields.push_back("middleName"); break;
                case 4: fields.push_back("address"); break;
                case 5: fields.push_back("birthDate"); break;
                case 6: fields.push_back("email"); break;
                case 7: fields.push_back("phone"); break;
                default: std::cout << "Неверный выбор.\n"; continue;
                }

                values.push_back(value);
            }

            phoneBook.searchContacts(fields, values);
            flagToStop = true;
        }
        break;
        case 5:
            // Sort contacts by last name
        {
            int index = 0;
            std::cout << "\nВыберите поля для сортировки:\n";
            std::cout << "1. Имя\n";
            std::cout << "2. Фамилия\n";
            std::cout << "3. Отчество\n";
            std::cout << "4. Адрес\n";
            std::cout << "5. Дата рождения\n";
            std::cout << "6. Email\n";;

            std::cin >> index;

            if(index < 7 && index > 0)
                phoneBook.setSortFields(index - 1);
        }
        break;
        default:
            std::cout << "Неверный выбор! Попробуйте снова.\n";
        }

        // Wait for user to press enter before continuing
        if (flagToStop) {
            std::cout << "\nНажмите Enter для продолжения работы.\n";
            std::cin.ignore();
            std::cin.get();
        }
    }

   // return 0;
}

