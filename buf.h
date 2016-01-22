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

#define buf_get(buf,i) (&((buf).data)[(i)]

/*gbuf -- a generic buffer*/
typedef struct _gbuf {
  void* data;
  size_t sz;
} gbuf;

#define gbuf_get(type, buf, i) ((type*)((buf).data))[(i)]

#define buf_dec_type(type)                  \
  typedef struct _##type##_buf {			\
    type *data;                             \
    size_t sz;                              \
  }type##_buf;

#define buf_dec_proto_arraytype(name)                         \
  BUFPRE int name##_buf_mk(name##_buf*);                      \
  BUFPRE int name##_buf_mk_sz(name##_buf*,size_t);            \
  BUFPRE void name##_buf_free(name##_buf*);                   \
  BUFPRE int name##_buf_resize(name##_buf*,size_t);           \
  BUFPRE int name##_buf_memcpy(name##_buf*,                   \
                               const name *,                  \
                               size_t);                       \
  BUFPRE int name##_buf_cpy(name##_buf*, const name##_buf*);  \
  BUFPRE void name##_buf_set(name##_buf*, const name);

#define buf_dec_proto(name)                     \
  buf_dec_proto_arraytype(name);                \
  BUFPRE void name##_buf_setp(name##_buf*, const name*);




/*End buf_dec_proto*/

#define gbuf_dec_proto(name)                                        \
  BUFPRE int name##_gbuf_mk(gbuf*);                                 \
  BUFPRE int name##_gbuf_mk_sz(gbuf*,size_t);                       \
  BUFPRE void name##_gbuf_free(gbuf*);                              \
  BUFPRE int name##_gbuf_resize(gbuf*,size_t);                      \
  BUFPRE int name##_gbuf_memcpy(gbuf*, const name *, size_t);		\
  BUFPRE int name##_gbuf_cpy(gbuf*, const gbuf*);                   \
  BUFPRE void name##_gbuf_set(gbuf*, name);                         \
  BUFPRE void name##_gbuf_setp(gbuf*, const name*);
/*end gbuf_dec_proto*/




/*implementations*/
#define _buf_mk_sz(type, data, sz, insize) {            \
    size_t allocsz = _size(insize);                     \
    if(!( data = calloc(allocsz,sizeof(type)) ))		\
      return -1;                                        \
    sz = allocsz;                                       \
    return 0;                                           \
  }

#define _buf_free(type, data, sz)               \
  { free(data); data = (type*) (sz = 0 );}

#define _buf_resize(type, data, sz, insize) {           \
    size_t allocsz = _size(insize);                     \
    void * tmp;                                         \
    if(!( tmp = realloc(data,allocsz*sizeof(type)) ))	\
      return -1;                                        \
    data = tmp; sz = allocsz*sizeof(type);              \
    return 0;                                           \
  }                                                     

#define _buf_set(type, data, sz, c) {           \
    for(int i=0; i < sz; ++i)                   \
      memcpy(((type*)data)+i, c,                \
             sizeof(type));                     \
  }                                             \


#define buf_def_main(type)                          \
  BUFPRE int                                        \
  type##_buf_mk(type##_buf* b) {                    \
    return  type##_buf_mk_sz(b, BUFSIZE);			\
  }                                                 \
                                                    \
  BUFPRE int                                        \
  type##_buf_mk_sz(type##_buf* b,                   \
                   size_t insize)                   \
  { _buf_mk_sz(type,                                \
               b->data, b->sz,insize); }            \
                                                    \
  BUFPRE void                                       \
  type##_buf_free(type##_buf* b)                    \
  { _buf_free(type, b->data, b->sz); }              \
                                                    \
  BUFPRE int                                        \
  type##_buf_resize(type##_buf* b, size_t insize)	\
  { _buf_resize(type,b->data,b->sz,insize); }		\
                                                    \
  BUFPRE int                                        \
  type##_buf_grow(type##_buf* b) {                  \
    return type##_buf_resize(b,(b->sz)*2);			\
  }                                                 \
                                                    \
  BUFPRE int                                        \
  type##_buf_memcpy(type##_buf* b,                  \
                    const type * in,                \
                    size_t len)	{                   \
    if (len > b->sz	&&                              \
        !type##_buf_resize(b,len))                  \
      return -1;                                    \
    memcpy(b->data,in,len);                         \
    return 0;                                       \
  }                                                 \
                                                    \
  BUFPRE int                                        \
  type##_buf_cpy(type##_buf* b,                     \
                 const type##_buf* in) {            \
    return type##_buf_memcpy(b,                     \
                             in->data,in->sz);      \
  }

