#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "packet.h"
//Modifier les includes



void send_packet(int sockfd, struct sockaddr *dest_addr);
void recv_ack(int sockfd, struct sockaddr *src_addr);
void create_packet(char *filename);
void * call_create_packet(void *arg);
void * call_send_packet(void *arg);
void * call_recv_ack(void *arg);


// dans packet.h peut etre
typedef struct {
    int sockfd;
    struct sockaddr *addr;
} address_t;


sem_t empty, full;  //
Packet packets_to_send[WINDOW_SIZE]; // array of packets to be send
int number_pack = 0;  // number of packets created
int finish = 0;  // indicates if the program has finished creating the packets
int last_ack = -1;  //last aknowledgement received


int main(int argc, char**argv)
{
    char filename[NAME_MAX] = "/dev/stdin"; //file to send, by default the standard input
    int sber = 0;  //byte transmission error ration in per-thousand
    int splr = 0;  // packet loss ratio in percentage
    int delay = 0;  // delay before transmitting each packet in milliseconds
    char hostname[NAME_MAX];  // hostname of the receiver
    int port;  // the port used


    int option = 0;
    static struct option options[] = {
                                        {"file", required_argument, 0, 'f'},
                                        {"sber", required_argument, 0, 'b'},
                                        {"splr", required_argument, 0, 'p'},
                                        {"delay", required_argument, 0, 'd'}
                                     };

    while((option = getopt_long(argc, argv, "b:d:f:p:", options, NULL)) != -1){
        switch(option){
            case 'b':
                sber = atoi(optarg);
                break;

            case 'd':
                delay = atoi(optarg);
                break;

            case 'f':
                strcpy(filename ,optarg);
                break;

            case 'p':
                splr = atoi(optarg);
                break;

            case '?':

                break;
        }
    }

    strcpy(hostname, argv[optind]);
    port = atoi(argv[optind+1]);

    sem_init(&empty, 0, WINDOW_SIZE);
    sem_init(&full, 0, 0);

    int sockfd;
    struct sockaddr_in dest_addr, src_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr=inet_addr(hostname);
    dest_addr.sin_port=htons(3200);

    address_t destination = {sockfd, (struct sockaddr *)&dest_addr};
    address_t source = {sockfd, (struct sockaddr *)&src_addr};

    
    printf("*** Envoi de fichier ***");
    pthread_t thread_send, thread_recv, thread_timer, thread_create_packet;
    
    pthread_create(&thread_create_packet, NULL, (void *)call_create_packet, (void *)&filename);
    pthread_create(&thread_send, NULL, (void *)call_send_packet, (void *) &destination);
    pthread_create(&thread_recv, NULL, (void *)call_recv_ack, (void *) &source);

    pthread_join(thread_create_packet, NULL);
    pthread_join(thread_send, NULL);
    pthread_join(thread_recv, NULL);
    
    printf("*** Fichier envoye ***");

   exit(0);
}


/* Send packets
    TO DO
*/
void send_packet(int sockfd, struct sockaddr *dest_addr){
    int n = 0;
    while(!finish || n < number_pack){
        sem_wait(&full);
        sendto(sockfd, (const void *)&packets_to_send[n%WINDOW_SIZE], sizeof(Packet), 0, dest_addr, sizeof(struct sockaddr));
        n++;
    }
   
}

/* Receive acknowledgments packets
    TO DO
*/
void recv_ack(int sockfd, struct sockaddr *src_addr){
    int n = 0;
    while(!finish || n < number_pack){
        Packet ack;
        socklen_t addrlen;
        recvfrom(sockfd, &ack, sizeof(Packet), 0, src_addr, &addrlen);
	if (verify_packet(ack) == 0){ // if packet is not good
            // TO DO if packet out of sequence
        }
        int i = n;
        n = ack.seq_num;
	
        for(i; i < n; i++){
	    sem_post(&empty);
	}  
	printf("gogoel\n");
    }
    
}

/* Create packets to be send.
    TO DO
*/
void create_packet(char *filename){
    int i = 0;
    int length = 0;
    char payload[PAYLOAD_SIZE];
    int fd = open(filename, O_RDONLY);
    if(fd == -1){
        // Message d'erreur
    }

    while((length = read(fd, payload, sizeof(payload))) > 0){
        sem_wait(&empty);
        Packet p = *data_packet(i, length, payload);
        packets_to_send[i%WINDOW_SIZE] = p;
        number_pack++;
        i++;

        sem_post(&full);
    }
    finish = 1;
    close(fd);
}



/* Call the method create_packet
*/
void * call_create_packet(void *arg){
    //name *n = (name *) arg;
    //create_packet(n->name);
    char * name = (char *) arg;
    create_packet(name);
}

/* Call the method send_packet
*/
void * call_send_packet(void *arg){
    address_t *a = (address_t *) arg;
    send_packet(a->sockfd, a->addr);
}

/* Call the method recv_ack
*/
void * call_recv_ack(void *arg){
    address_t *a = (address_t *) arg;
    recv_ack(a->sockfd, a->addr);
}

/* Call the method timer
*/
void * call_timer(void *arg){
    // TO DO
}
