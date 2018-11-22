#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csignal>
#include <pthread.h>
#include <vector>
#include "handler/mail_handler.h"
#include "commons/thread_args.h"

#define BUF 1024
//#define PORT 6540

void myhandler(int sig);

int server_sockfd;

//Vektor of client socks
std::vector<int> clientSockVector;

//Thread Vektor
std::vector<pthread_t> threadVector;

extern pthread_mutex_t print_lock;

extern std::vector<struct in_addr> blacklist;

//signal handler
void myhandler(int sig) {
    printf("\nS:Caught signal (%d). Mail server shutting down...\n\n", sig);
    fflush(stdout);


    //joining all threads before

    if (threadVector.size() > 0) {
        for (auto &vec :threadVector) {
            pthread_cancel(vec);
            printf("\nCanceled a thread");
            fflush(stdout);
        }
    }

    //Vektor über clientsockets alle closen
    if (clientSockVector.size() > 0) {
        for (auto &&client :clientSockVector) {
            if (client > 0) {
                printf("\nclosed a client socket");
                fflush(stdout);
                close(client);
            }
        }
    }


    close(server_sockfd);
    //pthread_exit(NULL);
    exit(sig);
}


int main(int argc, char **argv) {
    std::string spoolPath;
    struct thread_args *Thread_input = (struct thread_args *) malloc(sizeof(struct thread_args));

    int PORT;
    if (argc != 3) {
        std::cout << "USAGE: PORT SPOOL_FILE_PATH" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        PORT = atoi(argv[1]);
        spoolPath = argv[2];
    }

    int client_sockfd;
    socklen_t addrlen;
    char buffer[BUF];
    struct sockaddr_in server_address, client_address;

    //create socket
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("S:socket create error！\n");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons (PORT);

    //create a link
    if (bind(server_sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    //listening requests from clients
    listen(server_sockfd, 5);

    addrlen = sizeof(struct sockaddr_in);


    //ausserhald der while vector/array of thread inizialisieren

    while (true) { // Main thread will listen continously
        printf("Waiting for connections...\n");

        if ((client_sockfd = accept(server_sockfd,
                                    (struct sockaddr *) &client_address, &addrlen)) == -1) {
            sleep(1);
            continue;
        }

        if (client_sockfd > 0) {
            printf("Client connected from %s:%u...\n", inet_ntoa(client_address.sin_addr),
                   ntohs(client_address.sin_port));
            strcpy(buffer, "Welcome to myserver, Please enter your command:\n");
            send(client_sockfd, buffer, strlen(buffer), 0);

            Thread_input->client_address = client_address;
            Thread_input->new_socket = &client_sockfd;
            Thread_input->path = spoolPath.c_str();

            clientSockVector.push_back(client_sockfd);

            //creating threads
            pthread_t id;

            if (pthread_mutex_init(&print_lock, NULL) != 0) {
                printf("\n mutex init failed\n");
                return 1;
            }
            pthread_mutex_init(&print_lock, NULL);

            pthread_mutex_init(&print_lock, NULL);
            pthread_create(&id, NULL, handle_mail, (void *) Thread_input);
            //adding threads to threadVector
            threadVector.push_back(id);

        }

        //Signal handler call!
        (void) signal(SIGINT, myhandler);
    }

    pthread_mutex_destroy(&print_lock);
    pthread_exit(NULL);
    return 0;
}