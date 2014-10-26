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

#include "packet.h"
#define VALUE 31

void send_packet(int sockfd, struct sockaddr *dest_addr);
void recv_ack(int sockfd, struct sockaddr *src_addr);
void create_packet(char *filename);
Packet* data_packet(int seq, int length, char *payload);
void * call_create_packet(void *arg);
void * call_send_packet(void *arg);
void * call_recv_ack(void *arg);

int number_pack = 0;
int finish = 0;

typedef struct {
    int sockfd;
    struct sockaddr *addr;
} address_t;


sem_t empty, full;
Packet packets_to_send[VALUE]; // array of packets to be send

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

    sem_init(&empty, 0, VALUE);
    sem_init(&full, 0, 0);

    int sockfd;
    struct sockaddr_in dest_addr, src_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr=inet_addr(hostname);
    dest_addr.sin_port=htons(32000);

    address_t destination = {sockfd, (struct sockaddr *)&dest_addr};
    address_t source = {sockfd, (struct sockaddr *)&src_addr};

    pthread_t thread_send, thread_recv, thread_timer, thread_create_packet;

    pthread_create(&thread_create_packet, NULL, (void *)call_create_packet, (void *)&filename);
    pthread_create(&thread_send, 0, (void *)call_send_packet, (void *) &destination);
    pthread_create(&thread_recv, 0, (void *)call_recv_ack, (void *) &source);

    pthread_join(thread_create_packet, NULL);
    pthread_join(thread_send, NULL);
    pthread_join(thread_recv, NULL);

   exit(0);
}


/* Send packets
    TO DO
*/
void send_packet(int sockfd, struct sockaddr *dest_addr){
    int n = 0;
    while(!finish || n < number_pack){
        sem_wait(&full);
        sendto(sockfd, (const void *)&packets_to_send[n%VALUE], sizeof(Packet), 0, dest_addr, sizeof(struct sockaddr));
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
        printf("Packet recu : %s \n", ack.payload);
        n++;
        sem_post(&empty);
        /*if (verify_packet(ack) == 1){
            TO DO
          }
          else{} // do nothing
        */
    }
}

/* Create packets to be send.
    TO DO
*/
void create_packet(char *filename){
    int i = 0;
    int length = 0;
    char payload[512];
    int fd = open(filename, O_RDONLY);
    if(fd == -1){
        // Message d'erreur
    }

    while((length = read(fd, payload, sizeof(payload))) > 0){
        sem_wait(&empty);
        Packet p = *data_packet(i, length, payload);
        packets_to_send[i%VALUE] = p;
        number_pack++;
        i++;

        sem_post(&full);
    }
    finish = 1;
    close(fd);
}

Packet* data_packet(int seq, int length, char *payload){
    Packet *pack = malloc(sizeof(Packet));
    pack->type = 1;
    pack->window = 0;
    pack->seq_num = seq;
    pack->length = length;
    strcpy(pack->payload, payload);
    pack->CRC = 0; //TO DO

    return pack;
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
