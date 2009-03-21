#include <tcutil.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../include/tonburi.h"

/* --------------- init global variable ----------- */
void initPrimitiveVariable(){
  eval(newList(3, newSymbol("define"), newSymbol("nil"), newExp(&sNull, NULL_TAG)),
       sGlobalEnvironment);
  eval(newList(3, newSymbol("define"), newSymbol("#t"), newExp(&sTrue, TRUE_TAG)),
       sGlobalEnvironment);
  eval(newList(3, newSymbol("define"), newSymbol("#f"), newExp(&sFalse, FALSE_TAG)),
       sGlobalEnvironment);
  eval(newList(3, newSymbol("define"), newSymbol("#e"), newExp(&sError, ERROR_TAG)),
       sGlobalEnvironment);
}

/* --------------- variable ----------------------- */
Bool isVariable(sExpression *exp, sEnvironment *env){
  if(isSymbol(exp)){
    return TRUE;
  }
  return FALSE;
}

Bool isSymbol(sExpression *exp){
  return exp->type == SYMBOL_TAG ? TRUE : FALSE;
}

static TCXSTR *getTypeTag(char *name){
  TCXSTR *str = tcxstrnew();
  tcxstrcat2(str, name);
  tcxstrcat2(str, "_type");
  return str;
}

Bool isNoParent(sEnvironment *env){
  return (env->parent == 0) ? TRUE : FALSE;
}

sExpression *lookupVariable(sSymbol *symbol, sEnvironment *env){
  int i = 0;
  int keySize = strlen(symbol->name) * sizeof(char);
  TCXSTR *str = getTypeTag(symbol->name);

  void *result = tcmapget(env->varmap, symbol->name, keySize, &i);
  if(i == 0){
    //fail find
    if(isNoParent(env)){

      return &sNull;
    }else{
      return lookupVariable(symbol, env->parent);
    }
  }else{
    //success find
    sType *type = (sType *)tcmapget(env->varmap, tcxstrptr(str), tcxstrsize(str), &i);
    sExpression *resultExp = newExp(result, *type);
    return resultExp;
  }
}

sExpression *newSymbol(char *name){
  sSymbol *symb = (sSymbol *)malloc(sizeof(sSymbol));
  symb->name = name;
  sExpression *exp = newExp(symb, SYMBOL_TAG);
  return exp;
}

static void putVars(sList *parameterNames, sList *arguments, TCMAP *mapdb){
  sExpression *firstP = car(parameterNames);
  sExpression *firstA = car(arguments);
  char *pName;
  void *aPointer;
  void *pPointer;
  int aSize;
  int pSize;

  if(isNull(firstP)){
    return;
  }

  if(isSymbol(firstP)){
    pName = toSymb(firstP)->name;
  }else{
    return;
  }

  pPointer = pName;
  pSize = strlen(pName) * sizeof(char);
  aPointer = firstA->value;
  aSize = firstA->valueSize;

  TCXSTR *str = getTypeTag(pName);
  tcmapput(mapdb, pPointer, pSize, aPointer, aSize);
  tcmapput(mapdb, (char *)tcxstrptr(str), tcxstrsize(str), &(firstA->type), sizeof(sType));

  sExpression *remainP = cdr(parameterNames);
  sExpression *remainA = cdr(arguments);
  if(isList(remainP) && isList(remainA)){
    return putVars(toList(remainP), toList(remainA), mapdb);
  }else if(isSymbol(remainP)){
    sList *lastOneParameter = toList(cons(remainP, &sNull));
    sList *lastOneArgument  = toList(cons(remainA, &sNull));
    return putVars(lastOneParameter, lastOneArgument, mapdb);
  }
  return;
}

sEnvironment *newEnvironment(sList *parameterNames, sList *arguments){
  sEnvironment *env = (sEnvironment *)malloc(sizeof(sEnvironment));
  env->varmap = tcmapnew();
  putVars(parameterNames, arguments, env->varmap);
  return env;
}

sEnvironment *extendEnvironment(sList *parameterNames, sList *arguments, sEnvironment *parentEnv){
  sEnvironment *newEnv = newEnvironment(parameterNames, arguments);
  newEnv->parent = parentEnv;
  return newEnv;
}

/* ------------- assingment value ---------------------*/
static sSymbol *assignSynbol(sExpression *exp){
  if(isList(exp)){
    sExpression *temp = cadr(toList(exp));
    if(isSymbol(temp)){
      return toSymb(temp);
    }
  }
  return toSymb(newSymbol("_error"));
}

static sExpression *assignValue(sExpression *exp){
  if(isList(exp)){
    sExpression *temp = cdr(toList( cdr(toList(exp))));
    if(isList(temp)){
      return car(toList(temp));
    }else{
      return temp;
    }
  }
  return &sNull;
}

sExpression *evalAssignment(sExpression *exp, sEnvironment *env){
  if(setVariable(assignSynbol(exp), eval(assignValue(exp), env), env)){
    return &sTrue;
  }
  return &sFalse;
}

Bool setVariable(sSymbol *symbol, sExpression *value, sEnvironment *env){
  if(strcmp(symbol->name, "_error") != 0){
    int i = 0;
    void *result = tcmapget(env->varmap, symbol->name, (strlen(symbol->name) * sizeof(char)), &i);
    if(i == 0)
    {
      if(isNoParent(env)){
        return FALSE;
      }else{
        return setVariable(symbol, value, env->parent);
      }
    }
    else
    {
      sExpression *exp = newExp(symbol, SYMBOL_TAG);
      putVars(toList(cons(exp, &sNull)),
              toList(cons(value, &sNull)),
              env->varmap);
      return TRUE;
    }
  }
  return FALSE;
}

Bool isAssignment(sExpression *exp){
  if(isList(exp)){
    if(firstSymbol(toList(exp), "set!")){
      return TRUE;
    }
  }
  return FALSE;
}

/* --------- define value --------------------- */
static sSymbol *definitionSymbol(sExpression *exp){
  if(isList(exp)){
    sExpression *temp = cadr(toList(exp));
    if(isSymbol(temp)){
      return toSymb(temp);
    }else{
      return toSymb(car(toList(temp)));
    }
  }
  return toSymb(newSymbol("_error"));
}

static sExpression *definitionValue(sExpression *exp, sEnvironment *env){
  if(isList(exp)){
    sExpression *temp = cadr(toList(exp));
    if(isSymbol(temp)){
      temp = cdr(toList( cdr(toList(exp))));
      if(isList(temp)){
        return car(toList(temp));
      }else{
        return temp;
      }
    }else{
      sExpression *parameters = cdr(toList(temp));
      sExpression *body = cdr(toList( cdr(toList(exp))));
      return cons(newSymbol("lambda"),
                  cons(parameters,
                       body));
    }
  }
  return &sNull;
}


sExpression *evalDefine(sExpression *exp, sEnvironment *env){
  if(defineVariable(definitionSymbol(exp), eval(definitionValue(exp, env), env), env)){
    return &sTrue;
  }
  return &sFalse;
}

Bool defineVariable(sSymbol *symbol, sExpression *value, sEnvironment *env){
  if(strcmp(symbol->name, "_error") != 0){
    sExpression *exp = newExp(symbol, SYMBOL_TAG);
    putVars(toList(cons(exp, &sNull)),
            toList(cons(value, &sNull)),
            env->varmap);
    return TRUE;
  }
  return FALSE;
}

Bool isDefinition(sExpression *exp){
  if(isList(exp)){
    if(firstSymbol(toList(exp), "define")){
      return TRUE;
    }
  }
  return FALSE;
}
