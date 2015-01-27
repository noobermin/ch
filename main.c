#include "aifaleene.h"
#include "misc.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#ifdef __STDC_NO_VLA__
#error "Compile me on a platform/compiler that has VLAs!"
#endif /*__STDC_NO_VLA__*/

/* Exception handling.
 * 
 * These types currently only have context in this file.
 */
typedef struct _mem_guard
{
  int cur;
  large_mask alloc;
  /* These are done purposefully so that
   * this can be passed by value to a calling function
   */
  int *lineno;
  char *errstr;
  int *ret;
}mem_guard;


/*freer*/
#define _f_g(_g,call,n)	{ (call); _g.alloc&= ~(1<<n);}
#define _f(call,n) _f_g(_g,call,n)

/*exception jumper*/
#define gotoerr_gl(_g,LABEL,pos, err, vl...)				\
  { sprintf(_g.errstr, vl); *_g.lineno=pos; *_g.ret=err; goto LABEL; }
#define gotoerr_g(_g, pos, err, vl...) gotoerr_gl(_g,end,pos,err,vl)
#define gotoerr(pos, err, vl...) gotoerr_g(_g,pos,err,vl)

/*allocator*/
#define _a_gls(_g,LABEL,errstr,call) {                                  \
if (call)								\
  { gotoerr_gl(_g,LABEL, 0, MEMORY_ERROR, errstr); }			\
_g.alloc|=1<<(_g.cur++);						\
}
#define _a_gl(_g,LABEL,call) _a_gls(_g,LABEL,"Allocation Error",call)
#define _a_g(_g,call) _a_gl(_g,end,call)
#define _a(call) _a_g(_g,call)


static
line_error(const char *line, int pos,
	   const char* fmt,...)
{
  va_list vl;
  int i;
  va_start(vl,fmt);
  vprintf(fmt,vl);
  va_end(vl);
  printf("\n%s\n",line);
  if (pos < 0) return 0;
  
  for(i=0; i<pos; ++i) printf(" ");
  printf("^\n");
  return 0;
}

const char *pref_op_strs[] = {"-"};

const char *op_strs[] =
  /*              v--deliberate hack to keep this working with my scanner*/
  { "+","-","*","//","/","%","^",
    "==","!=",">","<",">=","<=",
    "&&","||",
    "=",""};

const int op_prec[] =
/*  +  -  *  // /  %  ^  == !=  >  <  >=  <=*/
  { 3, 3, 4, 5, 5, 5, 5, 2, 2,  2, 2, 2,  2,
/*  && || */
    1, 1,
  /* =  */
    -1,    
  };

static
precedence(token tok)
{
  return tok.type == OP_TOKEN ? op_prec[tok.op]
    : tok.type == OPEN_DELIM_TOKEN ? -2
    : tok.type == PREFIX_OP_TOKEN  ? 100
    : 100000;
}

const char *open_strs[] = {"(","{","["};

const char *close_strs[] = {")","}","]"};

const char *token_strs[] =
  {"value",
   "identifier",
   "operator",
   "prefix operator",
   "opening parentheical",
   "closing parentheical",
   "end of line",
   "invalid"
  };

const char *error_names[] =
  {"General Error",
   "Memory Error",
   "Syntax Error",
   "Division By Zero Error",
   "(Not) Found Error",
   "Invalid Error...?"
  };

static
isopendelim(int in)
{ return in == '(' || in == '{' || in == ']'; }

static
isclosedelim(int in)
{ return in == ')' || in == '}' || in == ']'; }

static
isop(int in)
{ 
  return in == '+' || in == '-' || in == '/'
    || in == '*'|| in == '%' || in == '^'
    || in == '=' || in == '>'|| in == '<'  
    || in == '!' || in == '&' || in == '|';
}

static
ispref(int in) { return in == '-'; }

/*static
isident(int in)
{ return isalphanum(in) || in == '_'; }*/

static
iswhitespace(int in)
{ return in == ' ' || in == '\t'; }

