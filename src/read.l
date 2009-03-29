%{
  #include <stdio.h>
  #include <string.h>
  #include "../include/tonburi.h"
  #include "y.tab.h"

  #undef  YY_INPUT
  #define YY_INPUT(buffer, result, max_size) (result = getLexInput(buffer, max_size))

  int
  yywrap(void)
  {
    return 1;
  }
%}
%%
"'" {
  return QUOTE;
}
"(" {
  return LIST_START;
}
")" {
  return LIST_END;
}
[1-9][0-9]* {
  int num;
  sscanf(yytext, "%d", &num);
  yylval.int_value = num;
  return INTEGER;
}
[a-zA-Z0-9_+*/=<>\.?!@#-]+ {
  yylval.symbol_name = strdup(yytext);
  return SYMBOL;
}
\"[^\"]*\" {
  char *str = strdup(yytext + 1);
  *(str + strlen(str) - 1) = '\0';
  yylval.string_value = str;
  return STRING;
}

%%
