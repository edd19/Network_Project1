#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char *argv[])
{
  int listenfd = 0;
    struct sockaddr_in6 serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(5000); 

    if(inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr)<=0) //transforme l'adresse IPv6 en binaire
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    while(1) {
    recvfrom(listenfd, sendBuff, sizeof(sendBuff), 0, (struct sockaddr *) &serv_addr, (socklen_t *)sizeof(serv_addr));
    }

    printf("%s", sendBuff);
}
