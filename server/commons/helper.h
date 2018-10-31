//
// Created by Alexander Tampier on 30.10.18.
//

#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

int check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind);

std::string create_user_dir(std::string recipient, std::string path);

void create_message_file(std::string sender, std::string recipient, std::string subject, std::string message);


/**
 * Search for a specific file in a given directory
 * @param searchPath
 * @param localRecursive
 * @param localIgnoreCapitalization
 * @param myLocalFileToFind
 * @return
 */
int check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind) {
    //struct for directory operations
    struct dirent *direntp;
    DIR *dirp;

    //errorhandling directory open
    if (!(dirp = opendir(searchPath))) {
        perror("Failed to open directory");
        return 0;
    }

    //dir opened, while until EOF dir
    //readdir -> reads next/current file
    while ((direntp = readdir(dirp)) != NULL) {
        //if -i is true (strCASEcmp)
        if (localIgnoreCapitalization != 0) {

            //comparing local file to find with the opened directories files
            if (strcasecmp(direntp->d_name, myLocalFileToFind) == 0) {
                while ((closedir(dirp) == -1) && (errno == EINTR));

                return 1;
            }

            //same without -i (not true)
        } else if (strcmp(myLocalFileToFind, direntp->d_name) == 0) {
            printf("++Process: %d -> Found File directory: %s/%s\n", getpid(), searchPath,
                   direntp->d_name);
            fflush(stdout);
            while ((closedir(dirp) == -1) && (errno == EINTR));

            return 1;
        }

        if (direntp->d_type == DT_DIR && localRecursive && direntp->d_name[0] != '.' && direntp->d_name[1] != '.') {
            char newPath[1024];
            snprintf(newPath, sizeof(newPath), "%s/%s", searchPath, direntp->d_name);
            return check_for_user_dir(newPath, localRecursive, localIgnoreCapitalization, myLocalFileToFind);

        }

    }
    //wait for file close
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return 0;
}

std::string create_user_dir(std::string recipient, std::string path) {
    char final[124] = "";
     strcat(final, path.c_str());
     strcat(final, "/");
     strcat(final, recipient.c_str());

    if (mkdir(final, 0777) == -1) {
        std::cerr << "Error :  " << strerror(errno) << std::endl;
        fflush(stdout);
    }
    else {
        std::cout << "Directory created";
        fflush(stdout);
    }

}

void create_message_file(std::string sender, std::string recipient, std::string subject, std::string message) {

}

#endif //SERVER_HELPER_H
