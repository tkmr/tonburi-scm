#include <stdio.h>
#include <stdlib.h>
#include "../include/tonburi.h"

/* mapping from C function to Scheme ---- */
static sExpression *callProcPrintf(sExpression *arguments){
  if(isList(arguments)){
    printExp(car(toList(arguments)));
  }else{
    printExp(arguments);
  }
  return &sNull;
}

/* math functions */
static sExpression *callProcIntFrame(sExpression *argument, int(*callback)(int a, int b)){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    if(isNumber(temp1) && isNumber(temp2)){
      int i = (*callback)((toNum(temp1))->value, (toNum(temp2))->value);
      return newNumber(i);
    }
  }

  return &sError;
}

static int _intAdd(int a, int b){
  return a + b;
}
static int _intSub(int a, int b){
  return a - b;
}
static int _intMul(int a, int b){
  return a * b;
}
static int _intDiv(int a, int b){
  return a / b;
}

static sExpression *callProcIntAdd(sExpression *arguments){
  return callProcIntFrame(arguments, _intAdd);
}
static sExpression *callProcIntSub(sExpression *arguments){
  return callProcIntFrame(arguments, _intSub);
}
static sExpression *callProcIntMul(sExpression *arguments){
  return callProcIntFrame(arguments, _intMul);
}
static sExpression *callProcIntDiv(sExpression *arguments){
  return callProcIntFrame(arguments, _intDiv);
}
static sExpression *callProcIntEqual(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    if(isNumber(temp1) && isNumber(temp2)){
      return ((toNum(temp1))->value == (toNum(temp2))->value) ? &sTrue : &sFalse;
    }
  }
  return &sError;
}
static sExpression *callProcIntLT(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    if(isNumber(temp1) && isNumber(temp2)){
      return ((toNum(temp1))->value < (toNum(temp2))->value) ? &sTrue : &sFalse;
    }
  }
  return &sError;
}
static sExpression *callProcIntGT(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    if(isNumber(temp1) && isNumber(temp2)){
      return ((toNum(temp1))->value > (toNum(temp2))->value) ? &sTrue : &sFalse;
    }
  }
  return &sError;
}


/* list functions */
static sExpression *callProcListCons(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    return cons(car(arguments), cadr(arguments));
  }

  return &sError;
}
static sExpression *callProcListCar(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp;
    if(isList(temp = car(arguments))){
      return car(toList(temp));
    }else{
      return temp;
    }
  }
  return &sError;
}
static sExpression *callProcListCdr(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp;
    if(isList(temp = car(arguments))){
      return cdr(toList(temp));
    }else{
      return cdr(arguments);
    }
  }
  return &sError;
}

/* misc */
Bool callProcSymbolEqualSub(sExpression *temp1, sExpression *temp2){
  int result = (temp1->value == temp2->value);
  if(isSymbol(temp1) && isSymbol(temp2)){
    result = strcmp(toSymb(temp1)->name, toSymb(temp1)->name) == 0;
  }else if(isNumber(temp1) && isNumber(temp2)){
    result = (toNum(temp1)->value) == (toNum(temp2)->value);
  }else if(isString(temp1) && isString(temp2)){
    result = strcmp(toString(temp1)->value, toString(temp2)->value) == 0;
  }
  return result == TRUE ? TRUE : FALSE;
}

static sExpression *callProcSymbolEqual(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    int result = callProcSymbolEqualSub(temp1, temp2);
    return result == TRUE ? &sTrue : &sFalse;
  }
  return &sError;
}

static sExpression *callProcSymbolEqualType(sExpression *argument){
  if(isList(argument)){
    sList *arguments = toList(argument);
    sExpression *temp1 = car(arguments);
    sExpression *temp2 = cadr(arguments);
    return (temp1->type == temp2->type) ? &sTrue : &sFalse;
  }
  return &sError;
}

static sExpression *callProcIsAtom(sExpression *argument){
  if(isList(argument)){
    sExpression *temp1 = car(toList(argument));
    if(isList(temp1)){
      return &sFalse;
    }else{
      return &sTrue;
    }
  }
  return &sError;
}

