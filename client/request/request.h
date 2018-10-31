//
// Created by Alexander Tampier on 27.10.18.
//

#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H
#define MAX_BUF_LENGTH 642

#include <iostream>
#include <vector>
#include <cstring>

char *request_send();

char *request_list();

char *request_read();

char *request_delete();

void getUserInput(std::string &str, std::string name, int length);

bool validate(std::string str, int length);

char *request_delete() {

    return "DEL\n";
}

char *request_read() {

    return "READ\n";
}

char *request_list() {
    char myarray[16];
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

    std::cout<<"Subject: ";
    std::getline(std::cin >> std::ws, subject);

    std::cout<<"Message: ";
    std::getline(std::cin >> std::ws, message);

    final.append("SEND\n");
    final.append(sender);
    final.append("\n");
    final.append(recipient);
    final.append("\n");
    final.append(subject);
    final.append("\n");
    final.append(message);
    final.append("\n.\n");

    strcpy(myarray, final.c_str());

    return myarray;
}

void getUserInput(std::string &str, std::string name, int length) {
    do {
        std::cout<<name;
        std::cin >> str;
        fflush(stdout);
    } while (!validate(str, length));
}

bool validate(std::string str, int length) {
    if (str.length() > length) {
        return false;
    }

    return true;
}

#endif //CLIENT_REQUEST_H
