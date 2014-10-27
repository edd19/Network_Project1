#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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

Packet* ack_packet(int seq_num)
{
  Packet *a = malloc(sizeof(Packet));
  a->type = 2;
  a->window = 31;
  a->seq_num = seq_num;
  a->length = 0;
  bzero(a->payload, PAYLOAD_SIZE);
  a->CRC = 0;
}

int verify_packet( Packet p) {
  if (p.length > PAYLOAD_SIZE)
    {
      return 0;
    }
  //if (p.CRC != CRC(packet))
   // {
    //  return 0;
    //}
  return 1;
}

//int isInSequence(Packet p, int lastack)
//{
//  if(lastack<MAX_SEQ_NUM - 31)
//    {
//      if (p.seq_unum - lostack>0)
//	{
//	  return 1;
//	}
//      else { return 0;}
//    }
//  else {
//    if(lastack<p.seq_num<MAX_SEQ_NUM)
//      {
//	return 1;
//      }
//    else if(0<=p.seq_num<MAX_lostack)
//      {
//	return 1;
//      }
//    else {
//      return 0;
//    }
//  }
//}



uint16_t get_length(Packet pack)
{
  return pack.length;
}

int is_last(Packet pack)
{
  if(pack.length < 512)
    {
      return 1; //fin de la liste
    }
  return 0;
}