static sExpression *callProcIsNull(sExpression *argument){
  if(isList(argument)){
    sExpression *temp1 = car(toList(argument));
    return isNull(temp1)==TRUE ? &sTrue : &sFalse;
  }
  return &sError;
}

/* define functions to symbols */
void initPrimitiveProcs(){
  defineProc(callProcPrintf, "print", sGlobalEnvironment);
  defineProc(callProcIntAdd, "+", sGlobalEnvironment);
  defineProc(callProcIntSub, "-", sGlobalEnvironment);
  defineProc(callProcIntDiv, "/", sGlobalEnvironment);
  defineProc(callProcIntMul, "*", sGlobalEnvironment);
  defineProc(callProcIntEqual, "=", sGlobalEnvironment);
  defineProc(callProcIntLT, "<", sGlobalEnvironment);
  defineProc(callProcIntGT, ">", sGlobalEnvironment);
  defineProc(callProcSymbolEqual, "equal?", sGlobalEnvironment);
  defineProc(callProcSymbolEqualType, "equalType?", sGlobalEnvironment);
  defineProc(callProcIsAtom, "atom?", sGlobalEnvironment);
  defineProc(callProcIsNull, "null?", sGlobalEnvironment);
  defineProc(callProcListCar, "car", sGlobalEnvironment);
  defineProc(callProcListCdr, "cdr", sGlobalEnvironment);
  defineProc(callProcListCons, "cons", sGlobalEnvironment);
}

/* helper functions --------------------- */
Bool defineProc(cFuncPointer cfunction, char *name, sEnvironment *env){
  sExpression *pexp = newProc(cfunction);
  eval(cons(newSymbol("define"),
            cons(newSymbol(name),
                 pexp)), env);
  return TRUE;
}

Bool isPrimitiveProc(sExpression *exp){
  return exp->type == PROC_TAG ? TRUE : FALSE;
}

sExpression *newProc(cFuncPointer cfunction){
  sProc *p = (sProc *)malloc(sizeof(sProc));
  p->Func = cfunction;
  sExpression *pexp = newExp(p, PROC_TAG);
  return pexp;
}

static void printExp_sub(sExpression *exp, FILE *fp){
  static int indent;

  if(isNumber(exp))
  {
    fprintf(fp, "%d ", toNum(exp)->value);
  }
  else if(isString(exp))
  {
    fprintf(fp, "'%s'", toString(exp)->value);
  }
  else if(isBoolean(exp))
  {
    fprintf(fp, (isTrue(exp) ? "True" : "False"));
  }
  else if(isNull(exp))
  {
    fprintf(fp, "Null");
  }
  else if(isSymbol(exp))
  {
    fprintf(fp, "#%s", toSymb(exp)->name);
  }
  else if(isList(exp))
  {
    int i;
    sExpression *temp = car(toList(exp));
    sExpression *remain = cdr(toList(exp));
    fprintf(fp, " (");
    printExp_sub(temp, fp);
    fprintf(fp, "\n");

    indent += 3;
    for(i = 0; i<indent; i++){ fprintf(fp, " "); }
    printExp_sub(remain, fp);
    indent -= 3;

    fprintf(fp, ")");
  }
  else if(isErrorType(exp))
  {
    fprintf(fp, "Error type!!");
  }
  else if(isLambdaType(exp))
  {
    sLambda *lam = toLambda(exp);
    fprintf(fp, "Lambda type\n");
    printExp_sub(newExp(lam->parameters, LIST_TAG), fp);
    fprintf(fp, "\n");
    printExp_sub(newExp(lam->body, LIST_TAG), fp);
  }
  else if(isSyntaxRule(exp))
  {
    fprintf(fp, "Syntax rules type \n");
    printExp_sub(newExp(toSyntax(exp)->syntaxes, LIST_TAG), fp);
    fprintf(fp, "\n");
    printExp_sub(newExp(toSyntax(exp)->rules, LIST_TAG), fp);
  }
  else
  {
    fprintf(fp, "e: unknown this type %d", exp->type);
  }
}

void printExp(sExpression *exp){
  printExp_sub(exp, sSTDOUT);
  fprintf(sSTDOUT, "\n");
}

void fprintExp(sExpression *exp, FILE *fp){
  printExp_sub(exp, fp);
  fprintf(fp, "\n");
}