static
isendline(int in)
{ return in == '\0'; }

static token_type
identify_token(char in, large_mask expected)
{
  
  if (isalpha(in) || in=='_')
    return ID_TOKEN;
  else if (isdigit(in))
    return VALUE_TOKEN;
  else if (isop(in))
    return !(expected & OP_TOKEN) && ispref(in) ? PREFIX_OP_TOKEN : OP_TOKEN;
  else if (isopendelim(in))
    return OPEN_DELIM_TOKEN;
  else if (isclosedelim(in))
    return CLOSE_DELIM_TOKEN;
  else if (isendline(in))
    return ENDL_TOKEN;
  else
    return INVALID_TOKEN;
}

print_value(value v)
{
  char *fmt;
  switch(v.type)
    {
    case UINTEGER_VALUE:
    case INTEGER_VALUE:
      fmt = "%i";
      break;
    case FLOAT_VALUE:
      fmt = "%f";
      break;
    case BOOL_VALUE:
      fmt = v.uinteger ? "true" : "false";
      break;
    }  
  return printf(fmt, get_value(v));
}

#define print_err(code)				\
  printf(">>> %s:\n",error_names[-(code)-1])

dump_stack_opt(const token_ibuf* in, const char* sep, int normal)
{
  int i = (in->pos)+1;
  while( --i >= 0)
    {
      token cur = ibuf_getip(in, !normal ? i : in->pos-i );
      switch(cur.type)
	{
	case VALUE_TOKEN:
	  print_value(cur.v);
	  break;
	case ID_TOKEN:
	  printf("%s",cur.id);
	  break;
	case OP_TOKEN:
	  printf("%s",op_strs[cur.op]);
	  break;
	case PREFIX_OP_TOKEN:
	  printf("%s",pref_op_strs[cur.prefix_op]);
	  break;
	case OPEN_DELIM_TOKEN:
	  printf("(");
	  break;
	case CLOSE_DELIM_TOKEN:
	  printf(")");
	  break;
	}
      puts(sep);
    }
  return printf("\n");
}
#define dump_stack(st,sep) dump_stack_opt(st,sep,-1)

prefix_operate(value *r, prefix_op_type pre)
{
  switch(pre)
    {
    case NEG_PREF:
      set_valuep(r, - get_valuep(r));
      break;
    default:
      return -1;
    }
  return 0;
}

#define zero(v)						\
  (v.type == UINTEGER_VALUE || v.type == INTEGER_VALUE  ? 0 : 0.0)

operate(value l, value r, op_type op, value *ret)
{
  /*handle promotion*/
  ret->type = max(l.type,r.type);
  switch(op)
    {
    case ADD_OP:
      set_valuep(ret, get_value(l) + get_value(r));
      break;
    case SUB_OP:
      set_valuep(ret, get_value(l) - get_value(r));
      break;
    case MUL_OP:
      set_valuep(ret, get_value(l) * get_value(r));
      break;
    case POW_OP:
      set_valuep(ret, pow(get_value(l), get_value(r)));
      break;
		 
    case IDIV_OP:
      /*casting first*/
      l.uinteger = (unsigned long) get_value(l);
      r.uinteger = (unsigned long) get_value(r); 
      ret->type = l.type = r.type = UINTEGER_VALUE; /*intentional fall through*/
    case DIV_OP:
      if (get_value(r) == zero(r))
	return DIVBYZERO_ERROR;
      set_valuep(ret, get_value(l) / get_value(r));
      break;
    case MOD_OP:
      l.uinteger = (unsigned long) get_value(l);
      r.uinteger = (unsigned long) get_value(r);
      ret->type = l.type = r.type = UINTEGER_VALUE;
      if (get_value(r) == zero(r))
	return DIVBYZERO_ERROR;
      set_valuep(ret, l.uinteger % r.uinteger);
      break;
    case EQ_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) == get_value(r));
      break;
    case NOT_EQ_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) != get_value(r));
      break;
    case GREAT_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) > get_value(r));
      break;
    case LESS_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) < get_value(r));
      break;
    case GR_EQ_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) >= get_value(r));
      break;
    case LS_EQ_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) <= get_value(r));
      break;
    case LAND_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) && get_value(r));
      break;
    case LOR_OP:
      ret->type = BOOL_VALUE;
      set_valuep(ret, get_value(l) || get_value(r));
      break;
    }
  return 0;
}
#undef zero



