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
void process_request(char* rq, int fd);
void child_waiter(int signum); /*receive SIGCHILD signal handler*/
void read_til_crnl(FILE *fp);
void cannot_do(int fd);
int not_exist(char* arg);
void do_404(char *arg, int fd);
int is_dir(char *arg);
void do_ls(char *arg, int fd);
int ends_in_cgi(cha *arg);
void do_exec(char *arg, int fd);
void do_cat(char *arg, int fd);

int main() {

    int webserver_socket, fd_communicate;
    FILE* fpin;

    char request[BUFSIZ];

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

        /*get request string from socket*/
        fpin = fdopen(fd_communicate, "r");
        fgets(request, BUFSIZ, fpin);
        printf("got a call: requst = %s\n", request);
        read_til_crnl(fpin);
        /*server process request and return response*/
        process_request(request, fd_communicate);

        fclose(fpin);
    }

    return 0;
}

void process_request(char* rq, int fd)
{
    char	cmd[BUFSIZ], arg[BUFSIZ];

    /* create a new process and return if not the child */
    if ( fork() != 0 )
        return;

    strcpy(arg, "./");		/* precede args with ./ */
    if ( sscanf(rq, "%s%s", cmd, arg+2) != 2 )
        return;

    if ( strcmp(cmd,"GET") != 0 )
        cannot_do(fd);
    else if ( not_exist( arg ) )
        do_404(arg, fd );
    else if (is_dir(arg) )
        do_ls( arg, fd );
    else if ( ends_in_cgi( arg ) )
        do_exec( arg, fd );
    else
        do_cat( arg, fd );

}

void child_waiter(int signum)
{
    //wait all child process exit, then clear child process source
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void read_til_crnl(FILE *fp)
{

}
void cannot_do(int fd)
{

}
int not_exist(char* arg)
{
    return 0;
}
void do_404(char *arg, int fd)
{

}
int is_dir(char *arg)
{
    return 0;
}
void do_ls(char *arg, int fd)
{

}
int ends_in_cgi(cha *arg)
{

    return 0;
}
void do_exec(char *arg, int fd)
{

}
void do_cat(char *arg, int fd)
{

}
