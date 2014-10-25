#include "packet.h"

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



