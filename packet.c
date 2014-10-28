#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*Creates a data packet with sequence number "seq", payload "payload"
 * and with useful length of the payload equals to "length".
 * Returns the packet created.
 */
Packet* data_packet(int seq, int length, char *payload){
    Packet *pack = malloc(sizeof(Packet));
    pack->type = 1;
    pack->window = 0;
    pack->seq_num = seq;
    pack->length = length;
    strcpy(pack->payload, payload);
    pack->CRC = 0; //TO DO
    if(length < PAYLOAD_SIZE){
        bzero(&(pack->payload[length]), PAYLOAD_SIZE - length);
    }
    return pack;
}

/*Creates an acknowledgement packet with the next sequence of number 
 * expected equals to "seq", and window space left equals to "window".
 * Returns the packet created.
 */
Packet* ack_packet(int seq, int window)
{
  Packet *a = malloc(sizeof(Packet));
  a->type = 2;
  a->window = window;
  a->seq_num = seq;
  a->length = 0;
  bzero(a->payload, PAYLOAD_SIZE);
  a->CRC = 0;
}

/*Verify il the packet wasn't modified due to transmission errors or
 * has been correctly created.
 * Return 0 if the packet isn't good and 1 in the contrary.
 */
int verify_packet( Packet p) {
  if (p.length > PAYLOAD_SIZE)  //the payload length cannot be greater than "PAYLOAD_SIZE"
    {
      return 0;
    }
  //if (p.CRC != CRC(packet))
   // {
    //  return 0;
    //}
  return 1;
}


/*Returns 1 if the packet was the last to be sent. Returns 0 if not.
 */
int is_last(Packet pack)
{
  if(pack.length < 512)  //check if the useful payload is inferior at 512
    {
      return 1;
    }
  return 0;
}

/*Check if the packet can be send with the current splr.
 * Returns 1 if yes, 0 if no.
 */
int apply_splr(int splr){
    srand(time(NULL));
    const int MIN = 1;
    const int MAX = 100;
    int n = (rand() % (MAX - MIN +1)) + MIN; // produce a random number between 1 and 100
    if( n <= splr){ // if the random number is inferior to the splr value then it cannot be sent
     return 0;
    }
    
    return 1;
}

/*Apply the sber on the packet p.
 * Returns a modified copy of the packet.
 */
Packet * apply_sber(Packet *p, int sber){
    Packet *copy = malloc(sizeof(Packet));
    memcpy(copy, p, sizeof(Packet));
  
    srand(time(NULL));
    const int MIN = 0;
    const int MAX = sizeof(Packet)-1;
    
    int sber_adjust = (sber * sizeof(Packet)) / 1000; // adjust the splr to the size of the packet
    int i = 0;
    
    for(i = 0; i < sber_adjust; i++){
      int n = (rand() % (MAX - MIN +1)) + MIN; // produce a random number between 1 and size of the packet
      memset(copy+n, 0, 1);  // set value 0 to the nth byte
    }
    
    return p;
}

