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


sem_t empty; //empty is the number of space left in the window
int seq_expected = 0; // sequence number of the data packet expected by the receiver
int finish = 0;  // indicates if all the file has been received

char filename[NAME_MAX] = "/dev/stdout";  //name of the file were the data received has to be writed
Packet window[WINDOW_SIZE];  //the window of the receiver, were the packet out of order are stocked


int main(int argc, char**argv)
{
    window[0].seq_num = -1;
  
    char hostname[NAME_MAX];  // hostname of the sender
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
    struct sockaddr_in6 dest_addr, cli_addr;

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port=htons(port);
    inet_pton(hostname, &dest_addr.sin6_addr);
    bind(sockfd,(struct sockaddr *)&dest_addr,sizeof(dest_addr)); 

    address_t destination = {sockfd, (struct sockaddr *)&dest_addr};
    address_t source = {sockfd, (struct sockaddr *)&cli_addr};
  
    printf("****Waiting for connection****\n");
    
    pthread_t thread_recv, thread_process;
    
    pthread_create(&thread_recv, NULL, (void *)call_recv_packet, (void *)&destination);
    pthread_create(&thread_process, NULL, (void *)call_process, (void *)&destination);
    
    pthread_join(thread_process, NULL);
    pthread_cancel(thread_recv);
    
    
    printf("**** File received **** \n");
    
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
	while(!finish) {
	  sem_wait(&empty);
	  Packet p;
	  recvfrom(sockfd, (void *)&p, sizeof(Packet), 0,cli_addr, &addr_len);
	  
	  if(verify_packet(p) == 0 || isInWindow(window, p)){ //if the packet suffer transmission error or has been already received
	    int left;
	    sem_getvalue(&empty, &left);
	    send_ack(ack_packet(seq_expected, left), sockfd, cli_addr);  // we ignore it and send an ack_packet to of the packet expected
	  }
	  add_window(p);
	}
}

void add_window(Packet p){
  window[p.seq_num%WINDOW_SIZE] = p;
}


void send_ack(Packet *a, int sockfd, struct sockaddr *addr){
    sendto(sockfd, (void *)a,sizeof(Packet),0,(struct sockaddr *)addr,sizeof(struct sockaddr_in6));
}


void process(int sockfd, struct sockaddr *addr){
	while(!finish){
		if(window[seq_expected%WINDOW_SIZE].seq_num == seq_expected){
			write_payload(window[seq_expected%WINDOW_SIZE]);
			if(window[(seq_expected+1)%WINDOW_SIZE].seq_num != seq_expected+1){
			    int left;
			    sem_getvalue(&empty, &left);
			    Packet *ack = ack_packet(seq_expected+1, left);
			    send_ack(ack, sockfd, addr);
			}
			if(is_last(window[seq_expected%WINDOW_SIZE])){
			    finish =1;
			}
			sem_post(&empty);
			seq_expected++;	
			
		}
	}
}

void write_payload(Packet p)
{
    int fd;
    fd = open(filename, O_APPEND | O_CREAT | O_WRONLY, 0755);
    write(fd, p.payload, p.length);
    close(fd);
}

int isInWindow(Packet window[], Packet p)
{
	int n=0;
	while(n<WINDOW_SIZE)
	{
		if((window[n%WINDOW_SIZE]).seq_num==p.seq_num)
		{
			return 1;
		}
		n++;
	}
	return 0;
}
