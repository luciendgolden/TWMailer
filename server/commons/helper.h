//
// Created by Alexander Tampier on 30.10.18.
//

#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <string>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

bool check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind);

void create_user_dir(std::string basic_string);

void create_message_file(std::string basic_string, std::string basicString, std::string subject, std::string message);


/**
 * Search for a specific file in a given directory
 * @param searchPath
 * @param localRecursive
 * @param localIgnoreCapitalization
 * @param myLocalFileToFind
 * @return
 */
bool check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind) {
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

                //mutex lock for printf (outputstream)
                printf("++Process: %d -> Found File directory: %s/%s\n", getpid(), searchPath,
                       direntp->d_name);
                //flush to delete remaining buffer of outputstream
                fflush(stdout);
                //printf("%d:%s:%s%s\n\n",getpid(), direntp->d_name, myLocalSearchPath, direntp->d_name);
                //wait for file close
                while ((closedir(dirp) == -1) && (errno == EINTR));

                return 0;
            }

            //same without -i (not true)
        } else if (strcmp(myLocalFileToFind, direntp->d_name) == 0) {
            printf("++Process: %d -> Found File directory: %s/%s\n", getpid(), searchPath,
                   direntp->d_name);
            //flush to delete remaining buffer of outputstream
            fflush(stdout);
            //printf("%d:%s:%s%s\n\n",getpid(), direntp->d_name, myLocalSearchPath, direntp->d_name);
            //wait for file close
            while ((closedir(dirp) == -1) && (errno == EINTR));

            return 0;
        }

        if (direntp->d_type == DT_DIR && localRecursive && direntp->d_name[0] != '.' && direntp->d_name[1] != '.') {
            char newPath[1024];
            //strcpy(newPath, direntp->d_name);
            //printf("Hello World: %s\n", newPath);
            snprintf(newPath, sizeof(newPath), "%s/%s", searchPath, direntp->d_name);
            return check_for_user_dir(newPath, localRecursive, localIgnoreCapitalization, myLocalFileToFind);

        }

    }
    //wait for file close
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return 0;
}

void create_user_dir(std::string basic_string) {

}

void create_message_file(std::string basic_string, std::string basicString, std::string subject, std::string message) {

}

#endif //SERVER_HELPER_H
