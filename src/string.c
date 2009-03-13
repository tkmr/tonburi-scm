#include <stdlib.h>
#include <string.h>
#include "../include/tonburi.h"

Bool isString(sExpression *exp){
  return (exp->type == STRING_TAG) ? TRUE : FALSE;
}

sExpression *newString(char *name){
  sString *str = (sString *)malloc(sizeof(sString));
  str->value = name;
  sExpression *exp = newExp(str, STRING_TAG);
  return exp;
}

