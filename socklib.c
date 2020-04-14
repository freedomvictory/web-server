//
// Created by victory on 2020/4/12.
//

#include "socklib.h"

/*socklib.c
*This file contain function used to lots when writing internet
*client/server programs. The two main  functions here are:
*
* int make_server_socket(portnum) return s server socket
*								  or -1 if error
* int make_server_socket(portnum,backlog)
*
* int connet_to_server(char *hostname, int portnum)
*						returns a connected socket
*						or -1 if error
*/



/*
 * return a server socket( socket() -> bind() -> listen())
 * if success, return a socket id, else return -1;
 * */
int make_server_socket_q(int portnum, int backlog)
{
    int sock_id;
    char hostname[HOSTLEN];
    struct hostent *hp;
    struct sockaddr_in saddr;

    /*create a socket : tcp protocol and transfer data type is stream */
    sock_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock_id == -1)
    {
        return -1;
    }

    bzero(&saddr, sizeof(saddr));                   //clear out struct
    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);                   //get info about host

    /*got local host ip address and package sockaddr_in struct*/
    bcopy((void *)hp->h_addr, (void *)&(saddr.sin_addr), hp->h_length);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(portnum);

    if(bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)        //bind local host ip
        return -1;
    if(listen(sock_id, BACKLOG) != 0)
        return -1;

    return sock_id;
}
int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum, BACKLOG);
}


/*create a client socket, and try connect to tcp server
 * if succeed , return client socket id
 * else, return -1
 * */
int connet_to_server(char* host, int portnum)
{
    int sock_id;
    char hostname[HOSTLEN];
    struct hostent *hp;
    struct sockaddr_in server_add;

    /*create a socket : tcp protocol and transfer data type is stream */
    sock_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock_id == -1)
    {
        return -1;
    }

    bzero(&server_add, sizeof(server_add));                   //clear out struct
    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);                             //get info about host
    if(hp == NULL)
        return -1;

    /*got local host ip address and package sockaddr_in struct*/
    bcopy((void *)hp->h_addr, (void *)&(server_add.sin_addr), hp->h_length);
    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(portnum);

    /*connect server */
    if(connect(sock_id, (struct sockaddr *)&server_add, sizeof(server_add)) != 0)
        return -1;
    return sock_id;
}