static ident_list*
ident_list_find(const char* id,
		ident_list* l)
{
  for(;l!=NULL; l=l->next)
    if ( !strcmp(l->data.name,id) ) break;
  return l;
}
		
identifier_get(const char* id,
	       ident_hash_table table,
	       ident** out)
{
  int h = hash(id);

  ident_list *l = table[h];
  if (!l || !(l=ident_list_find(id,l)))
    return NOTFOUND_ERROR;  
  *out = &l->data;
  return 0;
}

identifier_setdefault(const ident* in,
		      ident_hash_table table)
{
  int h = hash(in->name);
  
  ident_list *l;
  if (table[h] && (l=ident_list_find(in->name,table[h])))
    {
      l->data = *in; return 0;
    }
  else
    {
      return ident_list_pushp(table+h,in) ? MEMORY_ERROR : 0;
    }
}
		      

identifier_add(ident* in,
	       ident_hash_table table)
{
  int h = hash(in->name);
  if (table[h] && ident_list_find(in->name,table[h]))
    return NOTFOUND_ERROR; /*here it is the opposite*/
  ident_list_pushp(table+h,in);
  return 0;
}

value_from_id(const char *id,
	      ident_hash_table table,
	      token *ret)
{
  ident* idp;
  int st;
  if(st = identifier_get(id,table,&idp)) return st;
  if(idp->type != VALUE_ID)
    return NOTFOUND_ERROR;
  ret->v    = idp->v;
  ret->type = VALUE_TOKEN;
  return 0;
}

/*used in evaluate and aifaleene.*/
#define tok_to_val(tok)							\
    if (tok.type == ID_TOKEN						\
	&& value_from_id(tok.id, state->table, &tok) == NOTFOUND_ERROR)	\
      {gotoerr(tok.real_pos, NOTFOUND_ERROR,"token \"%s\" not found.",tok.id);}

evaluate(astate* state, const token_ibuf* expr, int len,
	 token_ibuf* aux_stack,
	 mem_guard _g)
{
  for(int i=0; i<=len; ++i)
    {
      token cur = ibuf_getip(expr,i);
      if (cur.type == VALUE_TOKEN || cur.type == ID_TOKEN)
	{
	  token_ibuf_push(aux_stack,cur);
	}
      else if (cur.type == OP_TOKEN)
	{  
	  token l,r; int st;
	  value v;
	  if (token_ibuf_popv(aux_stack,2,&r,&l))
	    gotoerr(cur.real_pos, SYNTAX_ERROR, "incorrect number of arguments.");
	  if (cur.op == ASSIGN_OP)
	    {
	      ident id; 
	      if (l.type != ID_TOKEN)
		gotoerr(l.real_pos, SYNTAX_ERROR, "cannot assign to non-identifier");
	      tok_to_val(r);
	      id.v = v = r.v; id.type = VALUE_ID; strncpy(id.name, l.id, ID_LEN);
	      if (st=identifier_setdefault(&id, state->table))
		gotoerr(cur.real_pos, st, "failed in assignment.");
	    }
	  else /*normal operator*/
	    {
	      tok_to_val(l);
	      tok_to_val(r);
	      if (st=operate(l.v,r.v,cur.op,&v))
		gotoerr(cur.real_pos, st, "failed in operation %s", op_strs[cur.op]);
	    }
	  token_ibuf_push(aux_stack,
			  (token){.type = VALUE_TOKEN,
			      .real_pos = cur.real_pos,
			      .v        = v});
	}
      else if (cur.type == PREFIX_OP_TOKEN)
	{
	  int st = prefix_operate(&(ibuf_getp(aux_stack).v),cur.prefix_op);
	  /*should be no errors right now.*/
	}
    } /*for each out element*/
 end:
  return (*_g.ret);
}

