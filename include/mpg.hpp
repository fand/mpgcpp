// mpg.h

#pragma once

#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <stdio.h>

#include "util.hpp"



#define BUFFER_SIZE (64 * 1024 * 1024)

typedef enum{ PCT_VIDEO, PCT_AUDIO, PCT_OTHER } PCT;

typedef struct{
  const BYTE *start;
  const BYTE *end;
  size_t size;
} pack;

typedef struct{
  const BYTE *start;
  const BYTE *end;
  size_t size;  
  PCT pct;
} packet;



class MPG
{
 private:
  BYTE *CODE_PACK;
  BYTE *CODE_PACKET;
  BYTE *MPG1_PACK_MASK;
  BYTE *MPG1_PACK_HEAD;
  BYTE *MPG2_PACK_MASK;
  BYTE *MPG2_PACK_HEAD;

  PCT PACKET_TYPE[8] = {PCT_OTHER, PCT_OTHER, PCT_OTHER, PCT_OTHER,
                        PCT_AUDIO, PCT_AUDIO, PCT_VIDEO, PCT_VIDEO};

  void initConst();
  void deleteConst();

  void pri(const char *s);
  
  FILE *f_in;
  
  BYTE *buf;
  BYTE *buf_end;
  BYTE *buf_v;
  BYTE *buf_a;
  long buf_v_size;
  long buf_a_size;  

  long buf_size;
  
  pack *packs;
  packet *packets;
  long packs_size;
  long packets_size;

  std::map<std::string, std::string> CODE;


 public:
  MPG(const char *src);
  ~MPG();
  long output(const char *dst);
  void searchPacks();
  void searchPackets();
  void getStream(BYTE *dst, PCT _pct);
  
};

