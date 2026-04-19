#include"defs.h"

void*
memset(const void* s, int c, size_t n)
{
  u8* cur = (u8*)s;

  while(n--)
  {
    *cur++ = (u8)c;
  }
  
  return s;
}

void*
memcpy(void* d, const void* s, size_t n)
{
  u8* curs = (u8*)s;
  u8* curd = (u8*)d;

  while(n--)
  {
    *curd++ = *curs++;
  }

  return d;
}
