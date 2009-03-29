%{
 #include <stdio.h>
 #include <string.h>
 #include "../include/tonburi.h"
 #define YYDEBUG 1
%}
%union {
  int      int_value;
  char     *symbol_name;
  char     *string_value;
  sExpression *expression_type;
}
%token <int_value> INTEGER
%token <symbol_name> SYMBOL
%token <string_value> STRING
%token LIST_START
%token LIST_END
%token QUOTE
%type  <expression_type> expression list list_expression
%%
program
  : expression
  {
    setYaccResult($1);
  }
  | expression program
  {
    setYaccResult($1);
  }
  ;
expression
  : QUOTE expression
  {
    $$ = cons(newSymbol("quote"), cons($2, &sNull));
  }
  | list
  | INTEGER
  {
    $$ = newNumber($1);
  }
  | SYMBOL
  {
    $$ = newSymbol($1);
  }
  | STRING
  {
    $$ = newString($1);
  }
  ;
list
  : LIST_START expression list_expression LIST_END
  {
    $$ = cons($2, $3);
  }
  | LIST_START expression LIST_END
  {
    $$ = cons($2, &sNull);
  }
  | LIST_START LIST_END
  {
    $$ = cons(&sNull, &sNull);
  }
  ;
list_expression
  : expression list_expression
  {
    $$ = cons($1, $2);
  }
  | expression
  {
    $$ = cons($1, &sNull);
  }
  ;

%%
int
yyerror(char const *str)
{
  extern char *yytext;
  fprintf(stderr, "parser error near %s\n", yytext);
  return 0;
}
