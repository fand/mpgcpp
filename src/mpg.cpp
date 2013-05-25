#include "mpg.hpp"

void MPG::pri(const char *s){
  std::cout << s << std::endl;
}

MPG::MPG(const char *src){
  
  initConst();
    
  // get file size
  struct stat sb;
  stat(src, &sb);
  buf_size = sb.st_size;

  // malloc buffers
  buf = new BYTE[buf_size];
  buf_v = new BYTE[buf_size];
  buf_a = new BYTE[buf_size];  
  buf_end = buf + (buf_size-1);
  buf_v_size = 0;
  buf_a_size = 0;
  
  // file open
  if ((f_in = fopen(src, "rb")) == NULL) {
    std::cout << "input file open error" << std::endl;
    std::exit(1);
  }

  // read src to buffer
  while (! feof(f_in)) {
    fread(buf, sizeof(BYTE), buf_size, f_in);
  }
  std::cout << "read" << std::endl;

  searchPacks();
  std::cout << "searcePacks ed" << std::endl;
  
  searchPackets();
  std::cout << "searcePackets ed" << std::endl;

  std::cout << "getting stream..." << std::endl;
  getStream(buf_v, PCT_VIDEO);
  getStream(buf_a, PCT_AUDIO);

  // search sequences

  
  // search frames
  
  
}


MPG::~MPG(){
  
  fclose(f_in);
  delete[] buf;
  delete[] buf_v;
  delete[] buf_a;

  free(packs);
  free(packets);

  deleteConst();
}


void MPG::searchPacks(){

  packs = (pack *)malloc(buf_size / 128);
  packs_size = 0;
  
  const BYTE *i;
  BYTE z1[10];
  BYTE z2[10];
  const BYTE *last = memsearch(buf, buf_size, CODE_PACK, 4);
  std::cout << "first pack : " << (last - buf) << std::endl;
  
  while (1) {
    i = memsearch(last+1, (buf_end-last) + 1, CODE_PACK, 4);
    if (i == NULL) break;
    
    // classify pack
    bitAnd(last+4, (const BYTE *)MPG1_PACK_MASK, z1, 10);
    bitAnd(last+4, (const BYTE *)MPG2_PACK_MASK, z2, 10);
    
    if(memcmp(z1, MPG1_PACK_HEAD, 10)==0){
      pack p = {last+12, i-1, i-(last+12)};
      packs[packs_size++] = p;
    }
    else if(memcmp(z2,MPG2_PACK_HEAD, 10)==0){
      pack p = {last+14, i-1, i-(last+14)};
      packs[packs_size++] = p;
    }
    last = i;
  }

  // for last pack (bigger than 30bytes)
  if (buf_end - last > 30) {  
    bitAnd(last+4, (const BYTE *)MPG1_PACK_MASK, z1, 10);
    bitAnd(last+4, (const BYTE *)MPG2_PACK_MASK, z2, 10);    
    if(memcmp(z1, MPG1_PACK_HEAD, 10) == 0){
      pack p = {last+12, buf_end, buf_end-(last+12) + 1};
      packs[packs_size++] = p;
    }
    else if(memcmp(z2, MPG2_PACK_HEAD, 10) == 0){
      pack p = {last+14, buf_end, buf_end-(last+14) + 1};
      packs[packs_size++] = p;
    }
  }
  std::cout << "packs num : " << packs_size << std::endl;
}


void MPG::searchPackets(){

  packets = (packet *)malloc(buf_size / 12);
  packets_size = 0;
  
  const BYTE *j;
  const BYTE *last_j;
  const BYTE *last_packet;
  pack p;
  long z;

  for (int i=0; i < packs_size; i++) {

    p = packs[i];

    last_j = memsearch(p.start, p.size, CODE_PACKET, 3);
    last_packet = last_j;
    
    while (true) {
      j = memsearch(last_j+1, (p.end - last_j),
                    CODE_PACKET, 3);
      if (j == NULL) break;
      if (j + 3 - p.end > 0) {
        break;
        last_j = j;
        continue;
      }

      z = (long)j[3];
      if (z < 0xBD || z > 0xFF) {
        last_j = j;
        continue;
      }

      packet pp = {last_packet + 4, j-1, j-(last_packet + 4),
                   PACKET_TYPE[(z & 0xF0) >> 4]};
      packets[packets_size++] = pp;
      last_packet = j;
      last_j = j;
    }

    if (p.end - last_j > 30) {  
      // for last packet
      z = (long)last_j[3];
      if (z < 0xBD || z > 0xFF) continue;
      packet pp = {last_packet + 4, p.end, p.end-(last_packet+4)+1,
                   PACKET_TYPE[(z & 0xF0) >> 4]};
      packets[packets_size++] = pp;
    }
  }

  std::cout << "packets num : " << packets_size << std::endl;
}


void MPG::getStream(BYTE *dst, PCT _pct){

  packet *p;
  long index = 0;
  const BYTE *left;
  
  for (int i=0; i < packets_size; i++) {
    p = &(packets[i]);
    if (p->pct != _pct) continue;
    for (int j=0; j < p->size; j++) {
      dst[index] = p->start[j];
      ++index;
    }
  }
}


long MPG::output(const char *dst){

  // file open
  FILE *f_out;
  if ((f_out = fopen(dst, "w+")) == NULL) {
    perror("output error\n");
    exit(1);
  }

  
  // modify buffer
  packet *pp;
  long i_v = 0;
  long i_a = 0;
  for (int i=0; i < packets_size; i++) {
    pp = &(packets[i]);

    if (pp->pct == PCT_VIDEO) {
      for (int j=0; j < pp->size; j++) {
        ((BYTE *)pp->start)[j] = buf_v[i_v++];
      }
    }
    else if (pp->pct == PCT_AUDIO) {
      for (int j=0; j < pp->size; j++) {      
        ((BYTE *)pp->start)[j] = buf_a[i_a++];
      }
    }
  }

  
  // write files
  pri("writing...");
  fwrite(buf, 1, buf_size, f_out);
  fclose(f_out);
  pri("done!");

  return 0;
}



// constants

void MPG::initConst(){

  CODE_PACK = new BYTE[4];
  CODE_PACKET = new BYTE[3];
  MPG1_PACK_MASK = new BYTE[10];
  MPG1_PACK_HEAD = new BYTE[10];
  MPG2_PACK_MASK = new BYTE[10];
  MPG2_PACK_HEAD = new BYTE[10];

  str2byte("\x00\x00\x01\xBA", CODE_PACK, 4);
  str2byte("\x00\x00\x01", CODE_PACKET, 3);
  str2byte("\xF1\x00\x01\x00\x01\x80\x00\x01\x00\x00", MPG1_PACK_MASK, 10);
  str2byte("\x21\x00\x01\x00\x01\x80\x00\x01\x00\x00", MPG1_PACK_HEAD, 10);
  str2byte("\xC4\x00\x04\x00\x04\x01\x00\x00\x03\x00", MPG2_PACK_MASK, 10);
  str2byte("\x44\x00\x04\x00\x04\x01\x00\x00\x03\x00", MPG2_PACK_HEAD, 10);
}

void MPG::deleteConst(){
  delete[] CODE_PACK;
  delete[] CODE_PACKET;
  delete[] MPG1_PACK_MASK;
  delete[] MPG1_PACK_HEAD;
  delete[] MPG2_PACK_MASK;
  delete[] MPG2_PACK_HEAD;
}
