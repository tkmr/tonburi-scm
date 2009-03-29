#include <stdlib.h>
#include "../include/tonburi.h"

Bool isAmb(sExpression *exp){
  if(isList(exp)){
    if(firstSymbol(toList(exp), "amb")){
      return TRUE;
    }
  }
  return FALSE;
}

sExpression *ambChoices(sExpression *exps){
  if(isList(exps)){
    return cdr(toList(exps));
  }
  return &sNull;
}

static sExpression *ambEvalSequenceLoop(sExpression *firstProc, sExpression *restProcs, sEnvironment *env, sExpression *succeed, sExpression *fail){
  if(isList(restProcs)){
    sExpression *nextProc = car(toList(restProcs));
    sExpression *next = eval(newList(4,
                                     firstProc,
                                     env,
                                     newList(6,
                                             newSymbol("lambda"),
                                             newList(2, newSymbol("avalue"), newSymbol("fail2")),
                                             nextProc,
                                             env,
                                             succeed,
                                             newSymbol("fail2")),
                                     fail), env);
    return ambEvalSequenceLoop(next, cdr(toList(restProcs)), env, succeed, fail);
  }
  return eval(firstProc, env);
}

sExpression *ambEvalSequence(sExpression *exps, sEnvironment *env, sExpression *succeed, sExpression *fail){
  return ambEvalSequenceLoop(car(toList(exps)), cdr(toList(exps)), env, succeed, fail);
}

static sExpression *ambEvalif(sExpression *exp, sEnvironment *env, sExpression *succeed, sExpression *fail){
  sExpression *pproc = cadr(toList(exp));
  sExpression *cproc = caddr(toList(exp));
  sExpression *aproc = cadddr(toList(exp));
  return eval(newList(4,
                      pproc,
                      env,
                      newList(6,
                              newSymbol("lambda"),
                              newList(2, newSymbol("predvalue"), newSymbol("fail2")),
                              newSymbol("if"),
                              newSymbol("predvalue"),
                              newList(4, cproc, newExp(env, ENV_TAG), succeed, newSymbol("fail2")),
                              newList(4, aproc, newExp(env, ENV_TAG), succeed, newSymbol("fail2"))),
                      fail), env);
}

sExpression *ambEval(sExpression *exp, sEnvironment *env, sExpression *succeed, sExpression *fail){
  if(isIf(exp)){
    return ambEvalif(exp, env, succeed, fail);
  }
  return eval(cons(succeed,
                   cons(eval(exp, env),
                        cons(fail, &sNull))), env);
}

