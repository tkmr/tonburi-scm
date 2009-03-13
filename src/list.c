#include <stdlib.h>
#include <stdarg.h>
#include "../include/tonburi.h"

Bool isList(sExpression *exp){
  return (exp->type == LIST_TAG) ? TRUE : FALSE;
}

sExpression *newList(int argnum, ...){
  int i;
  va_list ap;
  sExpression *temp;
  sExpression *tempList[argnum];

  va_start(ap, argnum);
  for(i = 0; i < argnum; i++){
    tempList[argnum - (i + 1)] = va_arg(ap, sExpression*);
  }
  va_end(ap);

  temp = cons(tempList[0], &sNull);
  for(i = 1; i < argnum; i++){
    temp = cons(tempList[i], temp);
  }

  return temp;
}

sExpression *car(sList *list){
  return list->car;
}

sExpression *cdr(sList *list){
  return list->cdr;
}

sExpression *cadr(sList *list){
  sExpression *right = cdr(list);
  if(isList(right)){
    return car(toList(right));
  }
  return &sNull;
}

sExpression *cons(sExpression *left, sExpression *right){
  sList *list = (sList *)malloc(sizeof(sList));
  list->car = left;
  list->cdr = right;
  sExpression *listexp = newExp(list, LIST_TAG);
  return listexp;
}
