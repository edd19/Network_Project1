
/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "packet.h"

int main(int argc, char**argv)
{
   int sockfd,n;
   int m = 0;
   struct sockaddr_in servaddr,cliaddr;
   char recvline[512];
   char buffer[512] = "gfsfsd";
   Packet p = {1, 0, 39, 512, 0, 21};
   strcpy(p.payload, buffer);

   if (argc != 2)
   {
      printf("usage:  udpcli <IP address>\n");
      exit(1);
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   servaddr.sin_port=htons(32000);

   while (m < 10)
   {
      printf("m = %d\n", m);
      sendto(sockfd,(const void *)&p,sizeof(Packet),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,512,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);
      m++;
   }

   //free(p);
}
