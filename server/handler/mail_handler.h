#ifndef SERVER_MAIL_HANDLER_H
#define SERVER_MAIL_HANDLER_H

#define MAXLINE 1500

#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include "../commons/string_code.h"
#include "../commons/messages.h"
#include "../commons/helper.h"
#include "../commons/thread_args.h"

void send_data(int sockfd, const char *data);

void mail_to_send(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_list(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_read(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_del(int client_sockfd, std::stringstream &strm, std::string path);

void respond_to_client(int client_sockfd, char *data, std::string path);

string_code hashit(char *);

#define BUF 1024

// process mailing events
void *handle_mail(void *params) {
    int client_sockfd, len;
    char buf[BUF];

    std::string localSpoolpath = ((struct thread_args*)params)->path;
    client_sockfd = *(((struct thread_args*)params)->new_socket);

    while (1) {
        memset(buf, 0, sizeof(buf));
        len = recv(client_sockfd, buf, (BUF - 1), 0);

        if (len > 0) {
            printf("Message received: \n%s\n", buf);
            respond_to_client(client_sockfd, buf, localSpoolpath);
        } else if (len == 0) {
            printf("Client closed remote socket\n");
            break;
        } else {
            perror("recv error");
        }
    }

    close(client_sockfd);

    return NULL;
}

void respond_to_client(int client_sockfd, char *data, std::string path) {
    char toCheck[4 + 1];
    std::string str;
    std::stringstream strm(data);
    std::getline(strm, str);
    strcpy(toCheck, str.c_str());

    switch (hashit(toCheck)) {
        case eSend :
            mail_to_send(client_sockfd, strm, path);
            break;
        case eList :
            mails_to_list(client_sockfd, strm, path);
            break;
        case eRead :
            mails_to_read(client_sockfd, strm, path);
            break;
        case eDel :
            mails_to_del(client_sockfd, strm, path);
            break;
        case eNone :
            break;
    }
}


void mail_to_send(int client_sockfd, std::stringstream &strm, std::string path) {
    // TODO - read further lines
    // TODO - check if recipient directory
    std::string sender, recipient, subject, message, user_dir;

    std::getline(strm,sender);
    std::getline(strm,recipient);
    // TODO find the recipient (the directory)
    std::getline(strm,subject);
    std::getline(strm,message);

    // TODO after finding the recipient also save sender in mail file
    if(check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(recipient.c_str())) == 1){
        user_dir = path +"/"+recipient;
    }else {
        user_dir = create_user_dir(recipient, path);
    }

    //in jedem fall ein user_dir in der hand
    //create file
    if(create_message_file(user_dir, sender,recipient,subject,message))
        send_data(client_sockfd, reply_code[6]);
    else
        send_data(client_sockfd, reply_code[28]);
}

void mails_to_list(int client_sockfd, std::stringstream &strm, std::string path){
    int count;
    std::string response;
    std::string all_subject;
    std::ifstream is_counter;
    std::string username;
    std::string user_dir;
    std::getline(strm,username);

    if(check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1){
        user_dir = path +"/"+username;
        is_counter.open(user_dir+"/counter");
        is_counter>>count;
        is_counter.close();

        all_subject = std::to_string(count)+"\n";
        /*
         * parameter user_dir: user_directory in spool
         * parameter counter: ignore the counter file as well as the "." and ".." dir in directory
         */
        all_subject.append(search_counter_file(user_dir.c_str(),"counter"));

        response = all_subject;

        send_data(client_sockfd, response.c_str());

    }else{
        send_data(client_sockfd, "0");
    }

}

void mails_to_read(int client_sockfd, std::stringstream &strm, std::string path){
    std::string response;
    std::string msg_number;
    std::string username;
    std::string user_dir;
    int msg_num;

    std::getline(strm,username);
    std::getline(strm,msg_number);
    msg_num = std::atoi(msg_number.c_str());

    if(check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1){
        user_dir = path +"/"+username;
        response.append(reply_code[6]);
        response.append(get_msg_content(user_dir.c_str(),"counter",msg_num));
        send_data(client_sockfd, response.c_str());
    }else{
        send_data(client_sockfd, reply_code[28]);
    }
}

void mails_to_del(int client_sockfd, std::stringstream &strm, std::string path){
    std::string response;
    std::string msg_number;
    std::string username;
    std::string user_dir;
    std::string file_path_to_del;
    int msg_num;

    std::getline(strm,username);
    std::getline(strm,msg_number);
    msg_num = std::atoi(msg_number.c_str());

    if(check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1){
        user_dir = path +"/"+username;
        file_path_to_del = get_msg_content(user_dir.c_str(),"counter",msg_num);

        if( remove( "myfile.txt" ) != 0 )
            send_data(client_sockfd, reply_code[28]);
        else
            send_data(client_sockfd, reply_code[6]);
    }else{
        send_data(client_sockfd, reply_code[28]);
    }
}


// send data by socket
void send_data(int sockfd, const char *data) {
    if (data != NULL) {
        send(sockfd, data, strlen(data), 0);
    }
}

string_code hashit(char *inString) {
    if (strcmp(inString, "SEND") == 0) return eSend;
    if (strcmp(inString, "LIST") == 0) return eList;
    if (strcmp(inString, "READ") == 0) return eRead;
    if (strcmp(inString, "DEL") == 0) return eDel;

    return eNone;
}

#endif //SERVER_MAIL_HANDLER_H
