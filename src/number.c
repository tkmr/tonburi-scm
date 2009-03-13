#include <stdio.h>
#include <stdlib.h>
#include "../include/tonburi.h"

sExpression *newNumber(int num){
  sNumber *number = (sNumber *)malloc(sizeof(sNumber));
  number->value = num;
  sExpression *exp = newExp(number, NUMBER_TAG);
  return exp;
}

Bool isNumber(sExpression *exp){
  return (exp->type == NUMBER_TAG) ? TRUE : FALSE;
}
