#ifndef _HASH_H_
#define _HASH_H_

#define HASH_SIZE 4096

#ifndef HASH_RECURSIVE
static inline unsigned						
hash(const char *s)
{
  unsigned r=0;
  while(*s) r = r*31 + *s++;
  return r;
}
#else
static inline unsigned						
hash(const char *s)
{
  return hash_rsub(s,0);
}

static inline unsigned						
hash_rsub(const char *s,unsigned r)
{
  return !(*s) ? r :
    hash_rsub(s+1,r*31+*s);
}
#endif /*HASH_RECURSIVE*/

#define hash_init(in) memset(in,0,sizeof(in))			
  

#define hash_dec(name)						\
  typedef name##_list* name##_hash_table[HASH_SIZE];		\
  void name##_hash_free(name##_hash_table,name##_sub freer);
  

#define hash_def(name)						\
  void name##_hash_free(name##_hash_table table,		\
			name##_sub freer)			\
  {								\
    for(int i=0; i<HASH_SIZE;++i)				\
      if(table[i]) name##_list_free(table[i],freer);		\
  }								\
  

#endif /*_HASH_H_*/
