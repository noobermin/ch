#ifndef _BUF_H_
#define _BUF_H_

#include <string.h>
#include <stdlib.h>
#include "misc.h"

#ifndef BUFSIZE
#define BUFSIZE 16
#endif

/*for inlining*/
#if defined(BUFINLINE) || defined(INLINETYPES)
#define BUFPRE static inline 
#else
#define BUFPRE 
#endif

#define _size(a) ( !(a)%BUFSIZE ? (a) : (a)/BUFSIZE+BUFSIZE )

#define buf_get(buf,i) (&(buf).data)[(i)]

#define buf_dec_type(name)				\
  typedef struct _##name##_buf {			\
    name *data;						\
    size_t sz;						\
  }name##_buf;

#define buf_dec_proto(name)						\
  BUFPRE int name##_buf_mk(name##_buf*);				\
  BUFPRE int name##_buf_mk_sz(name##_buf*,size_t);			\
  BUFPRE void name##_buf_free(name##_buf*);				\
  BUFPRE int name##_buf_resize(name##_buf*,size_t);			\
  BUFPRE int name##_buf_memcpy(name##_buf*, const name *, size_t);	\
  BUFPRE int name##_buf_cpy(name##_buf*, const name##_buf*);		\
  BUFPRE void name##_buf_set(name##_buf*, name);			\
  BUFPRE void name##_buf_setp(name##_buf*, const name*);
/*end buf_dec*/

#define buf_def(name)						\
								\
  BUFPRE int							\
  name##_buf_mk(name##_buf* b) {				\
    return  name##_buf_mk_sz(b, BUFSIZE);			\
  }								\
  								\
  BUFPRE int							\
  name##_buf_mk_sz(name##_buf* b, size_t insize) {		\
    size_t allocsz = _size(insize);				\
    b->data = calloc(allocsz,sizeof(name));			\
    if(!b->data) return -1;					\
    b->sz=allocsz;						\
    return 0;							\
  }								\
  								\
  BUFPRE void							\
  name##_buf_free(name##_buf* b) {				\
    free(b->data);						\
    b->data = (name*) (b->sz = 0);				\
  }								\
  								\
  BUFPRE int							\
  name##_buf_resize(name##_buf* b, size_t insize) {		\
    size_t allocsz = _size(insize);				\
    name* tmp;							\
    if (allocsz == b->sz)  return 0;				\
    tmp = (name*)calloc(allocsz,sizeof(name));			\
    if(!tmp) return -1;						\
    memcpy(tmp, b->data, min(b->sz,allocsz));			\
    free(b->data);						\
    b->data = tmp;						\
    b->sz = allocsz;						\
    return 0;							\
  }								\
  								\
  BUFPRE int							\
  name##_buf_grow(name##_buf* b) {				\
    return name##_buf_resize(b,(b->sz)*2);			\
  }								\
  								\
  BUFPRE int								\
  name##_buf_memcpy(name##_buf* b, const name * in, size_t len)	{	\
    if (len > b->sz							\
	&& !name##_buf_resize(b,len)) return -1;			\
    memcpy(b->data,in,len);						\
    return 0;								\
  }									\
									\
  BUFPRE int								\
  name##_buf_cpy(name##_buf* b, const name##_buf* in) {	  		\
    return name##_buf_memcpy(b, in->data,in->sz);			\
  }									\
									\
  BUFPRE void								\
  name##_buf_setp(name##_buf* in, const name* c) {			\
    for(int i=0;i<in->sz;++i)						\
      memcpy(in->data+i, c, sizeof(name));				\
  }									\
									\
  BUFPRE void								\
  name##_buf_set(name##_buf* in, name c) {				\
    name##_buf_setp(in, &c);						\
  }
/*end buf_def*/


/*for inlining*/
#if defined(BUFINLINE) || defined(INLINETYPES)
#define buf_dec(name)				\
  buf_dec_type(name);				\
  buf_dec_proto(name);				\
  buf_def(name);				

#else
#define buf_dec(name)				\
  buf_dec_type(name);				\
  buf_dec_proto(name);
#endif

#endif /*_BUF_H_*/
