#include <stdlib.h>
#include <tcutil.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <tcutil.h>
#include "../include/tonburi.h"

int main(int argc, char *argv[])
{
  initGlobalEnvironment();

  FILE *fp;
  fp = fopen("/home/tatsuya/Dropbox/HomeDev/tonburi/example/test1.scm", "r");
  driveInterctiveLoop(fp, sSTDOUT, 0);
  fclose(fp);
  return 0;

  if(argc > 1)
  {
    if(strcmp(argv[1], "-i") == 0)
    {
      driveInterctiveLoop(sSTDIN, sSTDOUT, 1);
    }
/*     else if(argc > 2 && strcmp(argv[1], "-f") == 0) */
/*     { */
/*       FILE *fp; */
/*       if((fp = fopen(argv[2], "r")) == NULL){ */
/*         printf("error!! open a file %s", argv[2]); */
/*         exit(EXIT_FAILURE); */
/*       } */
/*       driveInterctiveLoop(fp, sSTDOUT, 0); */
/*       fclose(fp); */
/*     } */
  }
  else
  {
    sEnvironment *env = sGlobalEnvironment;
    sExpression *left = newNumber(123);
    sExpression *right = newNumber(444);

    sList *list1 = toList(cons(left, right));
    sExpression *result1 = car(list1);
    sExpression *result2 = cdr(list1);

    printf("left is %d, right is %d \n", toNum(result1)->value, toNum(result2)->value);

    sExpression *number = newNumber(999);
    printExp(eval(number, env));
    printExp(eval(&sTrue,  env));
    printExp(eval(&sFalse, env));
    printExp(eval(&sNull,  env));
    printExp(eval(newString("hello string"), env));

    sExpression *ifSymbol = newSymbol("if");
    sExpression *ifList = newList(4, ifSymbol, &sTrue, newNumber(987), newNumber(789));
    printExp(eval(ifList, env));

    //store variable
    sList *parameters = toList(cons(newSymbol("hoge"), &sNull));
    sList *arguments  = toList(cons(newString("hello world"), &sNull));
    sEnvironment *newenv = extendEnvironment(parameters, arguments, sGlobalEnvironment);

    //lookup
    //sExpression *result = lookupVariable(toSymb(newSymbol("hoge")), newenv);
    printExp(eval(newSymbol("hoge"), newenv));

    sList *parameters2 = toList(cons(newSymbol("hoho"), newSymbol("big")));
    sList *arguments2 = toList(cons(newString("I am tom"), newNumber(9999)));
    sEnvironment *newenv2 = extendEnvironment(parameters2, arguments2, newenv);

    //lookup nested env ---------------------------------
    printExp(eval(newSymbol("hoho"), newenv2));
    printExp(eval(newSymbol("big"), newenv2));
    printExp(eval(newSymbol("hoge"), newenv2));
    printExp(eval(newSymbol("not_found"), newenv2));

    printExp(eval(newList(3,
                          newSymbol("define"),
                          newSymbol("myname"),
                          newString("this is define")), newenv2));

    printExp(eval(newSymbol("myname"), newenv2));

    printExp(eval(newList(3,
                          newSymbol("set!"),
                          newSymbol("hoge"),
                          newString("this is new value of hoge")), newenv2));

    printExp(eval(newSymbol("hoge"), newenv2));
    printExp(eval(newSymbol("hoge"), newenv));

    eval(newList(2,
                 newSymbol("print"),
                 newString("hi me no wa ru")), newenv2);

    //yacc -------------------------------------------------------------------------
    eval(readScheme("(print hoge)"), newenv2);
    eval(readScheme("(print 'hoge')"), newenv2);
    eval(readScheme("(print \"hoge\")"), newenv2);

    printExp(newList(3,
                     newSymbol("hoge"),
                     newString("hello"),
                     newNumber(123)));

    printExp(eval(newList(3,
                          newSymbol("+"),
                          newNumber(11),
                          newNumber(22)), newenv2));

    eval(readScheme("(print (+ 100 244))"), newenv2);
    eval(readScheme("(print (- 300 200))"), newenv2);
    eval(readScheme("(print (* 10  24))"), newenv2);
    eval(readScheme("(print (+ (- (/ 300 15) 10) (* 3 4)))"), newenv2);

    sExpression *lam = newList(3,
                               newSymbol("lambda"),
                               newList(1, newSymbol("n")),
                               newList(3,
                                       newSymbol("*"),
                                       newSymbol("n"),
                                       newSymbol("n"))
                               );

    printExp(eval(newList(2,
                          lam,
                          newNumber(11)), env));

    eval(readScheme("(define pow (lambda (n) (* n n)))"), env);
    eval(readScheme("(print (pow 100))"), env);
  }
  return 0;
}

