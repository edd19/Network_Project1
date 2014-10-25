#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

typedef struct { 

  uint8_t type : 3;
  uint8_t window : 5;
  uint8_t seq_num; 
  uint16_t length;
  uint16_t payload[32];
  uint32_t CRC; 
} packet _attribute_((packed));


uint16_t get_length(packet pack)
{
  return pack.length;
}

int is_last(packet pack)
{
  if(pack.length < 4096)
    {
      return 1; //fin de la liste
    }
  return 0;
}



