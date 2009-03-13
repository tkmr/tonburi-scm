#include <stdlib.h>
#include <string.h>
#include "../include/tonburi.h"

struct {
  char *inputs;
  int size;
} yyinString;

sExpression *yaccResult;

static void initLexInput(){
  yaccResult = &sNull;
  yyinString.inputs = "";
  yyinString.size = 0;
}

sExpression *readScheme(char *code){
  initLexInput();
  setLexInput(code);
  yyparse();
  return getYaccResult(sGlobalEnvironment);
}

void setLexInput(char *inputs){
  yyinString.inputs = (char *)malloc(sizeof(char) * (strlen(inputs) + 1));
  yyinString.size = strlen(inputs);
  strcpy(yyinString.inputs, inputs);
}

int getLexInput(char *buffer, int max_size){
  int result;
  if(yyinString.size < 1){
    result = 0;
  }else if(max_size > yyinString.size){
    strncpy(buffer, yyinString.inputs, yyinString.size);
    result = yyinString.size;
    yyinString.inputs += result;
    yyinString.size -= result;
  }else{
    strncpy(buffer, yyinString.inputs, max_size);
    result = max_size;
    yyinString.inputs += result;
    yyinString.size -= result;
  }
  return result;
}

sExpression *getYaccResult(sEnvironment *env){
  sExpression *temp = newList(1,
                              newList(3,
                                      newSymbol("lambda"),
                                      cons(&sNull, &sNull),
                                      yaccResult));
  return temp;
}

void setYaccResult(sExpression *exp){
  yaccResult = cons(exp, yaccResult);
}

