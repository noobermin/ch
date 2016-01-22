typedef struct {
  int i;
  float f;
} foo;

typedef char decstr[11];
#define INLINETYPES
#include "../buf.h"
buf_dec(foo);
buf_dec(decstr);
#undef INLINETYPES

#include <stdio.h>
#include <stdlib.h>

void
fooprobe(size_t i, const foo_buf* buf) {
  if ( printf(" %2i : %f\n", i, buf->data[i].f) > 0)
    return;
  
  fprintf(stderr, "unable to printf...");
  exit(1);
}

void
decstrprobe(size_t i, const decstr_buf* buf) {
  if ( printf(" %2i : %s\n", i, buf->data[i]) > 0)
    return;
  
  fprintf(stderr, "unable to printf...");
  exit(1);
}

#define ch_call(c) { \
    if (c) goto err; \
}

main(){
  foo_buf fbuf;
  decstr_buf sbuf;
  ch_call(foo_buf_mk(&fbuf));
  ch_call(decstr_buf_mk(&sbuf));
  foo_buf_set(&fbuf, (foo){.i=1,.f=2.5});
  //                     123456789
  decstr_buf_set(&sbuf, "tester   ");
  for (size_t i=0; i != fbuf.sz; ++i)
    fooprobe( i, &fbuf);

  for (size_t i=0; i != sbuf.sz; ++i)
    decstrprobe( i, &sbuf);
  return 0;

  
 err:
  printf("Failed to make the foo_buf\n");
  exit(1);
}
