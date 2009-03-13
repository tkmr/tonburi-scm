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
  if(isList(rule)){
    applySyntaxToRule(key, value, car(toList(rule)));
    applySyntaxToRule(key, value, cdr(toList(rule)));
  }
  else if(isSymbol(rule) && strcmp(toSymb(key)->name, toSymb(rule)->name) == 0){
    rule->value = value->value;
    rule->type = value->type;
  }
}
static sExpression *evalSyntaxRuleIter(sExpression *syntax, sExpression *parameters, sExpression *rule){
  if(isList(syntax) && isList(parameters)){
    sExpression *result = evalSyntaxRuleIter(car(toList(syntax)), car(toList(parameters)), rule);
    if(isNull(result)){
      return &sNull;
    }
    else{
      return evalSyntaxRuleIter(cdr(toList(syntax)), cdr(toList(parameters)), rule);
    }
  }
  else if(isSymbol(syntax)){
    if(strcmp(toSymb(syntax)->name, "_") == 0){
      return &sTrue;
    }
    else if((strcmp(toSymb(syntax)->name, "...") == 0)){
      applySyntaxToRule(syntax, parameters, rule);
      return &sTrue;
    }
    else if(isList(parameters)){
      applySyntaxToRule(syntax, car(toList(parameters)), rule);
      return &sTrue;
    }
    else{
      applySyntaxToRule(syntax, parameters, rule);
      return &sTrue;
    }
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
      syntaxes = cons(cdr(toList(temp)), syntaxes);
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
