#pragma once

#include <stdlib.h>
#include <iostream>

typedef unsigned char BYTE;

inline long min(long x, long y);
inline long max(long x, long y);
const BYTE * memsearch(const BYTE *s, size_t s_size, const BYTE *q, size_t q_size);
long bin2num(const BYTE *s, size_t size);
void bitAnd(const BYTE *a, const BYTE *b, BYTE *dst, size_t size);
void str2byte(const char *src, BYTE *dst, size_t size);

