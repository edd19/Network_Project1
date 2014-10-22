#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
    int sockfd = 0;
    struct sockaddr_in6 serv_addr; 

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 17)) < 0) //cree la socket
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    serv_addr.sin6_family = AF_INET6; //type d'adresse : IPv6 
    serv_addr.sin6_port = htons(5000); //transforme le port en equivalent binaire 

    if(inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr)<=0) //transforme l'adresse IPv6 en binaire
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    
    /*if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // initialise la connexion
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
    */
    
    char* msg = "hello"; 
    sendto(sockfd, (void*) &msg, sizeof(msg),0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); //envoie a l'adresse specifiee
    close(sockfd); //ferme la connexion 

    return 0;
}