aifaleene(char *line, astate* state)
{
  /*general stuff*/
  int n=0,i,len = strlen(line),ret=0;
  int pdepth=0;
  token_ibuf tok_stack, aux_stack, out_stack;
  token res;
  char errstr[max(len+1,64)];

  /*state information*/
  large_mask expected = VALUE_TOKEN |
    ID_TOKEN | OPEN_DELIM_TOKEN | PREFIX_OP_TOKEN |
    ENDL_TOKEN;
  enum {
    close_delim_unexpected,
    val_then_delim,
    close_delim_expected
  }close_state = close_delim_unexpected;
  /*error handling*/
  mem_guard _g =(mem_guard){.cur=0,     .alloc=0,
			    .lineno=&n, .errstr=errstr,
			    .ret=&ret};
  _a(token_ibuf_mk_sz(&tok_stack,len));
  /*parsing*/
  while(-1)
    {
      token tok;
      token_type type;
      /*skip whitespace*/
      n+=to_1st(line+n);
      tok.real_pos=n;
      tok.type = type = identify_token(*(line+n),expected);
      if (!(expected & type))
	gotoerr(n, SYNTAX_ERROR, "unexpected %s token.",token_strs[binplace(type)]);
      if (type == VALUE_TOKEN)
	{
#define FLT 37 
	  char base=FLT;
	  char im  = 0;
	  char *end;
	  if ( (*(line+n)) == '0')
	    {
	      char next = *(line+n+1);
	      if (next == 'b') !(base=2) && (n+=2);
	      else if (next == 'x') !(base=0x10) && (n+=2);
	      else if (next == 'd') !(base=10) && (n+=2);
	      else if (next>='1'&& next<='9')!(base=010) && ++n;
	    }

	  if (base==FLT)
	    {
	      tok.v.type = FLOAT_VALUE;
	      tok.v.floating = strtod(line+n, &end);
	    }
	  else if (base==10)
	    {
	      tok.v.type = INTEGER_VALUE;
	      tok.v.integer = strtol(line+n, &end,10);
	    }
	  else
	    {
	      tok.v.type = UINTEGER_VALUE;
	      tok.v.uinteger = strtoul(line+n, &end,base);
	    }
#undef FLT
	  n=end-line;
	  
	  expected |= ~(VALUE_TOKEN | INVALID_TOKEN | CLOSE_DELIM_TOKEN);
	  expected &= ~(VALUE_TOKEN);
	  if (close_state == val_then_delim)
	    close_state = close_delim_expected;
	  if (close_state == close_delim_expected)
	    expected |= CLOSE_DELIM_TOKEN;
	}
      else if (type == ID_TOKEN)
	{	  
	  /*for now, no arguments*/
	  /*first argument is good*/
	  int m = n+1,cplen=0;
	  for(;m < len;++m)
	    {
	      if (!(isalpha(line[m]) || isdigit(line[m])|| line[m]=='_'))
		break;
	    }
	  cplen = min(m-n,ID_LEN);
	  memcpy(tok.id, line+n, cplen);
	  cplen+1 < ID_LEN && (tok.id[cplen] = '\0');
	  n=m;
	  expected |= ~(VALUE_TOKEN | ID_TOKEN | INVALID_TOKEN | CLOSE_DELIM_TOKEN);
	  expected &= ~(VALUE_TOKEN | ID_TOKEN );
	  if (close_state == val_then_delim)
	    close_state = close_delim_expected;
	  if (close_state == close_delim_expected)
	    expected |= CLOSE_DELIM_TOKEN;
	}
	  /*scanning a lookup table, this
	   *is a hack in disguise, at the end of string,
	   *we are assured of no overruns because '/0' is not a
	   *valid character for anything, so the second char of the string
	   *at worst is '\0', no overrun, lol.
	   */ 
#define scan_table(table, TYPE)						\
	for (i=0; i != (int)INVALID_##TYPE; ++i)			\
	  /*we can do this because operators tend to be short ;)*/	\
	  if (*(line+n) == table[i][0])					\
	    {								\
	      /*if op is only one char long, instant match*/		\
	      if (table[i][1] == '\0')					\
		{ break; }						\
	      else if (*(line+n+1) == table[i][1])			\
		{ ++n; break;}						\
	    }								\
	++n;								\
	if (i==(int)INVALID_##TYPE)					\
	  {								\
	    printf("How in the hell did this happen?\n");		\
	    return -12345;						\
	  }							
      
      else if (type == OP_TOKEN)
	{
	  scan_table(op_strs, OP);
	  
	  tok.op = (op_type)i;
	  expected = VALUE_TOKEN | ID_TOKEN
	    | PREFIX_OP_TOKEN | OPEN_DELIM_TOKEN ;
	}
      else if (type == PREFIX_OP_TOKEN)
	{
	  scan_table(pref_op_strs, PREF);
	      tok.prefix_op = (prefix_op_type)i;
	      expected = VALUE_TOKEN | ID_TOKEN
		| PREFIX_OP_TOKEN | OPEN_DELIM_TOKEN ;
	}
      else if (type == OPEN_DELIM_TOKEN)
	{
	  scan_table(open_strs,OPEN);
	  tok.open = (open_delim_type)i;
	  //peek top
	  if (!ibuf_empty(tok_stack)
	      && ibuf_get(tok_stack).type == VALUE_TOKEN)
	    token_ibuf_push(&tok_stack,
	     (token){
	       .type=OP_TOKEN,
	       .real_pos=n,
	       .op = MUL_OP}
	     );
	  close_state = val_then_delim;
	  expected = VALUE_TOKEN |
	    ID_TOKEN | PREFIX_OP_TOKEN | OPEN_DELIM_TOKEN ;
	  ++pdepth;
	}
      else if (type == CLOSE_DELIM_TOKEN)
	{
	  if (!pdepth)
	    gotoerr(n, SYNTAX_ERROR, "unexpected closing delimiter");
	  scan_table(close_strs,CLOSE);
	  tok.close = (close_delim_type)i;
	  
	  expected = OP_TOKEN | ENDL_TOKEN;
	  if (--pdepth)
	    (expected |= CLOSE_DELIM_TOKEN) && (close_state = close_delim_expected);
	  else
	    close_state = close_delim_unexpected;
	}
      else if (type == ENDL_TOKEN)
	{
	  break;
	}
      token_ibuf_push(&tok_stack,tok);//this is guarenteed to be large enough
#undef scan_table
    }
  
  verbose(*state) && dump_stack(&tok_stack, "");
	     
  _a(token_ibuf_mk_sz(&aux_stack,len));
  _a(token_ibuf_mk_sz(&out_stack,len));
  /*to rpn*/
  for(i=0; i<=tok_stack.pos; ++i)
    {
      token tok = ibuf_geti(tok_stack,i);
      switch(tok.type){
      case ID_TOKEN:
      case VALUE_TOKEN:
	token_ibuf_push(&out_stack, tok);
	break;
	
      case PREFIX_OP_TOKEN:	
      case OP_TOKEN:
	/*peek top of stack*/
	if (!ibuf_empty(aux_stack) &&
	    precedence(ibuf_get(aux_stack)) >= precedence(tok))
	  {
	    token_ibuf_push(&out_stack,ibuf_get(aux_stack));
	    ibuf_get(aux_stack) = tok;/*swap*/
	  }
	else
	  token_ibuf_push(&aux_stack,tok);
  	break;
	
      case OPEN_DELIM_TOKEN:
	token_ibuf_push(&aux_stack,tok);
	break;
	
      case CLOSE_DELIM_TOKEN:
	while(!ibuf_empty(aux_stack) &&
	      ibuf_get(aux_stack).type != OPEN_DELIM_TOKEN)
	  {
	    token_ibuf_push(&out_stack,
			    token_ibuf_pop_unsafe(&aux_stack));
	  }
	  //assert(ibuf_get(aux_stack).type == OPEN_DELIM_TOKEN);
	token_ibuf_pop_unsafe(&aux_stack);
	break;
      }
    }
  while(!ibuf_empty(aux_stack))
    token_ibuf_push(&out_stack,token_ibuf_pop_unsafe(&aux_stack));
  
  _f(token_ibuf_free(&tok_stack),0);
  verbose(*state) && dump_stack(&out_stack, ",");
  /*evaluate*/
  if(evaluate(state, &out_stack, out_stack.pos, &aux_stack, _g))
    goto end; 
  _f(token_ibuf_free(&out_stack),2);
  
  /*printing out stack element.*/
  res = ibuf_get(aux_stack);
  tok_to_val(res);
  
  if(res.type != VALUE_TOKEN)
    gotoerr(res.real_pos, SYNTAX_ERROR, "Final token is not a value?");
  print_value(res.v); printf("\n");
  /*assign value to _*/
  ident _=(ident){.type=VALUE_ID,
		  .name = "_",
		  .v = res.v};
  int st;
  if(st=identifier_setdefault(&_, state->table))
    gotoerr(res.real_pos, st, "Error assigning to _");
  _f(token_ibuf_free(&aux_stack),1);
  ret = 0;
 end:
  if (_g.alloc & 0x01)
    {verbose(*state) && printf("freeing tok\n"); token_ibuf_free(&tok_stack);}
  if (_g.alloc & 0x02)
    {verbose(*state) && printf("freeing aux\n"); token_ibuf_free(&aux_stack);}
  if (_g.alloc & 0x04)
    {verbose(*state) && printf("freeing out\n"); token_ibuf_free(&out_stack);}
  if (! ret) return 0;
  /*error reporting*/
  print_err(ret);
  line_error(line, n, errstr);
  return ret;
}
#undef tok_to_val

astate_mk(astate *in){
  hash_init(in->table);
  in->flags=0; //returns 0;
}

astate_free(astate* in)
{
  ident_hash_free(in->table,NULL);
}

inspect_state(astate* in)
{
  for(int i=0;i<HASH_SIZE;++i)
    {
      ident_list *l=0;
      if(!in->table[i]) continue;
      printf("%04d: %p\n",i,in->table[i]);
      l=in->table[i];
      for(;l!=NULL;l=l->next)
	{
	  printf(" name: %s\n",l->data.name);
	  printf(" type: %s\n",l->data.type == VALUE_ID ? "value" : "function");
	  if (l->data.type == VALUE_ID)
	    { printf(" value:"); print_value(l->data.v); printf("\n");}
	}
      printf("done.\n");
    }
  return 0;
}

const char online_help[] = "\
usage:\n\
  At the > prompt, enter an expression to evaluate it.\n\
  For special commands, use a command starting with \#.\n\
commands:\n\
  #h           Output this help.\n\
  #i           Inspect the hash table of identifiers.\n\
  #v           Toggle verbosity during evaluation.\n\
  #q           Quit.";

main(int ac, char *av)
{
  int quit;
  unsigned int next=0;
  char* line;
  astate state;
  
  astate_mk(&state);
  for(;;)
    {
      line = readline(">");
      if (!line)
	{printf("\n"); break;}
      add_history(line);
      if (!first_few(line,"#q"))
	{ break; }
      else if(!first_few(line, "#i"))
	{ inspect_state(&state); continue; }
      else if(!first_few(line, "#v"))
	{ state.flags ^= VERBOSE_AFLAG; continue; }
      else if(!first_few(line, "#h"))
	{ puts(online_help); continue;}
      aifaleene(line, &state);
      free(line);
    }
  astate_free(&state);
  return 0;
}
