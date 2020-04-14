//
// Created by victory on 2020/4/13.
//
/*This is minimal web server main program
 * */

#include <stdio.h>
#include "socklib.h"
#include <time.h>
#include <string.h>

/*do handle the request sending by client */
void process_request(int fd);

int main() {

    int webserver_socket, fd_communicate;
    webserver_socket = make_server_socket(8888);
    if(webserver_socket < 0)
        return -1;

    while(1)
    {
        fd_communicate = accept(webserver_socket, NULL, NULL);
        if(fd_communicate < 0)
            break;

        /*debug*/
        printf("accept successs: fd:%d\n", fd_communicate);
        process_request(fd_communicate);

        close(fd_communicate);
    }




    return 0;
}

void process_request(int fd)
{
    time_t now;
    char* tm_str = NULL;

    time(&now);
    tm_str = ctime(&now);
    write(fd, tm_str, strlen(tm_str));
}
