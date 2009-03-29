#include <stdlib.h>
#include "../include/tonburi.h"

Bool isThunk(sExpression *obj){
  return (obj->type == THUNK_TAG) ? TRUE : FALSE;
}

sExpression *newThunk(sExpression *exp, sEnvironment *env){
  sThunk *thunkObj = (sThunk *)malloc(sizeof(sThunk));
  thunkObj->exp = exp;
  thunkObj->evaledExp = &sNull;
  thunkObj->env = env;
  return newExp(thunkObj, THUNK_TAG);
}

sExpression *forceIt(sExpression *obj){
  if(isThunk(obj)){
    if(toThunk(obj)->evaledExp == &sNull){
      toThunk(obj)->evaledExp = actualValue(toThunk(obj)->exp, toThunk(obj)->env);
    }
    return toThunk(obj)->evaledExp;
  }else{
    return obj;
  }
}

sExpression *delayIt(sExpression *exp, sEnvironment *env){
  return newThunk(exp, env);
}

sExpression *actualValue(sExpression *exp, sEnvironment *env){
  return forceIt(eval(exp, env));
}

static sExpression *listOfActualValues(sExpression *exps, sEnvironment *env){
  if(isList(exps)){
    return  cons(actualValue(car(toList(exps)), env),
                 listOfActualValues(cdr(toList(exps)), env));
  }
  return &sNull;
}

static sExpression *listOfDelayedArgs(sExpression *exps, sEnvironment *env){
  if(isList(exps)){
    return cons(delayIt(car(toList(exps)), env),
                listOfDelayedArgs(cdr(toList(exps)), env));
  }
  return &sNull;
}

sExpression *applyLazly(sExpression *procOrLambda, sExpression *argument, sEnvironment *env){
  if(isPrimitiveProc(procOrLambda))
  {
    sProc *cfunc = toProc(procOrLambda);
    return applyProc(cfunc, listOfActualValues(argument, env));
  }
  else if(isLambdaType(procOrLambda))
  {
    sLambda *lambd = toLambda(procOrLambda);
    sList *body = lambd->body;
    sList *arguments;
    sList *parameters;

    //delayed
    argument = listOfDelayedArgs(argument, env);

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


/* eval.c ///////////////////////////////////////////////////////////

  if(isApplication(exp))
  {
    if(LAZY_EVAL){
      sExpression *proexp = actualValue(operator(toList(exp)), env);
      if(isLambdaType(proexp) || isPrimitiveProc(proexp)){
        sExpression *operand = operands(toList(exp));
        return apply(proexp, operands, env);
      }
    }
  }

*/
