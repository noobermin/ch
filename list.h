#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

#define list_dec(name)					\
  typedef struct _##name##_list {			\
    name data;						\
    struct _##name##_list *next;			\
    struct _##name##_list *back;			\
  }name##_list;						\
  							\
  typedef int (*name##_test)(name*);					\
  typedef void (*name##_sub)(name*);					\
  int name##_list_insp(name##_list**,const name*);			\
  int name##_list_ins(name##_list**,name);				\
  int name##_list_ins_init(name##_list**,name##_test);			\
  int name##_list_pushp(name##_list**,const name*);				\
  int name##_list_push(name##_list**,name);				\
  void name##_list_free(name##_list*,name##_sub);			\

#define list_up(in) (in = in->next)
#define list_down(in) (in = in->back)
#define list_upp(in) (*in = &(*in)->next)
#define list_downp(in) (*in = &(*in)->back)
/*Use this carefully! Especially the trueexpr part.*/
#define list_get(list,trueexpr,out)					\
  {									\
    for(out = list ;out != NULL; out=out->next)				\
      if (trueexpr(&in->data))						\
	break;								\
  }									\


#define list_def(name)							\
									\
  int				 					\
  name##_list_insp(name##_list** in, const name* val)			\
  {									\
    name##_list *newel = (name##_list*)malloc(sizeof(name##_list));	\
    if (!newel)								\
      return -1;							\
    newel->data = *val;							\
    newel->back = *in;							\
    newel->next = *in ? (*in)->next : NULL;				\
    *in = newel;							\
    return 0;								\
  }									\
									\
  int				 					\
  name##_list_ins(name##_list** in, name val)				\
  {									\
    return name##_list_insp(in,&val);					\
  }									\
									\
  int				 					\
  name##_list_ins_init(name##_list** in, name##_test initer)		\
  {									\
    name##_list *newel = (name##_list*)malloc(sizeof(name##_list));	\
    if (!newel)								\
      return -1;							\
    if ( !initer && initer(&newel->data))				\
      return -1;							\
    newel->back = *in;							\
    *in = newel;							\
    return 0;								\
  }									\
									\
  int									\
  name##_list_pushp(name##_list** in, const name* val)			\
  {									\
    while(*in)								\
      in = &(*in)->next;						\
    return name##_list_insp(in,val);					\
  }									\
									\
  int									\
  name##_list_push(name##_list** in, name val)				\
  {									\
    return name##_list_pushp(in, &val);					\
  }									\
									\
  void									\
  name##_list_free(name##_list* in,name##_sub freer)			\
  {									\
    while(in->next != NULL)						\
      {									\
	if(freer) freer(&in->data);					\
	free( (in = in->next)->back );					\
      }									\
    if(freer) freer(&in->data);						\
    free(in);								\
  }									\
  									\
  void									\
  name##_rm(name##_list** in, name##_test rmif, name##_sub rmer)	\
  {									\
    while(*in != NULL)							\
      {									\
	if (rmif(&(*in)->data))						\
	  {								\
	    name##_list *rm = *in;					\
	    (*in)->next->back = (*in)->back; *in = (*in)->next;		\
	    if (rmer) rmer(&rm->data);					\
	    free(rm);							\
	  }								\
	in = &(*in)->next;						\
      }									\
  }									\
  
#endif
