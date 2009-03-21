#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tonburi.h"

Bool isSyntaxRule(sExpression *exp){
  return (exp->type == SYNTAX_TAG) ? TRUE : FALSE;
}

Bool isDefinitionSyntax(sExpression *exp){
  if(isList(exp) && isSymbol(car(toList(exp)))){
    if(strcmp(toSymb(car(toList(exp)))->name, "define-syntax") == 0){
      return TRUE;
    }
  }
  return FALSE;
}

Bool isSyntaxRules(sExpression *exp){
  if(isList(exp) && isSymbol(car(toList(exp)))){
    if(strcmp(toSymb(car(toList(exp)))->name, "syntax-rules") == 0){
      return TRUE;
    }
  }
  return FALSE;
}

static void *applySyntaxToRule(sExpression *key, sExpression *value, sExpression *rule){
  if(isList(key) && isList(cdr(toList(key))) && isSymbol(cadr(toList(key))) && (strcmp(toSymb(cadr(toList(key)))->name, "...") == 0) &&
     isList(rule) && isList(cdr(toList(rule))) && isSymbol(cadr(toList(rule))) && (strcmp(toSymb(cadr(toList(rule)))->name, "...") == 0))
  {
    if(isList(value)){
      if(isSymbol(car(toList(rule)))){
        sExpression *tempRule = newSymbol(toSymb(car(toList(rule)))->name);
        evalSyntaxRuleIter(car(toList(key)), car(toList(value)), tempRule);

        if(isSymbol(tempRule) && strcmp(toSymb(tempRule)->name, toSymb(car(toList(rule)))->name) == 0){
          //printExp(rule);
        }
        else{
          if(isNull(cdr(toList(value)))){
            rule->value = cons(tempRule, &sNull)->value;
          }
          else{
            rule->value = cons(tempRule,
                             cons(car(toList(rule)),
                                  cdr(toList(rule))))->value;
            rule->type = LIST_TAG;
            applySyntaxToRule(key, cdr(toList(value)), cdr(toList(rule)));
          }
        }
      }
    }
  }
  else if(isList(rule)){
    applySyntaxToRule(key, value, car(toList(rule)));
    applySyntaxToRule(key, value, cdr(toList(rule)));
  }
  else if(isSymbol(rule) && strcmp(toSymb(key)->name, toSymb(rule)->name) == 0){
    rule->value = value->value;
    rule->type = value->type;
  }
}

sExpression *evalSyntaxRuleIter(sExpression *syntax, sExpression *parameters, sExpression *rule){
  if(isList(syntax) && isList(parameters)){
    if(isSymbol(cadr(toList(syntax))) && (strcmp(toSymb(cadr(toList(syntax)))->name, "...") == 0)){
      applySyntaxToRule(syntax, parameters, rule);
      return &sTrue;
    }
    else{
      sExpression *result = evalSyntaxRuleIter(car(toList(syntax)), car(toList(parameters)), rule);
      if(isNull(result)){
        return &sNull;
      }
      else{
        return evalSyntaxRuleIter(cdr(toList(syntax)), cdr(toList(parameters)), rule);
      }
    }
  }
  else if(isNull(syntax) && !isNull(parameters)){
    return &sNull;
  }
  else if(isSymbol(syntax)){
    if(strcmp(toSymb(syntax)->name, "_") == 0){
      return &sTrue;
    }
    else{
      applySyntaxToRule(syntax, parameters, rule);
      return &sTrue;
    }
  }
  else if(isNull(syntax) && isNull(parameters)){
    return &sTrue;
  }
  return &sNull;
}

sExpression *evalSyntaxRule(sSyntax *syntax, sExpression *parameters){
  sExpression *clonedRule = cloneList(newExp(syntax->rules, LIST_TAG));
  sExpression *temp = &sNull;
  sExpression *syntax_tmp = car(syntax->syntaxes);
  sExpression *rule = car(toList(clonedRule));
  sExpression *syntaxcdr = cdr(syntax->syntaxes);
  sExpression *rulecdr = cdr(toList(clonedRule));

  temp = evalSyntaxRuleIter(syntax_tmp, parameters, rule);
  while(isList(syntaxcdr) && isNull(temp)){
    syntax_tmp = car(toList(syntaxcdr));
    rule = car(toList(rulecdr));
    syntaxcdr = cdr(toList(syntaxcdr));
    rulecdr = cdr(toList(rulecdr));
    temp = evalSyntaxRuleIter(syntax_tmp, parameters, rule);
  }
  return rule;
}

sExpression *callProcSyntaxRules(sExpression *argument){
  if(isList(argument)){
    sExpression *body = cdr(toList(argument));
    sExpression *temp = car(toList(argument));
    sExpression *syntaxes = cons(car(toList(temp)), &sNull);
    sExpression *rules = cons(cadr(toList(temp)), &sNull);
    while(isList(body)){
      temp = car(toList(body));
      rules = cons(cadr(toList(temp)), rules);
      syntaxes = cons(car(toList(temp)), syntaxes);
      body = cdr(toList(body));
    }
    return newSyntax(toList(rules), toList(syntaxes));
  }
  return &sError;
}

sExpression *evalDefineSyntax(sExpression *exp, sEnvironment *env){
  if(isDefinitionSyntax(exp) && isSyntaxRules( cadr(toList( cdr(toList(exp)))))){
    sSymbol *name = toSymb(cadr(toList(exp)));
    sExpression *rules = cdr(toList( cdr(toList( cadr(toList( cdr(toList( exp))))))));
    defineVariable(name, callProcSyntaxRules(rules), env);
    return &sTrue;
  }
  return &sError;
}

sExpression *newSyntax(sList *rules, sList *syntaxes){
  sSyntax *syntax = (sSyntax *)malloc(sizeof(sSyntax));
  syntax->rules = rules;
  syntax->syntaxes = syntaxes;
  return newExp(syntax, SYNTAX_TAG);
}
