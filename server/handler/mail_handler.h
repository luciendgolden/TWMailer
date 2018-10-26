#ifndef SERVER_MAIL_HANDLER_H
#define SERVER_MAIL_HANDLER_H

#include <unistd.h>
#include <errno.h>
#include "../commons/string_code.h"
#include "../commons/messages.h"

void send_data(int, const char *);
void mail_to_send(int, char *);
string_code hashit(char *);
void respond_to_client(int sockfd, char buf[1024]);

#define BUF 1024

// process mailing events
void *handle_mail(void *param) {
    int client_sockfd, len;
    char buf[BUF];

    client_sockfd = *(int *) param;

    while(1){
        memset(buf, 0, sizeof(buf));
        len = recv(client_sockfd, buf, (BUF - 1), 0);

        if (len > 0) {
            printf("Message received: %s\n", buf);
           respond_to_client(client_sockfd, buf);
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

void respond_to_client(int client_sockfd, char *data) {
    send_data(client_sockfd, reply_code[6]);
    switch (hashit(data)) {
        case eSend :
            mail_to_send(client_sockfd, data);
            break;
        case eList :
            break;
        case eRead :
            break;
        case eDel :
            break;
        case eNone :
            break;
    }
}


void mail_to_send(int client_sockfd, char *data) {
    // TODO - check if mail can be sent to the recipient
}

// send data by socket
void send_data(int sockfd, const char *data) {
    if (data != NULL) {
        send(sockfd, data, strlen(data), 0);
    }
}

string_code hashit(char *inString) {
    if (strcmp(inString, "SEND\n") == 0) return eSend;
    if (strcmp(inString, "LIST\n") == 0) return eList;
    if (strcmp(inString, "READ\n") == 0) return eRead;
    if (strcmp(inString, "DEL\n") == 0) return eDel;

    return eNone;
}

// read a ‘\n‘ terminated line from a descriptor, char by char
ssize_t readline(void *vptr, int fd, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;
    //ptr = vptr ;
    for (n = 1; n < maxlen; n++) {
        again:
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return (0);
            else break;
        } else {
            if (errno == EINTR)
                goto again;
            return (-1);
        };
    };
// newline ist stored, like fgets()
// EOF, no data read
// EOF, some data was read
// error, errno set by read()
// null terminate like fgets()
    *ptr = 0;
    return (n);
}

#endif //SERVER_MAIL_HANDLER_H
