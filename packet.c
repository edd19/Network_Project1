#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

*packet data_packet(int seq_num, char buffer[], int length){
  Packet p = {1, 0, seq_num%max, length, buffer, ...}; 
  if (length >512) 
    {
      return NULL; 
    }
  else if ( length <512) 
    {//padding
    memset(&buffer[length-1], 0, max_length-length);

  }
      return p;
}

*packet ack_packet(int seq_num)
{
  Packet a = {2, window_size, seq_num, 0, null, ...};
  //CRC ? 
}

int verify_packet( Packet p) {
  if (p.length>MAX_LENGTH)
    {
      return -1;
    }
  if (p.CRC != CRC(packet))
    {
      return 0;
    }
  return 1; 
}

int isInSequence(Packet p, int lostack)
{
  if(lostack<MAX_SEQ_NUM - 31)
    {
      if (p.seq_unum - lostack>0)
	{
	  return 1;
	}
      else { return 0;}
    }
  else {
    if(lostack<p.seq_num<MAX_SEQ_NUM)
      {
	return 1;
      }
    else if(0<=p.seq_num<MAX_lostack)
      {
	return 1;
      }
    else {
      return 0; 
    }
  }
}



uint16_t get_length(Packet pack)
{
  return pack.length;
}

int is_last(Packet pack)
{
  if(pack.length < 4096)
    {
      return 1; //fin de la liste
    }
  return 0;
}



