//
// Created by Alexander Tampier on 27.10.18.
//

#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H
#define MAX_BUF_LENGTH 642

#include <iostream>
#include <vector>
#include <cstring>
#include <regex>

char *request_send();

char *request_list();

char *request_read_or_del(std::string option);

void getUserInput(std::string &str, std::string name, int length);

bool validate(std::string str, int length);

char *request_login();


char *request_login() {
    char myarray[8 + 128 + 1];
    std::string final;
    std::string username;
    std::string password;

    getUserInput(username, "Username: ", 8);

    std::cout << "Password: ";
    std::getline(std::cin >> std::ws, password);

    final.append("LOGIN\n");
    final.append(username);
    final.append("\n");
    final.append(password);
    strcpy(myarray, final.c_str());

    return myarray;
}

char *request_read_or_del(std::string option) {
    // unsigned int + username + \n + \0
    char myarray[32 + 8 + 4 + 1];
    // 2 - 4 byte
    // 0 to 65,535 or
    // 0 to 4,294,967,295
    unsigned int msg_number;
    std::string final;
    std::string username;

    getUserInput(username, "Username: ", 8);


    std::cout << "Nachrichten-Nummer: ";
    while (!(std::cin >> msg_number)) {
        std::cout << "Nachrichten-Nummer: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    final.append(option + "\n");
    final.append(username);
    final.append("\n");
    final.append(std::to_string(msg_number));

    strcpy(myarray, final.c_str());

    return myarray;
}

char *request_list() {
    char myarray[15 + 1];
    std::string final;
    std::string username;

    getUserInput(username, "Username: ", 8);

    //TODO - check the input !!!!!

    final.append("LIST\n");
    final.append(username);
    final.append("\n");
    strcpy(myarray, final.c_str());

    return myarray;
}

char *request_send() {
    char myarray[642];
    std::string final, sender, recipient, subject, message;

    //TODO - check the input!!!!

    getUserInput(sender, "Sender: ", 8);
    getUserInput(recipient, "Recipient: ", 8);

    std::cout << "Subject: ";
    std::getline(std::cin >> std::ws, subject);

    final.append("SEND\n");
    final.append(sender);
    final.append("\n");
    final.append(recipient);
    final.append("\n");
    final.append(subject);

    std::cout << "Message: ";

    while (getline(std::cin, message)) {
        if (message == ".") {
            break;
        }
        final += "\n" + message;
    }

    strcpy(myarray, final.c_str());

    return myarray;
}

void getUserInput(std::string &str, std::string name, int length) {
    do {
        std::cout << name;
        std::cin >> str;
        fflush(stdout);
    } while (!validate(str, length));
}

bool validate(std::string str, int length) {

    std::regex rx("(if17b[0-9]{3})");
    std::match_results<std::string::const_iterator> mr;
    std::regex_search(str, mr, rx);

    if (!(str.length() > length) && (std::regex_match (str, std::regex("if17b[0-9]{3}")))) {
        return true;
    }
    else
        return false;
}


#endif //CLIENT_REQUEST_H
