#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define WINDOW_SIZE 31  // maximum number of elements that can be stored in the window
#define PAYLOAD_SIZE 512  // the size of the payload


//strucuture definition of a packet
typedef struct __attribute__((packed)) {

  uint8_t type : 3;
  uint8_t window : 5;
  uint8_t seq_num;
  uint16_t length;
  char payload[PAYLOAD_SIZE];
  uint32_t CRC;
} Packet;



//retourne la taille du packet
uint16_t get_length(Packet pack);

//permet de savoir si le packet est le dernier a etre envoye
int is_last(Packet pack);

//
Packet* data_packet(int seq, int length, char *payload);

int verify_packet( Packet p);

Packet* ack_packet(int seq_num);

