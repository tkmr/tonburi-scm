#include <stdio.h>
#include <stdlib.h>
#include "../include/tonburi.h"

sExpression *eval(sExpression *exp, sEnvironment *env){
  /* ------------------atom-----------------------*/
  /* 1, 10, false, null, "abc" */
  if(isSelfEval(exp))
  {
    return exp;
  }
  /* a symbol */
  else if(isVariable(exp, env))
  {
    return lookupVariable(toSymb(exp), env);
  }
  /* ------------------list-----------------------*/
  /* (quote blur blur) */
  else if(isQuoted(exp))
  {
    return textOfQuoted(exp);
  }
  /* (set! name value) */
  else if(isAssignment(exp))
  {
    return evalAssignment(exp, env);
  }
  /* (define name value) */
  else if(isDefinition(exp))
  {
    return evalDefine(exp, env);
  }
  else if(isDefinitionSyntax(exp))
  {
    return evalDefineSyntax(exp, env);
  }
  /* (if blur blur blur) */
  else if(isIf(exp))
  {
    return evalIf(toList(exp), env);
  }
  /* (lambda (args) (body)) */
  else if(isLambdaConst(exp))
  {
    sList *body;
    sList *param = toList( cadr(toList(exp)));
    sExpression *temp = cadr(toList( cdr(toList(exp))));
    if(isList(temp)){
      body = toList(temp);
    }else{
      body = cons(temp, &sNull);
    }
    return newLambda(param, body, env);
  }
  /* (syntax blur blur) syntax rule */
  else if(isSymbol(car(toList(exp))) && isSyntaxRule(eval(car(toList(exp)), env)))
  {
    sExpression *exp2 = evalSyntaxRule(toSyntax(eval(car(toList(exp)), env)), exp);
    return eval(exp2, env);
  }
  /* the other list (x . y) */
  else if(isApplication(exp))
  {
    sExpression *proexp = eval(operator(toList(exp)), env);
    if(isLambdaType(proexp) || isPrimitiveProc(proexp)){
      sExpression *operand = operands(toList(exp));
      sExpression *arguments = listOfValues(operand, env);
      return apply(proexp, arguments);
    }
  }

  return &sError;
}

static sExpression *checkArgumentsIter(sExpression *parameters, sExpression *arguments, Bool isVarArgument){
  if(isList(parameters) && isList(arguments)){
    if(isNull(cdr(toList(parameters))) && isVarArgument && isList(cdr(toList(arguments)))){
      return cons(arguments, &sNull);
    }else{
      return cons(car(toList(arguments)),
                  checkArgumentsIter(cdr(toList(parameters)),
                                     cdr(toList(arguments)),
                                     isVarArgument));
    }
  }else{
    return arguments;
  }
}
static sList *checkArguments(sList *parameters, sList *arguments, Bool isVarArgument){
  return toList(checkArgumentsIter(newExp(parameters, LIST_TAG),
                                   newExp(arguments,  LIST_TAG),
                                   isVarArgument));
}

static sExpression *checkParametersIter(sExpression *parameters, sExpression *arguments){
  if(isList(arguments)){
    if(isList(parameters) && isSymbol(car(toList(parameters)))){
      return cons(car(toList(parameters)),
                  checkParametersIter(cdr(toList(parameters)),
                                      cdr(toList(arguments))));
    }else{
      return &sNull;
    }
  }
  return &sNull;
}
static sList *checkParameters(sList *parameters, sList *arguments){
  sExpression *temp = checkParametersIter(newExp(parameters, LIST_TAG),
                                          newExp(arguments,  LIST_TAG));

  if(isList(temp)){
    return toList(temp);
  }else{
    return cons(&sNull, &sNull);
  }
}

sExpression *apply(sExpression *procOrLambda, sExpression *argument){
  if(isPrimitiveProc(procOrLambda))
  {
    sProc *cfunc = toProc(procOrLambda);
    return applyProc(cfunc, argument);
  }
  else if(isLambdaType(procOrLambda))
  {
    sLambda *lambd = toLambda(procOrLambda);
    sList *body = lambd->body;
    sList *arguments;
    sList *parameters;

    if(isList(argument)){
      //可変長引数のため
      parameters = checkParameters(lambd->parameters, toList(argument));
      arguments = checkArguments(parameters, toList(argument), lambd->isVarArgument);
    }else{
      parameters = lambd->parameters;
      arguments = toList(cons(argument, &sNull));
    }

    sEnvironment *env = extendEnvironment(parameters, arguments, lambd->frame);
    if(isList(car(body))){
      return evalSequence(body, env);
    }else{
      return eval(newExp(body, LIST_TAG), env);
    }
  }
  return &sNull;
}

sExpression *applyProc(sProc *proc, sExpression *arguments){
  return (*proc->Func)(arguments);
}

static Bool isLastExp(sList *exps){
  return isNull(cdr(exps));
}
static sExpression *firstExp(sList *exps){
  return car(exps);
}
static sList *restExp(sList *exps){
  return toList(cdr(exps));
}
sExpression *evalSequence(sList *exps, sEnvironment *env){
  if(isLastExp(exps)){
    return eval(firstExp(exps), env);
  }else{
    eval(firstExp(exps), env);
    return evalSequence(restExp(exps), env);
  }
}

sExpression *evalIf(sList *arguments, sEnvironment *env){
  sExpression *temp = cdr(arguments);
  if(isList(temp)){
    sList *args = toList(temp);
    sExpression *predicate = car(args);
    sExpression *trueExp   = car(toList(cdr(args)));
    sExpression *falseExp  = car(toList(cdr(toList(cdr(args)))));
    if(isTrue(eval(predicate, env))){
      return eval(trueExp, env);
    }else{
      return eval(falseExp, env);
    }
  }
  return &sNull;
}
