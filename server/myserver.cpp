#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "handler/mail_handler.h"
#include "commons/thread_args.h"

#define BUF 1024
//#define PORT 6543



int main(int argc, char **argv) {
    std::string spoolPath;
    struct thread_args *Thread_input = (struct thread_args *)malloc(sizeof(struct thread_args));

    int PORT;
    if (argc != 3) {
        std::cout << "USAGE: PORT Filedirectory" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        PORT = atoi(argv[1]);
        spoolPath = argv[2];
    }

    int create_socket, new_socket;
    socklen_t addrlen;
    char buffer[BUF];
    int size;
    struct sockaddr_in address, cliaddress;

    create_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons (PORT);

    if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) != 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }
    listen(create_socket, 5);

    addrlen = sizeof(struct sockaddr_in);

    while (1) {
        printf("Waiting for connections...\n");

        if ((new_socket = accept(create_socket,
                                 (struct sockaddr *) &cliaddress, &addrlen)) == -1) {
            sleep(1);
            continue;
        }

        if (new_socket > 0) {
            printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
            strcpy(buffer, "Welcome to myserver, Please enter your command:\n");
            send(new_socket, buffer, strlen(buffer), 0);

            Thread_input->new_socket = &new_socket;
            Thread_input->path = spoolPath.c_str();

            pthread_t id;
            pthread_create(&id, NULL, handle_mail, (void *)Thread_input);
            pthread_join(id, NULL);
        }
    }
    close(create_socket);
    return 0;
}
