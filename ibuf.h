#ifndef _IBUF_H_
#define _IBUF_H_
#include <string.h>
#include <stdarg.h>

#define IBUF_MAGIC_MAX 32
#define IBUF_EMPTY 1

#define ibuf_geti(ib, i) (ib.buf.data[(i)])
#define ibuf_get(ib) (ib.buf.data[ib.pos])

#define ibuf_getip(ibp, i) (ibp->buf.data[(i)])
#define ibuf_getp(ibp) (ibp->buf.data[ibp->pos])

#define ibuf_empty(ib) (ib.pos == -1)

#define ibuf_dec(name)						\
  typedef struct _##name_ibuf {					\
    name##_buf buf;						\
    int pos;							\
    char magic;							\
  }name##_ibuf;							\
								\
  int name##_ibuf_mk(name##_ibuf*);				\
  int name##_ibuf_mk_sz(name##_ibuf*,size_t);			\
  void name##_ibuf_free(name##_ibuf*);				\
  int name##_ibuf_push(name##_ibuf*, name );			\
  int name##_ibuf_pushp(name##_ibuf*, const name* );		\
  int name##_ibuf_pop(name##_ibuf*, name*);			\
  name name##_ibuf_pop_unsafe(name##_ibuf*);			\
  int name##_ibuf_popv(name##_ibuf*, size_t,...);		\
/*end ibuf_dec*/

#define ibuf_def(name)						\
  int								\
  name##_ibuf_mk(name##_ibuf* in)				\
  {								\
    in->pos=-1; in->magic=0;					\
    return name##_buf_mk(&(in->buf));				\
  }								\
  								\
  int									\
  name##_ibuf_mk_sz(name##_ibuf* in, size_t sz)				\
  {									\
    in->pos=-1; in->magic=0;						\
    return name##_buf_mk_sz(&(in->buf),sz);				\
  }									\
  									\
  void									\
  name##_ibuf_free(name##_ibuf* in)				\
  {								\
    name##_buf_free(&(in->buf));				\
  }								\
  								\
									\
  int									\
  name##_ibuf_push(name##_ibuf* in, name val)				\
  {									\
    if (++(in->pos) > in->buf.sz && name##_buf_grow(&(in->buf)) )	\
      return -1;							\
    in->buf.data[in->pos] = val;					\
    return 0;								\
  }									\
									\
  int									\
  name##_ibuf_pushp(name##_ibuf* in, const name* val)			\
  {									\
    if (++(in->pos) > in->buf.sz && name##_buf_grow(&(in->buf)) )	\
      return -1;							\
    in->buf.data[in->pos] = *val;					\
    return 0;								\
  }									\
  									\
  int									\
  name##_ibuf_pop( name##_ibuf* in, name* out )				\
  {									\
    if (in->pos == -1) /*stack is empty*/				\
      return IBUF_EMPTY;						\
    name ret = in->buf.data[in->pos];					\
    if ( --(in->pos) <= in->buf.sz/2					\
	 && ++(in->magic) >IBUF_MAGIC_MAX				\
	 && name##_buf_resize(&(in->buf),in->buf.sz/2))			\
      /*if the resize fails, it's okay.*/				\
      in->magic = 0;/*we don't reset magic if it fails*/		\
    *out = ret;								\
    return 0;								\
  }									\
									\
  int									\
  name##_ibuf_popv(name##_ibuf* in, size_t n,...)			\
  {									\
    va_list vl;								\
    va_start(vl,n);							\
    while(n-->0)							\
      {									\
	if (name##_ibuf_pop(in,va_arg(vl,name*)))			\
	  return -1;							\
      }									\
    va_end(vl);								\
    return 0;								\
  }									\
									\
  name									\
  name##_ibuf_pop_unsafe(name##_ibuf* in)				\
  {									\
    return in->buf.data[in->pos--];					\
  }									

#endif /*_IBUF_H_*/
