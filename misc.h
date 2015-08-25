#ifndef _MISC_H_
#define _MISC_H_

#define min(a,b) ( (a) < (b) ? (a) : (b) )
#define max(a,b) ( (a) > (b) ? (a) : (b) )
#include <stdio.h>
#include <stdarg.h>

static inline
binplace(unsigned long i)
{
  switch(i) {
  case 0x0001: return  0;
  case 0x0002: return  1;
  case 0x0004: return  2;
  case 0x0008: return  3;
  case 0x0010: return  4;
  case 0x0020: return  5;
  case 0x0040: return  6;
  case 0x0080: return  7;
  case 0x0100: return  8;
  case 0x0200: return  9;
  case 0x0400: return 10;
  case 0x0800: return 11;
  case 0x1000: return 12;
  case 0x2000: return 13;
  case 0x4000: return 14;
  case 0x8000: return 15;
  default: return -1;
  }
}

static inline
_x_free(int n,...)
{
  va_list vl; va_start(vl, n);
  while(n-->0)
    free(va_arg(vl,void*));
  va_end(vl);
  return 0;
}

#define first_few(s1, s2) strncmp(s1,s2, min(strlen(s1),strlen(s2)))

/* Pls dun call me when str==NULL.
 * Also, yes, I'm needed like this.
 */
static inline
to_1st(char* str)
{
  int i=0;
  for(;str[i]==' '||str[i]=='\t';++i);
  return i;
}

static inline char*
getlinein(char *buf, size_t s, FILE* f)
{
  ( (buf=fgets(buf,s,f)) != NULL) && (buf[strlen(buf)-1]= '\0');
  return buf;
}

static inline char*
save_rest(char* dest, char* buf, int first,int sz)
{
  return strncpy(dest, buf+first+1+to_1st(buf+first+1), sz);
}



#endif /*_MISC_H_*/
