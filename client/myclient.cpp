#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "request/request.h"

void receive_from_server(int create_socket, char *buffer);

#define BUF 1024
#define PORT 6543

int main(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    char clientData[BUF];
    struct sockaddr_in address;
    int size;

    if (argc < 2) {
        printf("Usage: %s ServerAdresse\n", argv[0]);
        exit(EXIT_FAILURE);
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
            strcpy(buffer,request_send());
        }
        else if (strcmp(clientData, "LIST\n") == 0) {
            strcpy(buffer,request_list());
        }
        else if (strcmp(clientData, "READ\n") == 0) {
            strcpy(buffer,request_read());
        }
        else if (strcmp(clientData, "DEL\n") == 0) {
            strcpy(buffer,request_delete());
        }
        else if (strcmp(clientData, "QUIT\n") == 0) {
            strcpy(buffer,clientData);
            break;
        } else {
            printf("\nPLEASE ENTER VALID COMMAND TO CONTINUE:\nSEND--LIST--READ--DEL--QUIT\n");
            continue;
        }

        send(create_socket, buffer, strlen(buffer), 0);
        receive_from_server(create_socket, buffer);
    } while (strcmp(buffer, "QUIT\n") != 0);
    close(create_socket);
    return EXIT_SUCCESS;
}

void receive_from_server(int create_socket, char *buffer) {
    int size = recv(create_socket, buffer, BUF - 1, 0);
    if (size > 0) {
        buffer[size] = '\0';
        printf("%s", buffer);
    }

}
