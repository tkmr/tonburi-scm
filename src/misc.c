#include <tcutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tonburi.h"

void initGlobalEnvironment(){
  sSTDIN = stdin;
  sSTDOUT = stdout;

  sExpression *temp = (sExpression *)malloc(sizeof(sExpression *));
  sTrue.type = TRUE_TAG;
  sTrue.value = temp;
  sFalse.type = FALSE_TAG;
  sFalse.value = temp;
  sNull.type = NULL_TAG;
  sNull.value = temp;
  sError.type = ERROR_TAG;
  sError.value = temp;

  sTypeSize[NULL_TAG] = sizeof(sExpression);
  sTypeSize[TRUE_TAG] = sizeof(sExpression);
  sTypeSize[FALSE_TAG] = sizeof(sExpression);
  sTypeSize[NUMBER_TAG] = sizeof(sNumber);
  sTypeSize[SYMBOL_TAG] = sizeof(sSymbol);
  sTypeSize[LIST_TAG] = sizeof(sList);
  sTypeSize[LAMBDA_TAG] = sizeof(sLambda);
  sTypeSize[STRING_TAG] = sizeof(sString);
  sTypeSize[PROC_TAG] = sizeof(sProc);
  sTypeSize[ERROR_TAG] = sizeof(sExpression);
  sTypeSize[EXPRESSION_TAG] = sizeof(sExpression);
  sTypeSize[SYNTAX_TAG] = sizeof(sSyntax);
  sTypeSize[THUNK_TAG] = sizeof(sThunk);
  sTypeSize[ENV_TAG] = sizeof(sEnvironment);

  sGlobalEnvironment = newEnvironment(cons(&sNull, &sNull), cons(&sNull, &sNull));
  initPrimitiveProcs();
  initPrimitiveVariable();
}

void driveInterctiveLoop(FILE *fpin, FILE *fpout, int evalLine){
  int maxSize = 10000;
  char inputs[maxSize];
  if(evalLine){
    while(fgets(inputs, maxSize, fpin) != NULL){
      printf("input: %s\n", inputs);
      fprintExp(eval(readScheme(inputs), sGlobalEnvironment), fpout);
    }
  }else{
    TCXSTR *temp = tcxstrnew();
    while(fread(inputs, sizeof(char), maxSize, fpin) > 0){
      tcxstrcat2(temp, inputs);
    }
    fprintExp(eval(readScheme(tcxstrptr(temp)), sGlobalEnvironment), fpout);
  }
}

Bool isEqual(sExpression *exp1, sExpression *exp2){
  return (exp1 == exp2) ? TRUE : FALSE;
}

Bool isBoolean(sExpression *exp){
  return (exp->type==TRUE_TAG || exp->type==FALSE_TAG) ? TRUE : FALSE;
}

Bool isNull(sExpression *exp1){
  return (exp1->type == NULL_TAG) ? TRUE : FALSE;
}

Bool isSelfEval(sExpression *exp){
  return isNumber(exp) || isBoolean(exp) || isNull(exp) || isString(exp) || isPrimitiveProc(exp);
}

Bool isErrorType(sExpression *exp){
  return (exp->type == ERROR_TAG) ? TRUE : FALSE;
}

Bool firstSymbol(sList *list, char *name){
  sExpression *first = car(list);
  if(isSymbol(first)){
    return (strcmp(toSymb(first)->name, name) == 0) ? TRUE : FALSE;
  }
  return FALSE;
}

Bool isTrue(sExpression *exp){
  if(exp->type == FALSE_TAG || exp->type == NULL_TAG){
    return FALSE;
  }else{
    return TRUE;
  }
}

Bool isIf(sExpression *exp){
  if(isList(exp)){
    if(firstSymbol(toList(exp), "if")){
      return TRUE;
    }
  }
  return FALSE;
}

sExpression *newExp(void *value, sType type){
  sExpression *temp = (sExpression *)malloc(sizeof(sExpression));
  temp->value = value;
  temp->type = type;
  temp->valueSize = sTypeSize[type];
  return temp;
}

sExpression *cloneList(sExpression *exp){
  if(isList(exp)){
    sList *temp = toList(exp);
    return cons(cloneList(car(temp)),
                cloneList(cdr(temp)));
  }
  if(isSymbol(exp)){
    return newSymbol(toSymb(exp)->name);
  }
  if(isNumber(exp)){
    return newNumber(toNum(exp)->value);
  }
  if(isString(exp)){
    return newString(toString(exp)->value);
  }
  return exp;
}
