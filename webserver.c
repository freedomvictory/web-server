//
// Created by victory on 2020/4/13.
//
/*This is minimal web server main program
 * features:supports the GET command only
 *                      run in the current directory
 *                      forks a new child to handle each request
 *                      has MAJOR security holes, for demo purposes only
 *                      has many other weakness, but is a good start
 * */

#include <stdio.h>
#include "socklib.h"
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#define PORTNUM 8888

extern void logging(const char *fmt, ...);

/*do handle the request sending by client */
void process_request(char* rq, int fd);
void child_waiter(int signum);              /*receive SIGCHILD signal handler*/
void read_til_crnl(FILE *fp);
void cannot_do(int fd);
int not_exist(char* arg);                   /*judge Is the file exist in web server?*/
void do_404(char *item, int fd);             /*web server do 404 response to client*/
int is_dir(char *file);                      /*judge Is file a dir? */
void do_ls(char *dir, int fd);              /*do ls command*/
int ends_in_cgi(char *arg);
void do_exec(char *arg, int fd);            /*do exec command*/
void do_cat(char *file, int fd);             /*do cat command*/
void header(FILE *fp, char *content_type); /*the reply header*/
char *file_type(char *file);                /*got the file 'extension'*/
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
                continue;
            else
                break;
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

/*
 * handle the request from client
 * rq: request string
 * fd: communicate socket file descriptor
 * */
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
/*
 * singnal handler when recevie SIGCHILD signal
 * signum:
 * */
void child_waiter(int signum)
{
    //wait all child process exit, then clear child process memory source
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
/* ------------------------------------------------------ *
   read_til_crnl(FILE *)
   skip over all request info until a CRNL is seen
   ------------------------------------------------------ */
void read_til_crnl(FILE *fp)
{
    char buf[BUFSIZ];
    while(fgets(buf, BUFSIZ, fp) != NULL && strcmp(buf, "\r\n") != 0);
}
/* ------------------------------------------------------ *
   the reply header thing: all functions need one
   if content_type is NULL then don't send content type
   ------------------------------------------------------ */

void header( FILE *fp, char *content_type )
{
    fprintf(fp, "HTTP/1.0 200 OK\r\n");
    logging("response:%s",  "HTTP/1.0 200 OK\r\n");
    if ( content_type )
    fprintf(fp, "Content-type: %s\r\n", content_type );

}

/* what did server do when receive request which not supported
 * fd: communicate file descriptor
 * */

void cannot_do(int fd)
{
    FILE *fp;
    fp = fdopen(fd, "w");
    if(fp == NULL)
    {
        //error occur: can't open fd as file stream.
        return;
    }
    fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
    logging("response:%s",  "HTTP/1.0 501 Not Implemented");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "That command is not yet implemented\r\n");
    fclose(fp);

}

/******************************************
 *  judge whether the file exist?
 * @param arg : file name
 * @return 1: not exist ; 0: exist
 */

int not_exist(char* arg)
{
    struct stat info;
    return (stat(arg, &info) == -1);
}

/****************************************
 * web server do 404 response to client browser
 * @param item :file name
 * @param fd  :communicate socket file disc
 */
void do_404(char *item, int fd)
{
    FILE	*fp = fdopen(fd,"w");
    if(fp == NULL)
    {
        //error occurs: can't open file
        return;
    }

    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
    logging("response:%s",  "HTTP/1.0 404 Not Found\\r\\n\"");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "The item you requested: %s\r\nis not found\r\n",
            item);
    fclose(fp);

}
/*************************************
 * judge whether is it a directory ?
 * @param arg : dir path name
 * @return : 0 is dir, 1 is others
 */
int is_dir(char *file)
{
    struct stat info;
    return ( stat(file, &info) != -1 && S_ISDIR(info.st_mode) );
}
/**************************************
 * web server do ls command
 * @param arg :dir path name
 * @param fd  :communicate socket
 */

void do_ls(char *dir, int fd)
{
    FILE* fp;
    fp = fdopen(fd, "w");
    if(fp == NULL)
    {
        //error occurs
        exit(-1);
    }
    header(fp, "text/plain");
    fprintf(fp,"\r\n");
    fflush(fp);


    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("ls", "ls", "-l", dir, NULL);
    perror(dir);
    exit(1);
}
/********************************************
 * judge whether the file extension is  cgi
 * @param arg
 * @return 1 : yes or 0 : no
 */
int ends_in_cgi(char *arg)
{
    return (strcmp(file_type(arg), "cgi") == 0);
}
void do_exec(char *arg, int fd)
{
    FILE* fp;
    fp = fdopen(fd, "w");
    if(fp == NULL)
    {
        //error occurs
        exit(-1);
    }
    header(fp, NULL);
    fflush(fp);

    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execl(arg, arg, NULL);
    perror(arg);
    exit(1);

}
/****************************************
 * return the 'extension' of file
 * @param file :filename
 * @return : file extension
 */
char *file_type(char *file)
{
    char *cp;
    if((cp = strrchr(file, '.') )!= NULL)
    {
        return cp + 1;
    }
    return "";
}
/*****************************************
 * sends back file contents after a header
 * @param file:filename
 * @param fd:socket
 */
void do_cat(char *file, int fd)
{
    char *extension = file_type(file);
    char *content = "text/plain";
    FILE *fp_sock;
    FILE *fp_file;
    char c;

    if(!strcmp(extension, "html"))
    {
        content = "text/html";
    }
    else if(!strcmp(extension, "gif"))
    {
        content = "image/gif";
    }
    else if(!strcmp(extension, "jpg"))
    {
        content = "image/jpeg";
    }
    else if(!strcmp(extension, "jpeg"))
    {
        content = "image/jpeg";
    }

    fp_sock = fdopen(fd, "w");
    fp_file = fopen(file, "r");
    if(fp_sock != NULL & fp_file != NULL)
    {
        header(fp_sock, content);
        fprintf(fp_sock, "\r\n");
        while((c = getc(fp_file)) != EOF)
        {
            putc(c, fp_sock);
        }
        fclose(fp_sock);
        fclose(fp_file);
    }
    exit(0);
}
