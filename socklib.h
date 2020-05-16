//
// Created by Administrator on 2020/4/12.
//


#ifndef PROJECT_WEB_SERVER_SOCKLIB_H
#define PROJECT_WEB_SERVER_SOCKLIB_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <arpa/inet.h>

#define HOSTLEN 256
#define BACKLOG 1

int make_server_socket(int portnum);
int connect_to_server(char* host, int portnum);





#endif //PROJECT_WEB_SERVER_SOCKLIB_H
