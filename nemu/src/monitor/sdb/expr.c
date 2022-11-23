/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/paddr.h>

#undef DBG_TAG
#undef DBG_LVL
#define DBG_TAG          "sdb_expr"
#define DBG_LVL          DBG_LOG
#include <debug_log.h> 

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_DEREF,TK_NEG, 

  /* TODO: Add more token types */
  TK_DEC, TK_HEX,TK_REG, 
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"[0-9]+", TK_DEC},
  {"0[xX][0-9a-fA-F]+", TK_HEX},
  {"^\\$[Xx](3[0-1]|[0-2]?[0-9])$", TK_REG},
   {"\\$(\\$0|ra|[sgt]p|t[0-6]|a[0-7]|s([0-9]|1[0-1]))", TK_REG},//registers
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
 
  {"\\-", '-'},          // sub
  {"\\*", '*'},          // multiply
  {"/", '/'},           // div
  {"\\(", '('},
  {"\\)", ')'},
   {"==", TK_EQ},        // equal
	{"!=", TK_NEQ},
	{"&&", TK_AND},
	{"\\|\\|", TK_OR},
	{"!", '!'},
  
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        LOG_D("match rules[%d] = \"%s\" at position %d with len %d: %.*s, nr_token : %d",
            i, rules[i].regex, position, substr_len, substr_len, substr_start, nr_token);

        position += substr_len;
        if (nr_token >= sizeof(tokens)/sizeof(tokens[0]))
        {
            LOG_D("token is overflow");
        }
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        // printf("token_type : %d \r\n", rules[i].token_type);

        switch (rules[i].token_type) {
          case TK_DEC:
          case TK_HEX:
          case TK_REG:
            strncpy( tokens[nr_token].str, substr_start, substr_len); 
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          break;
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          break;
          case TK_NEQ:
          case TK_AND:
          case TK_OR:
          case TK_EQ:
          case '!':         
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          break;
          default: TODO();
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
bool check_parentheses_is_legal(int p, int q)
{
  int i;
  uint32_t check = 0;
  for (i = p; i <= q; i++)
  {
      if ( tokens[i].type == '(' )
      {          
          check ++;
          // printf("check : %d\r\n", check);
      }        
      else if (  tokens[i].type == ')' )
      {
        if(check > 0)
        {
           check --;
          // printf("check : %d\r\n", check);
        }         
        else
          return false;
      } 
  }
//  printf("check : %d\r\n", check);
  if (check != 0)
    return false;

  return true;
}

bool check_parentheses(int start, int end)
{
  if( (tokens[start].type != '(')  ||  (tokens[end].type != ')') )
    return false;
  if (check_parentheses_is_legal(start + 1,  end -1 ) != true)
    return false;

  return true;  
}

typedef struct stack{
  char *buffer;
  uint32_t size;
  uint32_t top;
}stack_t;

static int stack_init(stack_t **s, uint32_t size)
{
  char *p;
  stack_t *ps = NULL;

  ps = malloc(sizeof(stack_t));
  if (ps == NULL)
    return -1;
 
  p = malloc(sizeof(char) * size);
  if (p == NULL)
    return -1;

  ps->buffer = p;
  ps->size = size;
  ps->top = 0;

  *s = ps;
  return 0;
}

static int stack_push(stack_t *s, char op)
{
  if (s->top >= s->size)
  {
    return -1;
  }
  s->buffer[s->top] = op;
  s->top++;

  return 0;
}

static int stack_pop(stack_t *s, char *op)
{
  if (s->top <= 0)
  {
    return -1;
  }

  s->top--;
  *op = s->buffer[s->top]; 

  return 0;
}

static int stack_is_empty(stack_t *s)
{
  return (s->top == 0) ? 0 : -1;
}

static int stack_destroy(stack_t *s)
{
  free( s->buffer );
  free(s);

  return 0;
}

int stack_get_top(stack_t *s, char *op)
{
  if(stack_is_empty(s) == 0)
    return -1;
  *op = s->buffer[s->top - 1]; 
  return 0;
}

//获得运算符的优先级
static int get_priority(char c)
{
	if (c == '(')
		return 1;    //由于栈顶为左括号时符号可直接压栈，因此将其等级设为最低
	if (c == '+' || c == '-')
		return 2;
	if (c == '*' || c == '/')
		return 3;

  return -1;
}

//STFW: 使用逆波兰式实现简单计算器的功能; 中缀表达式转化为后缀表达式
static int find_primary_operator(int p, int q)
{
  int ret;
  int i;
  stack_t *ps = NULL;
  char op_type = 0;
  int op_priority = 0;
  int stack_top_op_priority = 0;
  int primary_operator = 0;
  assert ((q - p + 1) > 0); 

  ret = stack_init(&ps, q - p + 1);
  assert(ret == 0);
  // void(ret);
  // dbg_log("start : %d, end : %d , primary op position : %d ", p, q, primary_operator);
  for (i = p; i <= q; i++)
  {
    if (tokens[i].type >= TK_DEC)   //过滤数字
    {
      continue;
    }
    if ( stack_is_empty(ps) == 0 )     //空栈或者运算符是左括号时直接将运算符压栈
    {
      stack_push(ps, tokens[i].type);
      primary_operator = i;
    }
    else if ( tokens[i].type == '(' ) 
    {
      stack_push(ps, tokens[i].type);
    }
    else if( tokens[i].type == ')' )    //运算符为右括号时，开始弹栈，直到弹出左括号
    {
      while (1) {
        stack_pop(ps, &op_type);
        if (op_type == '(')
          break;
      }
    }
    else    //栈顶与当前字符都是普通运算符时，如果当前字符优先级小，则弹栈，直到其比栈顶元素优先级高，再压栈
    {
        stack_get_top(ps , &op_type);
        op_priority = get_priority(tokens[i].type);
        stack_top_op_priority = get_priority(op_type);
        while(stack_top_op_priority  >= op_priority )
        {
          stack_pop(ps , &op_type);
          if (stack_is_empty(ps) == 0) 
          {
            primary_operator = i;
            break;
          }
            
          stack_top_op_priority = get_priority(op_type);
        }
        stack_push(ps, tokens[i].type);
    }
  }
  stack_destroy(ps);

  LOG_D("start : %d, end : %d , primary op position : %d ", p, q, primary_operator);
  return primary_operator;
}

static uint32_t eval(int p, int q, bool *success) {
  
  int op = 0;
  int op_type;
  uint32_t val1, val2;

  LOG_D("start : %d, end : %d ", p, q);
  *success = true;
  if(check_parentheses_is_legal(p, q) != true)
  {
    *success = false;
    assert(0);
  }
    
  if (p > q) {
    /* Bad expression */
    assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    switch (tokens[p].type)
    {
      case TK_HEX:
         sscanf(tokens[p].str, "%x", &val1); 
         break;
      case TK_DEC:
        sscanf(tokens[p].str, "%d", &val1); 
      case TK_REG:
        return isa_reg_str2val(tokens[p].str, success);
        break;
      default:
        assert(0);
    }
    return  val1;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else {
    // op = the position of 主运算符 in the token expression;
    op = find_primary_operator(p, q);
    if ( (op ==  '(') || ( op == ')' ))
    {
      *success = false;
      return 0;
      
    }
    op_type = tokens[op].type;
    if (op_type == '!' || op_type == TK_NEG || op_type == TK_DEREF) {
      uint32_t val = eval(op + 1, q, success);
      if (!*success) {
        return 0;
      }

      switch (op_type) {
      case '!':
        return !val;
      case TK_NEG:
        return -val;
      case TK_DEREF:
        return paddr_read(val, 4);
      default:
        assert(0);
      }
    }

    val1 = eval(p, op - 1, success);
    if (*success == false)
      return 0;
      
    val2 = eval(op + 1, q, success);
    if (*success == false)
      return 0;

    switch (op_type) {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2; /* ... */
    case '*':
      return val1 * val2; /* ... */
    case '/':
      if (val2 == 0) {
        *success = false;
        return 0;
      }
      else
      {
        return val1 / val2; /* ... */
      }       
    case TK_NEQ:
      return val1 != val2;
    case TK_EQ:
      return val1 == val2;
    case TK_AND:
      return val1 && val2;
    case TK_OR:
      return val1 || val2;
    default:
      *success = false;
      return 0;
    }
  }
}

word_t expr(char *e, bool *success) {
  int i;

  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  // printf("nr_token : %d \r\n", nr_token);

  for (i = 0; i < nr_token; i++) {
    if (tokens[i].type == '-' &&
        (i == 0 || (tokens[i - 1].type == '(') || (tokens[i - 1].type == '+') ||
         (tokens[i - 1].type == '-') || (tokens[i - 1].type == '*') ||
         (tokens[i - 1].type == '/'))) {
      tokens[i].type = TK_NEG;
    }
    if (tokens[i].type == '*' &&
        (i == 0 || (tokens[i - 1].type == '(') || (tokens[i - 1].type == '+') ||
         (tokens[i - 1].type == '-'))) {
       tokens[i].type = TK_DEREF;    //# 指针解引用
    }
  }
  word_t ret = 0;  
  /* TODO: Insert codes to evaluate the expression. */
  ret = eval(0, nr_token-1, success);
  // printf(" = 0x%x\r\n",ret);
  return ret;
}
