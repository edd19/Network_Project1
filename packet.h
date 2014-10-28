#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define WINDOW_SIZE 31  // maximum number of elements that can be stored in the window
#define PAYLOAD_SIZE 512  // the size of the payload


//strucuture definition of a packet
typedef struct __attribute__((packed)) {

  uint8_t type : 3;  //type definition 1 for data packet, 2 for acknowledgement packet
  uint8_t window : 5; // size of the window of the receiver, this field is set at 0 for the sender
  uint8_t seq_num;  // sequence number of the data packet sent by the sender, sequence number of the packet expected for the receiver
  uint16_t length;  // useful length of the payload
  char payload[PAYLOAD_SIZE];  // data to be transmitted
  uint32_t CRC;
} Packet;


Packet* data_packet(int seq, int length, char *payload);
Packet* ack_packet(int seq, int window);
int verify_packet( Packet p);
int is_last(Packet pack);
int apply_splr(int splr);
Packet * apply_sber(Packet *p, int sber);



