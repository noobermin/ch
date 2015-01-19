#ifndef _AIFALEENE_H_
#define _AIFALEENE_H_

typedef unsigned long large_mask;

/*A myraid of types*/
typedef enum _token_type{
  VALUE_TOKEN        = 0x001,
  ID_TOKEN           = 0x002,
  OP_TOKEN           = 0x004,
  PREFIX_OP_TOKEN    = 0x008,
  OPEN_DELIM_TOKEN   = 0x010,
  CLOSE_DELIM_TOKEN  = 0x020,
  ENDL_TOKEN         = 0x040,
  INVALID_TOKEN      = 0x080
}token_type;

typedef enum _value_type{
  UINTEGER_VALUE          = 1,
  INTEGER_VALUE           = 2,
  FLOAT_VALUE             = 3,
  BOOL_VALUE              = 4,
}value_type;
    
typedef enum _op_type{
  ADD_OP=0,
  SUB_OP,
  MUL_OP,
  IDIV_OP,
  DIV_OP,
  MOD_OP,
  POW_OP,
  EQ_OP,
  NOT_EQ_OP,
  GREAT_OP,
  LESS_OP,
  GR_EQ_OP,
  LS_EQ_OP,
  LAND_OP,
  LOR_OP,
  ASSIGN_OP,
  INVALID_OP
}op_type;

typedef enum _prefix_op_type{
  NEG_PREF=0,
  INVALID_PREF
}prefix_op_type;

typedef enum _open_delim_type{
  PARA_OPEN,
  SQ_OPEN,
  CURL_OPEN,
  INVALID_OPEN
}open_delim_type;

typedef enum _close_delim_type{
  PARA_CLOSE,
  SQ_CLOSE,
  CURL_CLOSE,
  INVALID_CLOSE
}close_delim_type;

typedef unsigned long uint_t;
typedef long int_t;
typedef double flt_t;

typedef struct _value{
   value_type type;
   union
   {
     uint_t uinteger;
     int_t  integer;
     flt_t  floating;
   };
}value;

#define get_value(val)					\
  (val.type == UINTEGER_VALUE ? val.uinteger		\
   : val.type == BOOL_VALUE ? val.uinteger		\
   : val.type == INTEGER_VALUE ? val.integer		\
   : val.floating)

#define get_valuep(val)					\
  (val->type == UINTEGER_VALUE ? val->uinteger		\
   : val->type == BOOL_VALUE ? val->uinteger		\
   : val->type == INTEGER_VALUE ? val->integer		\
   : val->floating)


#define set_value(val, r)					\
  (val.type == UINTEGER_VALUE ? (val.uinteger=(uint_t)(r))	\
   : val.type == BOOL_VALUE ? (val.uinteger = 0 || (r))		\
   : val.type == INTEGER_VALUE ? (val.integer=(int_t)(r))	\
   : (val.floating=(flt_t)(r)))

#define set_valuep(val, r)					\
  (val->type == UINTEGER_VALUE ? (val->uinteger=(uint_t)(r))	\
   : val->type == BOOL_VALUE ? (val->uinteger = 0 || (r))	\
   : val->type == INTEGER_VALUE ? (val->integer=(int_t)(r))	\
   : (val->floating=(flt_t)(r)))

#define ID_LEN 64
typedef char idname[ID_LEN];

typedef struct _token{
  token_type type;
  int real_pos;
  union {
    value v;
    idname id;
    op_type op;
    prefix_op_type prefix_op;
    open_delim_type open;
    close_delim_type close;
  };
}token;

#define get_token(tok)					\
  (tok.type == VALUE_TOKEN        ? (tok.v)		\
   : tok.type == OP_TOKEN         ? (tok.op)		\
   : tok.type == PREFIX_OP_TOKEN  ? (tok.prefix_op)	\
   : tok.type == OPEN_DELIM_TOKEN ? (tok.open)		\
   : (tok.close))

#define get_tokenp(tok)					\
  (tok->type == VALUE_TOKEN        ? (tok->v)		\
   : tok->type == OP_TOKEN         ? (tok->op)		\
   : tok->type == PREFIX_OP_TOKEN  ? (tok->prefix_op)	\
   : tok->type == OPEN_DELIM_TOKEN ? (tok->open)		\
   : (tok->close))

#define set_token(tok, r)					\
  (tok.type == VALUE_TOKEN        ? (tok.v = (r))		\
   : tok.type == OP_TOKEN         ? (tok.op = (r))		\
   : tok.type == PREFIX_OP_TOKEN  ? (tok.prefix_op =(r))	\
   : tok.type == OPEN_DELIM_TOKEN ? (tok.open = (r))		\
   : (tok.close=(r)))

#define set_tokenp(tok, r)					\
  (tok->type == VALUE_TOKEN        ? (tok->v = (r))		\
   : tok->type == OP_TOKEN         ? (tok->op = (r))		\
   : tok->type == PREFIX_OP_TOKEN  ? (tok->prefix_op =(r))	\
   : tok->type == OPEN_DELIM_TOKEN ? (tok->open = (r))		\
   : (tok->close=(r)))


#include "buf.h"
buf_dec(token);
#include "ibuf.h"
ibuf_dec(token);

typedef struct _function
{
  int i; //hurr
}function;

typedef enum {
  VALUE_ID = 0,
  CALL_ID,
  INVALID_ID
} ident_type;


typedef struct _ident
{
  ident_type type;
  idname name;
  union
  {
    value v;
    function f;
  };
}ident;

#include "list.h"
list_dec(ident);
#include "hash.h"
hash_dec(ident);

typedef struct _astate{
  ident_hash_table table;
  large_mask flags;
}astate;

typedef enum {
  VERBOSE_AFLAG=0x001,
  INVALID_AFLAG
}astate_flag;

#define verbose(state) ((state).flags & VERBOSE_AFLAG)
#define verbosep(state) verbose(*state)

/*error codes*/
typedef enum {
  INVALID_ERROR   =  -6,
  NOTFOUND_ERROR,
  DIVBYZERO_ERROR,
  SYNTAX_ERROR,
  MEMORY_ERROR,
  GENERAL_ERROR
}errorcode;

#endif /*_AIFALEENE_H_*/
