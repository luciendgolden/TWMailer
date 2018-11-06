//
// Created by Alexander Tampier on 30.10.18.
//

#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fstream>

int
check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind);

std::string create_user_dir(std::string recipient, std::string path);

bool create_message_file(std::string user_dir, std::string sender, std::string recipient, std::string subject,
                         std::stringstream &strm);

std::string get_msg_content(const char *searchPath, char *myLocalFileToFind, int msg_num);

std::string get_file_name_to_del(const char *searchPath, char *myLocalFileToFind, int msg_num);

std::string search_counter_file(const char *searchPath, char *myLocalFileToFind);


/**
 * Search for a specific file in a given directory
 * @param searchPath
 * @param localRecursive
 * @param localIgnoreCapitalization
 * @param myLocalFileToFind
 * @return
 */
int
check_for_user_dir(const char *searchPath, int localRecursive, int localIgnoreCapitalization, char *myLocalFileToFind) {
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


std::string search_counter_file(const char *searchPath, char *myLocalFileToFind) {
    //struct for directory operations
    struct dirent *direntp;
    int msg_num = 1; //counting the messages found for output 1:
    DIR *dirp;
    std::ifstream is_readfromfile;
    std::string result;
    std::string subject;
    std::string trash;
    std::string mystring;

    //errorhandling directory open
    if (!(dirp = opendir(searchPath))) {
        perror("Failed to open directory");
        return NULL;
    }

    //dir opened, while until EOF dir
    //readdir -> reads next/current file
    while ((direntp = readdir(dirp)) != NULL) {

        //comparing local file to find with the opened directories files
        // ignoe ".", ".." and "counter" files
        mystring = direntp->d_name;
        if (mystring.size() >= 16) {
            if (strcmp(mystring.substr(9, 7).c_str(), "message") == 0) {
                std::string myString = searchPath;
                myString.append("/");
                myString.append(direntp->d_name);

                //ignore trash from file because we only need the subject (line3)
                is_readfromfile.open(myString);
                for (int i = 0; i < 2; i++) {
                    is_readfromfile >> trash;
                }

                std::getline(is_readfromfile >> std::ws, subject);
                is_readfromfile.close();

                result.append(std::to_string(msg_num) + ": ");
                result.append(subject);
                result.append("\n");
                msg_num++;
            }
        }
    }
    //wait for file close
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return result;
}

std::string get_msg_content(const char *searchPath, char *myLocalFileToFind, int msg_num) {
    //struct for directory operations
    struct dirent *direntp;
    DIR *dirp;
    std::ifstream is_readfromfile;
    std::string result;
    std::string subject;
    std::string trash;
    bool check_counter = false;

    //errorhandling directory open
    if (!(dirp = opendir(searchPath))) {
        perror("Failed to open directory");
        return NULL;
    }

    //dir opened, while until EOF dir
    //readdir -> reads next/current file
    while ((direntp = readdir(dirp)) != NULL) {

        //comparing local file to find with the opened directories file
        // ignoe ".", ".." and "counter" files
        if (strcmp(direntp->d_name, myLocalFileToFind) == 0) {
            check_counter = true;
            continue;
        }

        if (check_counter) {
            if (msg_num == 1) {
                //do da magic
                //get file
                std::string line;
                std::string myString = searchPath;
                myString.append("/");
                myString.append(direntp->d_name);

                is_readfromfile.open(myString);

                while (!is_readfromfile.eof()) {
                    getline(is_readfromfile, line);
                    result.append(line + "\n");
                    line = "";
                }
                is_readfromfile.close();
                break;
            } else {
                msg_num--;
                continue;
            }
        }
    }
    //wait for file close
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return result;
}


std::string get_file_name_to_del(const char *searchPath, char *myLocalFileToFind, int msg_num) {
    //struct for directory operations
    struct dirent *direntp;
    DIR *dirp;
    std::ifstream is_readfromfile;
    std::string result;
    std::string subject;
    std::string trash;
    bool check_counter = false;

    //errorhandling directory open
    if (!(dirp = opendir(searchPath))) {
        perror("Failed to open directory");
        return NULL;
    }

    //dir opened, while until EOF dir
    //readdir -> reads next/current file
    while ((direntp = readdir(dirp)) != NULL) {

        //comparing local file to find with the opened directories file
        // ignoe ".", ".." and "counter" files
        if (strcmp(direntp->d_name, myLocalFileToFind) == 0) {
            check_counter = true;
            continue;
        }

        if (check_counter) {
            if (msg_num == 1) {
                //do da magic
                //get file
                std::string line;
                std::string myString = searchPath;
                myString.append("/");
                myString.append(direntp->d_name);

                return myString;
            } else {
                msg_num--;
                continue;
            }
        }
    }
    //wait for file close
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return NULL;
}

std::string create_user_dir(std::string recipient, std::string path) {
    char final[124] = "";
    std::string result;
    std::string id_file_name = "counter";
    strcat(final, path.c_str());
    strcat(final, "/");
    strcat(final, recipient.c_str());

    if (mkdir(final, 0777) == -1) {
        std::cerr << "Error :  " << strerror(errno) << std::endl;
        fflush(stdout);

        return NULL;
    } else {
        result = final;

        std::ofstream outfile(result + "/" + id_file_name);
        outfile << 0;
        outfile.flush();
        outfile.close();


        return result;
    }

}

bool create_message_file(std::string user_dir, std::string sender, std::string recipient, std::string subject,
                         std::stringstream &strm) {
    // TODO - Exception Handling?
    int count;
    std::string line;
    std::ifstream is_counter;
    std::ofstream out_counter;

    is_counter.open(user_dir + "/counter");
    is_counter >> count;
    is_counter.close();
    count++;
    out_counter.open(user_dir + "/counter");
    out_counter << count;
    out_counter.flush();
    out_counter.close();


    std::string name = recipient + "_message" + std::to_string(count);
    std::ofstream outfile(user_dir + "/" + name + ".txt");

    outfile << sender << "\n";
    outfile << recipient << "\n";
    outfile << subject << "\n";

    while (!strm.eof()) {
        getline(strm, line);
        outfile << line + "\n";
        line = "";
    }

    outfile.flush();
    outfile.close();

    return true;
}

#endif //SERVER_HELPER_H
