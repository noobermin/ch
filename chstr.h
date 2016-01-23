#ifndef _CHSTR_H_
#define _CHSTR_H_

#include "buf.h"
buf_dec(char);

static inline char*
_chstr(char_buf in) { return in.data; }
/*undefine if you will*/
#ifndef _str
#define _str _chstr
#endif

#endif /*_CHSTR_H_*/
