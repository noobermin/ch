#ifndef _IBUF_H_
#define _IBUF_H_

#include <string.h>
#include <stdarg.h>


/*for inlining*/
#if defined(IBUFINLINE) || defined(INLINETYPES)
#define IBUFPRE static inline 
#else
#define IBUFPRE 
#endif

#define IBUF_MAGIC_MAX 32
#define IBUF_EMPTY 1

#define ibuf_dec_type(name)					\
  typedef struct _##name_ibuf {             \
    name##_buf buf;                         \
    int pos;                                \
    char magic;                             \
  }name##_ibuf;                             \

#define ibuf_dec_proto(name)                                    \
  IBUFPRE int name##_ibuf_mk(name##_ibuf*);                     \
  IBUFPRE int name##_ibuf_mk_sz(name##_ibuf*,size_t);			\
  IBUFPRE void name##_ibuf_free(name##_ibuf*);                  \
  IBUFPRE int name##_ibuf_push(name##_ibuf*, name );			\
  IBUFPRE int name##_ibuf_pushp(name##_ibuf*, const name* );    \
  IBUFPRE int name##_ibuf_pop(name##_ibuf*, name*);             \
  IBUFPRE int name##_ibuf_popv(name##_ibuf*, size_t,...);		\
  /*end ibuf_dec*/

/*usage macros*/
#define ibuf_geti(ib, i) ((ib).buf.data[(i)])
#define ibuf_get(ib) ((ib).buf.data[(ib).pos])

#define ibuf_empty(ib) ((ib).pos == -1)
/*use these only if you are sure you know what you're doing!*/
#define ibuf_pop(ib) ibuf_geti(ib,(ib).pos--)
#define ibuf_push(ib,val) (ibuf_geti(ib,++(ib).pos) = val)

#define ibuf_def(name)                                              \
  IBUFPRE int                                                       \
  name##_ibuf_mk(name##_ibuf* in) {                                 \
    in->pos=-1; in->magic=0;                                        \
    return name##_buf_mk(&(in->buf));                               \
  }                                                                 \
                                                                    \
  IBUFPRE int                                                       \
  name##_ibuf_mk_sz(name##_ibuf* in, size_t sz)	{                   \
    in->pos=-1; in->magic=0;                                        \
    return name##_buf_mk_sz(&(in->buf),sz);                         \
  }                                                                 \
                                                                    \
  IBUFPRE void                                                      \
  name##_ibuf_free(name##_ibuf* in) {                               \
    name##_buf_free(&(in->buf));                                    \
  }                                                                 \
                                                                    \
  IBUFPRE int                                                       \
  name##_ibuf_pushp(name##_ibuf* in, const name* val) {             \
    if (++(in->pos) > in->buf.sz && name##_buf_grow(&(in->buf)) )	\
      return -1;                                                    \
    ibuf_get(*in) = *val;                                           \
    return 0;                                                       \
  }                                                                 \
                                                                    \
  IBUFPRE int                                                       \
  name##_ibuf_push(name##_ibuf* in, name val) {                     \
    if (++(in->pos) > in->buf.sz                                    \
        && name##_buf_grow(&(in->buf)) )                            \
      return -1;                                                    \
    ibuf_get(*in)=val;                                              \
    return 0;                                                       \
  }                                                                 \
                                                                    \
  IBUFPRE int                                                       \
  name##_ibuf_pop(name##_ibuf* in, name* out ) {                    \
    if (in->pos == -1) /*stack is empty*/                           \
      return IBUF_EMPTY;                                            \
    name ret = in->buf.data[in->pos];                               \
    if ( --(in->pos) <= in->buf.sz/2                                \
         && ++(in->magic) >IBUF_MAGIC_MAX                           \
         && name##_buf_resize(&(in->buf),in->buf.sz/2))             \
      /*if the resize fails, it's okay.*/                           \
      in->magic = 0;/*we don't reset magic if it fails*/            \
    *out = ret;                                                     \
    return 0;                                                       \
  }                                                                 \
                                                                    \
  IBUFPRE int                                                       \
  name##_ibuf_popv(name##_ibuf* in, size_t n,...) {                 \
    va_list vl;                                                     \
    va_start(vl,n);                                                 \
    while(n-->0) {                                                  \
      if (name##_ibuf_pop(in,va_arg(vl,name*)))                     \
        return -1;                                                  \
    }                                                               \
    va_end(vl);                                                     \
    return 0;                                                       \
  }

/*for inlining*/
#if defined(IBUFINLINE) || defined(INLINETYPES)
#define ibuf_dec(name)				\
  ibuf_dec_type(name);				\
  ibuf_dec_proto(name);				\
  ibuf_def(name);				
#else
#define ibuf_dec(name)				\
  ibuf_dec_type(name);				\
  ibuf_dec_proto(name);
#endif


#endif /*_IBUF_H_*/
