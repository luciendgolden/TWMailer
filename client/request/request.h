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
    std::string final = "LIST\n";


    return &final[0];
}

char *request_send() {
    char myarray[642];
    std::string final, _sender, _recipient, _subject, _message;

    getUserInput(_sender, "Sender: ", 8);
    getUserInput(_recipient, "Recipient: ", 8);
    getUserInput(_subject, "Subject: ", 128);

    std::cout<<"Message: ";
    std::getline(std::cin >> std::ws, _message);

    final.append(_sender);
    final.append("\n");
    final.append(_recipient);
    final.append("\n");
    final.append(_subject);
    final.append("\n");
    final.append(_message);
    final.append("\n.\n");

    strcpy(myarray, final.c_str());

    return myarray;
}

void getUserInput(std::string &str, std::string name, int length) {
    do {
        std::cout<<name;
        std::cin >> str;
    } while (!validate(str, length));
}

bool validate(std::string str, int length) {
    if (str.length() > length) {
        return false;
    }

    return true;
}

#endif //CLIENT_REQUEST_H
