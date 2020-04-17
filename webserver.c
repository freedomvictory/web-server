//
// Created by victory on 2020/4/13.
//
/*This is minimal web server main program
 * */

#include <stdio.h>
#include "socklib.h"
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define PORTNUM 8888


/*do handle the request sending by client */
void process_request(int fd);
void child_waiter(int signum); /*receive SIGCHILD signal handler*/
int main() {

    int webserver_socket, fd_communicate;

    /*set what handler will call, when receive SIGCHILD signal*/
    signal(SIGCHLD, child_waiter);

    webserver_socket = make_server_socket(PORTNUM);
    if(webserver_socket < 0)
        return -1;

    while(1)
    {
        fd_communicate = accept(webserver_socket, NULL, NULL);
        if(fd_communicate < 0 )
        {
            if (errno == EINTR)         //accept interupted
                break;
            else
                continue;
        }

        /*debug*/
        printf("accept successs: fd:%d\n", fd_communicate);
        process_request(fd_communicate);

        close(fd_communicate);
    }




    return 0;
}

void process_request(int fd)
{
    pid_t pid;

    pid = fork();
    switch (pid)
    {
        case -1:
            return;
        case 0:
            dup2(fd, 1);
            close(fd);
            execl("/bin/date", "date", NULL);
            //oops("execlp");
        default:
            wait(NULL);
    }

}

void child_waiter(int signum)
{
    //wait all child process exit, then clear child process source
    while(waitpid(-1, NULL, WNOHANG) > 0);
}


