#ifndef _CHSTR_H_
#define _CHSTR_H_

#include "buf.h"
#include <string.h>
buf_dec(char);

static inline char*
_chstr(char_buf in) { return in.data; }
static inline char*
_chstrp(char_buf *in) { return in->data; }

/*undefine if you will*/
#ifndef _str
#define _str _chstr
#endif


static inline int
_strcat(char_buf *out, const char_buf* app) {
  size_t outlen = strlen(_chrstrp(app)) + strlen(_chrstrp(out))+1;
  if ( outlen > out->sz && char_buf_resize(out, outlen))
    return -1;
  strcat(_chstrp(out), _chrstrp(app));
  return 0;
}

static inline int
_str_precat(char_buf *out, const char_buf* pre) {
  size_t
    prelen  = strlen(_chrstp(pre)),
    outlen  = prelen +  strlen(_chrstp(out)) + 1;
  if ( outlen > out->sz && char_buf_resize(out, outlen))
    return -1;
  strcpy(_chstrp(out)+prelen,_chrstrp(out));
  memcpy(_chstrp(out), _chrstrp(pre), prelen);
  return 0;
}

#endif /*_CHSTR_H_*/
