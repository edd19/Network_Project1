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
void process ();
void write_payload(Packet p);
void send_ack(Packet *a, int sockfd, struct sockaddr *addr);
int isInWindow(Packet window[], Packet p);
void *call_process(void *arg);


typedef struct {
    int sockfd;
    struct sockaddr *addr;
} address_t;


sem_t empty;
sem_t full;
int expected = 0;

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
    sem_init(&full, 0, 0);


    int sockfd;
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    dest_addr.sin_port=htons(port);
    bind(sockfd,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

    address_t destination = {sockfd, (struct sockaddr *)&dest_addr};

    pthread_t thread_recv, thread_process;

    pthread_create(&thread_recv, NULL, (void *)call_recv_packet, (void *)&destination);
    pthread_create(&thread_process, NULL, (void *)call_process, (void *)&destination);

    pthread_join(thread_recv, NULL);
    pthread_join(thread_process, NULL);

   exit(0);
}


void * call_recv_packet(void *arg){
    address_t *a = (address_t *) arg;
    recv_packet(a->sockfd, a->addr);
}

void *call_process(void *arg){
	address_t *a = (address_t *) arg;
	process(a->sockfd, a->addr);
}

void recv_packet(int sockfd, struct sockaddr *cli_addr){
	socklen_t addr_len;
	while(1) {
	sem_wait(&empty);
	Packet p;
	recvfrom(sockfd, (void *)&p, sizeof(Packet), 0,cli_addr, &addr_len);
	add_window(p);
	}
}

void add_window(Packet p){
	//verify_packet(p);
	if(isInWindow(window, p)){
		//send ack with seq_num of last_ack
	}
	else{
		window[p.seq_num%WINDOW_SIZE] = p;
		sem_post(&full);
	}
}


void send_ack(Packet *a, int sockfd, struct sockaddr *addr){
    printf("ack n: %d\n", a->seq_num);
    sendto(sockfd, (void *)a,sizeof(Packet),0,(struct sockaddr *)addr,sizeof(struct sockaddr));
}



void process(int sockfd, struct sockaddr *addr){
	while(1){
		sem_wait(&full);
		if(&window[expected] != NULL){
			write_payload(window[expected]);
			send_ack(&window[expected], sockfd, addr);
			//&window[expected] = NULL;
			expected++;
			
		}
	}
}

void write_payload(Packet p)
{
    int fd;
    fd = open("test.txt", O_APPEND&&O_CREAT, O_WRONLY);
	write(fd, p.payload, p.length);
	close(fd);
}

int isInWindow(Packet window[], Packet p)
{
	int n=0;
	while(n<WINDOW_SIZE)
	{
		if((window[n]).seq_num==p.seq_num)
		{
			return 1;
		}
		n++;
	}
	return 0;
}
