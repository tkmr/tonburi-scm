#include <string.h>
#include "../include/tonburi.h"

Bool isQuoted(sExpression *exp){
  if(isList(exp)){
    if(firstSymbol(toList(exp), "quote")){
      return TRUE;
    }
  }
  return FALSE;
}

sExpression *textOfQuoted(sExpression *exp){
  if(isQuoted(exp)){
    return cadr(toList(exp));
  }
  return &sNull;
}
