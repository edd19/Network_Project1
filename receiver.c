#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "packet.h"
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <getopt.h>
#include <pthread.h>

void * call_recv_packet(void *arg);
void recv_packet(int sockfd, struct sockaddr *cli_addr);
void add_window(Packet p);
void process (Packet p);
void send_ack(Packet *a, int sockfd, struct sockaddr *addr);


typedef struct {
    int sockfd;
    struct sockaddr *addr;
} address_t;


sem_t empty;

char filename[NAME_MAX] = "/dev/stdout";
Packet window[WINDOW_SIZE];


int main(int argc, char**argv)
{
    char hostname[NAME_MAX];  // hostname of the receiver
    int port;  // the port used


    int option = 0;
    static struct option options[] = {
                                        {"file", required_argument, 0, 'f'}
                                     };

    while((option = getopt_long(argc, argv, "b:d:f:p:", options, NULL)) != -1){
        switch(option){
            case 'f':
                strcpy(filename ,optarg);
                break;

            case '?':

                break;
        }
    }

    strcpy(hostname, argv[optind]);
    port = atoi(argv[optind+1]);

    sem_init(&empty, 0, WINDOW_SIZE);

    int sockfd;
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    dest_addr.sin_port=htons(port);
    bind(sockfd,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

    address_t destination = {sockfd, (struct sockaddr *)&dest_addr};

    pthread_t thread_recv;

    pthread_create(&thread_recv, NULL, (void *)call_recv_packet, (void *)&destination);

    pthread_join(thread_recv, NULL);

   exit(0);
}


void * call_recv_packet(void *arg){
    address_t *a = (address_t *) arg;
    recv_packet(a->sockfd, a->addr);
}


void recv_packet(int sockfd, struct sockaddr *cli_addr){
	socklen_t addr_len;
	while(1) {
	sem_wait(&empty);
	recvfrom(sockfd, (void *)&window[0], sizeof(Packet), 0,cli_addr, &addr_len);
	}
}

void add_window(Packet p){
	verify_packet(p);
	//isInSequence();
	//if(direct process(pack); //send ack;
	//else if (window[i])
}

void send_ack(Packet *a, int sockfd, struct sockaddr *addr){
    printf("ack n: %d\n", a->seq_num);
    sendto(sockfd, (void *)a,sizeof(Packet),0,(struct sockaddr *)addr,sizeof(struct sockaddr));
}

void process (Packet p)
{
    int fd;
    fd = open("test.txt", O_APPEND&&O_CREAT, O_WRONLY);
	write(fd, p.payload, p.length);
	close(fd);
}
