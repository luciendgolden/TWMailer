//
// Created by Alexander Tampier on 26.10.18.
//
#include "../commons/string_code.h"
#include <regex>


void send_data(int y, const char * str);

string_code hashit(char *);

void mail_to_send(int x, char * str);

#ifndef SERVER_MAIL_HANDLER_H
#define SERVER_MAIL_HANDLER_H

#define BUF 1024

string_code hashit(char *inString) {
    if (strcmp(inString, "SEND\n") == 0) return eSend;
}

// process mailing events
void *handle_mail(void *param) {
    int client_sockfd, len, status;
    char buf[BUF];

    memset(buf, 0, sizeof(buf));
    client_sockfd = *(int *) param;

    do {
        memset(buf, 0, sizeof(buf));


        len = recv(client_sockfd, buf, (BUF - 1), 0);

        if (len > 0) {
            buf[BUF] = '\0';
            printf("Message received: %s\n", buf);

            switch (hashit(buf)) {
                case eSend :
                    mail_to_send(client_sockfd, buf);
                    break;       // and exits the switch
            }
        } else if (len == 0) {
            printf("Client closed remote socket\n");
            break;
        } else {
            perror("recv error");
        }
    } while (strncmp(buf, "QUIT", 4) != 0);
    close(client_sockfd);
}

void mail_to_send(int sockfd, char *data) {
    int status=0;
    char sender[BUF];
    std::regex my_regex("if17b[0-9]{3}");

    send_data(sockfd, "Sender: ");

    int len = recv(sockfd, data, (BUF - 1), 0);
    //rec USEr
    if (len > 0){
        strcpy(sender,data);

        if(regex_match(sender,my_regex)){
            printf("regex match");
        }
    }

    if(status==1){
        send_data(sockfd, "200 OK");
    }else{
        send_data(sockfd, "500 ERR");
    }
}

// send data by socket
void send_data(int sockfd, const char *data) {
    if (data != NULL) {
        send(sockfd, data, strlen(data), 0);
    }
}

#endif //SERVER_MAIL_HANDLER_H
