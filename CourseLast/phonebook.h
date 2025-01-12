// PhoneBook.h
#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <map>
#include <optional>
#include <QObject>
#include <QTableView>

struct Contact {
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string address;
    std::string birthDate; // Format: YYYY-MM-DD
    std::string email;
    std::vector<std::string> phoneNumbers;
    unsigned long long ID;
};

class ContactValidator {
public:

    static bool chekFullContact(const Contact& contact);
    static std::string trim(const std::string& str);
    static bool isValidName(const std::string& name);
    static bool isValidPhone(const std::string& phone);
    static bool isValidEmail(const std::string& email);
    static bool isNoComma(const std::string& address);
    static bool isValidDate(const std::string& date);
};

class PhoneBook {
private:
    const std::string filename = "contacts.txt";
    bool checkPhoneMatch(const std::vector<std::string>& phoneNumbers, const std::string& phoneQuery) const;
    bool useDatabase;

public:
    PhoneBook() : useDatabase(false) {}

    void setUseDatabase(bool useDb) {
        useDatabase = useDb;
    }

    void trimFields(Contact& contact) const;
    void saveAllContacts(const std::vector<Contact>& contacts) const;
    std::vector<std::string> split(const std::string& str, const std::string& delimiter) const;
    void addContact(Contact contact, bool isNeedValidation = true);
    void printTitle(int columnWidth);
    void listContacts();
    void deleteContact(size_t index);
    void searchContacts(const std::vector<std::string>& fields, const std::vector<std::string>& values);
    std::vector<Contact> loadContacts() const;
};

#endif
