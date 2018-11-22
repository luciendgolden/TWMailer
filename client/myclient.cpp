#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include "request/request.h"

void receive_from_server(int create_socket, char *buffer);

#define BUF 1024

std::string username;

int main(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    char clientData[BUF];
    struct sockaddr_in address;
    int size;
    int trash;
    int PORT;

    if (argc != 3) {
        printf("Usage: %s IP_ADDRESS PORT\n", argv[0]);
        exit(EXIT_FAILURE);
    } else {
        PORT = atoi(argv[2]);
    }

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (PORT);
    inet_aton(argv[1], &address.sin_addr);

    if (connect(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
        printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
        receive_from_server(create_socket, buffer);
    } else {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // login user
    // if user is not loged in
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        std::string result_code;

        strcpy(buffer, request_login());

        send(create_socket, buffer, strlen(buffer), 0);

        int size = recv(create_socket, buffer, BUF - 1, 0);

        result_code = buffer;

        if (size > 0) {
            buffer[size] = '\0';
            printf("%s", buffer);
        }

        std::string code = result_code.substr(0, 3);


        if(!code.compare("506")) {
            exit(EXIT_FAILURE);
        }else if(!code.compare("507")) {
            exit(EXIT_FAILURE);
        }else if (!code.compare("250")) {
            break;
        }
    }

    printf("SEND: Senden einer Nachricht vom Client zum Server.\n"
           "LIST: Auflisten der Nachrichten eines Users. Es soll die Anzahl der\n"
           "Nachrichten und pro Nachricht die Betreff Zeile angezeigt werden.\n"
           "READ: Anzeigen einer bestimmten Nachricht für einen User.\n"
           "DEL: Löschen einer Nachricht eines Users.\n"
           "QUIT: Logout des Clients\n");
    fflush(stdout);

    do {
        printf("Enter command: ");
        fflush(stdout);
        fgets(clientData, BUF, stdin);

        if (strcmp(clientData, "SEND\n") == 0) {
            strcpy(buffer, request_send());
        } else if (strcmp(clientData, "LIST\n") == 0) {
            strcpy(buffer, request_list());
        } else if (strcmp(clientData, "READ\n") == 0) {
            strcpy(buffer, request_read_or_del("READ"));
        } else if (strcmp(clientData, "DEL\n") == 0) {
            strcpy(buffer, request_read_or_del("DEL"));
        } else if (strcmp(clientData, "QUIT\n") == 0) {
            strcpy(buffer, clientData);
            break;
        } else {
            printf("\nPLEASE ENTER VALID COMMAND TO CONTINUE:\nSEND--LIST--READ--DEL--QUIT\n");
            continue;
        }

        send(create_socket, buffer, strlen(buffer), 0);
        receive_from_server(create_socket, buffer);

        /* discard all characters up to and including newline */
        while ((trash = getchar()) != '\n' && trash != EOF);
    } while (strcmp(buffer, "QUIT\n") != 0);
    close(create_socket);
    return EXIT_SUCCESS;
}


void receive_from_server(int create_socket, char *buffer) {
    int size = recv(create_socket, buffer, BUF - 1, 0);
    if (size > 0) {
        buffer[size] = '\0';
        printf("%s", buffer);
    } else if (size == 0) {
        printf("Server closed connection!");
        fflush(stdout);
        close(create_socket);
        exit(0);
    }
}
