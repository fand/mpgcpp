#include "util.hpp"


inline long min(long x, long y){
  return (x<y) ? x:y;
}

inline long max(long x, long y){
  return (x>y) ? x:y;
}


const BYTE *memsearch(const BYTE *s, size_t s_size, const BYTE *q, size_t q_size){

  if (s_size < q_size) {
    std::cout << "memsearch : size error!" << std::endl;
    return NULL;
  }
  
  const BYTE *ss = s;
  const BYTE *s_end = s + s_size - q_size;
  const BYTE *q_end = q + q_size - 1;
  
  register const BYTE *i, *j;
  
  do {
    i = ss;
    j = q;
    while (*i == *j && j != q_end) {
      ++i; ++j;
    }
    if (*i == *j && j == q_end) return ss;
    
  } while (ss++ != s_end);
  return NULL;
}


long bin2num(const BYTE *s, size_t size){
  long r = 0;
  int i = 0;
  while (i<size) {
    r = r << 8 + (long)(s+i);
  }
  return r;
}


void bitAnd(const BYTE *a, const BYTE *b, BYTE *dst, size_t size){
  for (int i=0; i<size; i++) {
    dst[i] = a[i] & b[i];
  }
}

void str2byte(const char *src, BYTE *dst, size_t size){
  for (int i=0; i<size; i++) {
    dst[i] = (BYTE)src[i];
  }
}

