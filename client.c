//
// Created by victory on 2020/4/13.
//
/*This is a web client main program
 * */


#include <stdio.h>
#include "socklib.h"


#define LEN 256

/*communicate with web server, send request and receive response*/
void talk_with_server(int fd);


int main(void)
{
    int fd_communicate;
    fd_communicate = connect_to_server("192.168.43.64", 8888);
    if(fd_communicate < 0)
        return -1;

    talk_with_server(fd_communicate);
    close(fd_communicate);

    return 0;
}

void talk_with_server(int fd)
{
    char buf[LEN];
    int n;

    n = read(fd, buf, LEN);
  // printf("buf:%s\n", buf);
    write(1, buf, n);


}