//
// Created by Alexander Tampier on 30.10.18.
//

#ifndef SERVER_THREAD_ARGS_H
#define SERVER_THREAD_ARGS_H

struct thread_args {
    int *new_socket;
    const char *path;
};

#endif //SERVER_THREAD_ARGS_H