#define buf_def_assignable(type)                    \
  BUFPRE void                                       \
  type##_buf_setp(type##_buf* b,                    \
                  const type* c) {                  \
    _buf_set(type, b->data, b->sz, c);				\
  }                                                 \
                                                    \
  BUFPRE void                                       \
  type##_buf_set(type##_buf* b, const type c) {     \
    type##_buf_setp(b, &c);                         \
  }

#define buf_def_arrtype(type)                       \
  BUFPRE void                                       \
  type##_buf_set(type##_buf* b,                     \
                 const type c) {                    \
    _buf_set(type, b->data, b->sz, c);				\
  }                                                 \

/*end buf_def*/

#define gbuf_def(type)                             \
  BUFPRE int                                       \
  type##_gbuf_mk(gbuf* b) {                        \
    return  type##_gbuf_mk_sz(b, BUFSIZE);         \
  }                                                \
                                                   \
  BUFPRE int                                       \
  type##_gbuf_mk_sz(gbuf* b, size_t insize)        \
  { _buf_mk_sz(type, b->data, b->sz,insize); }     \
                                                   \
  BUFPRE void                                      \
  type##_gbuf_free(gbuf* b)                        \
  { _buf_free(type, b->data, b->sz); }             \
                                                   \
  BUFPRE int                                       \
  type##_gbuf_resize(gbuf* b, size_t insize)       \
  { _buf_resize(type,b->data,b->sz,insize); }      \
                                                   \
  BUFPRE int                                       \
  type##_gbuf_grow(gbuf* b) {                      \
    return type##_gbuf_resize(b,(b->sz)*2);        \
  }                                                \
                                                   \
  BUFPRE int                                       \
  type##_gbuf_memcpy(gbuf* b,                      \
                     const type * in,              \
                     size_t len) {                 \
    if (len > b->sz && !type##_gbuf_resize(b,len)) \
      return -1;                                   \
    memcpy(b->data,in,len);                        \
    return 0;                                      \
  }                                                \
                                                   \
  BUFPRE int                                       \
  type##_gbuf_cpy(gbuf* b, const gbuf* in) {       \
    return type##_gbuf_memcpy(b, in->data,in->sz); \
  }                                                \
                                                   \
  BUFPRE void                                      \
  type##_gbuf_setp(gbuf* b, const type* c) {       \
    _buf_set(type, b->data, b->sz, c);             \
  }                                                \
                                                   \
  BUFPRE void                                      \
  type##_gbuf_set(gbuf* in, type c) {              \
    type##_gbuf_setp(in, &c);                      \
  }
/*end gbuf_def*/

#define buf_def(type)       \
  buf_def_main(type);       \
  buf_def_assignable(type); 

#define buf_def_arraytype(type)       \
  buf_def_main(type);                 \
  buf_def_arrtype(type); 

                    

/*For inlining*/
#if defined(BUFINLINE) || defined(INLINETYPES)
#define buf_dec(type)				\
  buf_dec_type(type);				\
  buf_dec_proto(type);				\
  buf_def(type);

#define buf_dec_arraytype(type)     \
  buf_dec_type(type);				\
  buf_dec_proto_arraytype(type);    \
  buf_def_arraytype(type);                    

#define gbuf_dec(type)				\
  gbuf_dec_proto(type);				\
  gbuf_def(type);				
#else
#define buf_dec(type)				\
  buf_dec_type(type);				\
  buf_dec_proto(type);
#define buf_dec_arraytype(type)     \
  buf_dec_type(type);				\
  buf_dec_proto_arraytype(type);

#define gbuf_dec(type)				\
  gbuf_dec_proto(type);
#endif

#endif /*_BUF_H_*/
