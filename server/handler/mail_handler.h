#ifndef SERVER_MAIL_HANDLER_H
#define SERVER_MAIL_HANDLER_H

#define MAXLINE 1500

#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <vector>
#include "../commons/string_code.h"
#include "../commons/messages.h"
#include "../commons/helper.h"
#include "../commons/thread_args.h"
#include "../commons/ldap.h"

pthread_mutex_t print_lock;

std::vector<struct in_addr> blacklist;

void send_data(int sockfd, const char *data);

void mail_to_send(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_list(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_read(int client_sockfd, std::stringstream &strm, std::string path);

void mails_to_del(int client_sockfd, std::stringstream &strm, std::string path);

void respond_to_client(int client_sockfd, char *data, std::string path);

void *handle_mail(void *params);

string_code hashit(char *);

#define BUF 1024

// process mailing events
void *handle_mail(void *params) {
    int client_sockfd, len;
    char buf[BUF];
    int loginAttempts = 1;

    std::string loged_in_sender;
    bool loged_in;

    struct sockaddr_in client_address = ((struct thread_args *) params)->client_address;
    std::string localSpoolpath = ((struct thread_args *) params)->path;
    client_sockfd = *(((struct thread_args *) params)->new_socket);

    while (1) {
        memset(buf, 0, sizeof(buf));

        if (client_sockfd <= 0) {
            return 0;
        }

        std::vector<struct in_addr>::const_iterator it;

        for(it = blacklist.begin(); it != blacklist.end(); it++)    {
            if (client_address.sin_addr.s_addr == it.base()->s_addr) {
                send_data(client_sockfd, reply_code[30]);
                close(client_sockfd);
                return 0;
            }
        }

        len = recv(client_sockfd, buf, (BUF - 1), 0);


        if (len > 0) {
            printf("Message received: \n%s\n", buf);

            if (!loged_in) {
                pthread_mutex_lock(&print_lock);
                //check buffer credentials with ldap-server
                std::string username;
                std::string password;
                std::string login;
                std::stringstream strm(buf);
                std::getline(strm, login);
                std::getline(strm, username);
                std::getline(strm, password);


                //add counter for login trys
                //when counter bigger than 3 oder so
                // add die client address auf einen vector of client addresses (blacklist)

                if (loginAttempts >= 3) {
                    send_data(client_sockfd, reply_code[29]);
                    blacklist.push_back(client_address.sin_addr);
                    close(client_sockfd);
                    pthread_mutex_unlock(&print_lock);
                    return 0;
                }

                if(my_login(username, password)){
                    send_data(client_sockfd, reply_code[6]);
                    loged_in = true;
                    pthread_mutex_unlock(&print_lock);
                    continue;
                }else {
                    //err
                    send_data(client_sockfd, reply_code[19]);
                    pthread_mutex_unlock(&print_lock);
                    loginAttempts++;
                }

                /*
                if (std::strcmp(username.c_str(), "if17b052") == 0 &&
                    std::strcmp(password.c_str(), "abcd") == 0) {
                    //success
                    send_data(client_sockfd, reply_code[6]);
                    loged_in = true;
                    pthread_mutex_unlock(&print_lock);
                    continue;
                } else {
                    //err
                    send_data(client_sockfd, reply_code[19]);
                    pthread_mutex_unlock(&print_lock);
                    loginAttempts++;
                }
                */
            } else {
                // if user loged in give sender
                respond_to_client(client_sockfd, buf, localSpoolpath);
            }
        } else if (len == 0) {
            printf("Client closed remote socket\n");
            break;
        } else {
            perror("recv error");
        }
    }
    close(client_sockfd);
    return 0;
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
    pthread_mutex_lock(&print_lock);
    std::string sender;
    std::string recipient;
    std::string subject;
    std::string message;
    std::string user_dir;
    std::string line;

    std::getline(strm, sender);
    std::getline(strm, recipient);
    // TODO find the recipient (the directory)
    std::getline(strm, subject);
    // TODO after finding the recipient also save sender in mail file
    if (check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(recipient.c_str())) == 1) {
        user_dir = path + "/" + recipient;
    } else {
        user_dir = create_user_dir(recipient, path);
    }

    //in jedem fall ein user_dir in der hand
    //create file
    if (create_message_file(user_dir, sender, recipient, subject, strm))
        send_data(client_sockfd, reply_code[6]);
    else
        send_data(client_sockfd, reply_code[28]);

    pthread_mutex_unlock(&print_lock);
}

void mails_to_list(int client_sockfd, std::stringstream &strm, std::string path) {
    pthread_mutex_lock(&print_lock);
    int count;
    std::string response;
    std::string all_subject;
    std::ifstream is_counter;
    std::string username;
    std::string user_dir;
    std::getline(strm, username);

    if (check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1) {
        user_dir = path + "/" + username;
        is_counter.open(user_dir + "/counter");
        is_counter >> count;
        is_counter.close();

        all_subject = std::to_string(count) + "\n";
        /*
         * parameter user_dir: user_directory in spool
         * parameter counter: ignore the counter file as well as the "." and ".." dir in directory
         */
        all_subject.append(search_counter_file(user_dir.c_str(), "counter"));

        response = all_subject;

        send_data(client_sockfd, response.c_str());

    } else {
        send_data(client_sockfd, "0");
    }

    pthread_mutex_unlock(&print_lock);
}

void mails_to_read(int client_sockfd, std::stringstream &strm, std::string path) {
    pthread_mutex_lock(&print_lock);
    std::string response;
    std::string msg_number;
    std::string username;
    std::string user_dir;
    int msg_num;

    std::getline(strm, username);
    std::getline(strm, msg_number);
    msg_num = std::atoi(msg_number.c_str());

    if (check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1) {
        user_dir = path + "/" + username;
        response.append(reply_code[6]);
        response.append(get_msg_content(user_dir.c_str(), "counter", msg_num));
        send_data(client_sockfd, response.c_str());
    } else {
        send_data(client_sockfd, reply_code[28]);
    }
    pthread_mutex_unlock(&print_lock);
}

void mails_to_del(int client_sockfd, std::stringstream &strm, std::string path) {
    pthread_mutex_lock(&print_lock);
    int msg_num;
    int count;
    std::string response;
    std::string msg_number;
    std::string username;
    std::string user_dir;
    std::string file_path_to_del;
    std::ifstream is_counter;

    std::getline(strm, username);
    std::getline(strm, msg_number);
    msg_num = std::atoi(msg_number.c_str());

    // find directory
    if (check_for_user_dir(path.c_str(), 0, 1, const_cast<char *>(username.c_str())) == 1) {
        user_dir = path + "/" + username;

        is_counter.open(user_dir + "/counter");
        is_counter >> count;
        is_counter.close();

        if (msg_num <= count && msg_num != 0) {
            file_path_to_del = get_file_name_to_del(user_dir.c_str(), "counter", msg_num);
        } else {
            send_data(client_sockfd, reply_code[28]);
            return;
        }

        if (remove(file_path_to_del.c_str()) != 0) {
            send_data(client_sockfd, reply_code[28]);
        } else {
            send_data(client_sockfd, reply_code[6]);
            // decrement counter in counter file
            std::ofstream out_counter;

            count--;
            out_counter.open(user_dir + "/counter");
            out_counter << count;
            out_counter.flush();
            out_counter.close();
        }
    } else {
        send_data(client_sockfd, reply_code[28]);
    }
    pthread_mutex_unlock(&print_lock);
}


// send data by socket
void send_data(int sockfd, const char *data) {
    if (data != NULL) {
        //todo - return value
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

static ssize_t
my_read(int fd, char *ptr) {
    static int read_cnt = 0;
    static char *read_ptr;
    static char read_buf[MAXLINE];
    if (read_cnt <= 0) {
        again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return (-1);
        } else if (read_cnt == 0)
            return (0);
        read_ptr = read_buf;
    };
    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;
    ptr = static_cast<char *>(vptr);
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return (0);
            else
                break;
        } else
            return (-1);
    };
    *ptr = 0;
    return (n);
}

#endif //SERVER_MAIL_HANDLER_H
