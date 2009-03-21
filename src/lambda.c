#include <stdlib.h>
#include "../include/tonburi.h"

sExpression *operator(sList *list){
  return car(list);
}

sExpression *operands(sList *list){
  return cdr(list);
}

sExpression *listOfValues(sExpression *exp, sEnvironment *env){
  if(isList(exp)){
    return cons(eval(car(toList(exp)), env),
                listOfValues(cdr(toList(exp)), env));
  }
  else
  {
    return eval(exp, env);
  }
}

Bool isLambdaType(sExpression *exp){
  return exp->type == LAMBDA_TAG ? TRUE : FALSE;
}

Bool isLambdaConst(sExpression *exp){
  if(isList(exp)){
    //first symbol is "lambda"
    if(firstSymbol(toList(exp), "lambda")){
      //parameters is valid
      if(isList( cadr(toList(exp)))){
        //body is valid
        if(isList( cdr(toList( cdr(toList(exp)))))){
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

Bool isApplication(sExpression *exp){
  if(isList(exp)){
    return TRUE;
  }
  return FALSE;
}

static sList *checkLambdaParameters(sLambda *lamd, sList *parameters){
  if(isNull(cdr(parameters))){
    return toList(cons(car(parameters),
                       &sNull));
  }

  sExpression *temp1 = car(parameters);
  if(isSymbol(temp1) && (strcmp((toSymb(temp1))->name, ".") == 0)){
    lamd->isVarArgument = TRUE;
    return toList(cons(cadr(parameters),
                       &sNull));
  }
  else{
    return toList(cons(temp1,
                       newExp(checkLambdaParameters(lamd, toList(cdr(parameters))), LIST_TAG)));
  }
}

sExpression *newLambda(sList *parameters, sList *body, sEnvironment *env){
  sLambda *lamd = (sLambda *)malloc(sizeof(sLambda));
  lamd->isVarArgument = FALSE;
  lamd->parameters = checkLambdaParameters(lamd, parameters);

  //printExp(car(body));

  lamd->body = body;
  lamd->frame = env;
  sExpression *exp = newExp(lamd, LAMBDA_TAG);
  return exp;
}
