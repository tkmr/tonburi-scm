#include <stdio.h>
#include <tcutil.h>
#define TRUE 1
#define FALSE 0

/* ----- types ------------------------------- */
typedef int Bool;

typedef struct{
  char *name;
} sSymbol;

typedef struct{
  char *value;
} sString;

typedef struct {
  int value;
} sNumber;

typedef struct s_expression{
  void *value;
  int type;
  int valueSize;
} sExpression;

typedef struct{
  sExpression *car;
  sExpression *cdr;
} sList;

typedef struct s_environment{
  struct s_environment *parent;
  TCMAP *varmap;
} sEnvironment;

typedef struct {
  sList *parameters;
  sList *body;
  sEnvironment *frame;
  Bool isVarArgument;
} sLambda;

typedef struct {
  sList *syntaxes;
  sList *rules;
} sSyntax;

typedef sExpression* (*cFuncPointer)(sExpression *);

typedef struct {
  cFuncPointer Func;
} sProc;

typedef struct {
  sExpression *exp;
  sEnvironment *env;
} sThunk;

typedef enum{
  NULL_TAG, TRUE_TAG, FALSE_TAG, NUMBER_TAG, SYMBOL_TAG,
  LIST_TAG, LAMBDA_TAG, STRING_TAG, PROC_TAG, ERROR_TAG, EXPRESSION_TAG, SYNTAX_TAG, THUNK_TAG
} sType;

int sTypeSize[60];

/* primitive object --------------------------------*/
struct s_expression sTrue;
struct s_expression sFalse;
struct s_expression sNull;
struct s_expression sError;
struct s_environment *sGlobalEnvironment;
FILE *sSTDIN;
FILE *sSTDOUT;

/* scheme compiler --------------------------------------- */
sExpression *eval(sExpression *exp, sEnvironment *env);
sExpression *evalIf(sList *arguments, sEnvironment *env);
sExpression *evalAssignment(sExpression *exp, sEnvironment *env);
sExpression *evalDefine(sExpression *exp, sEnvironment *env);
sExpression *evalSequence(sList *exps, sEnvironment *env);
sExpression *evalDefineSyntax(sExpression *exp, sEnvironment *env);
sExpression *evalSyntaxRule(sSyntax *syntax, sExpression *body);
sExpression *evalSyntaxRuleIter(sExpression *syntax, sExpression *parameters, sExpression *rule);
sExpression *apply(sExpression *procOrLambda, sExpression *arguments, sEnvironment *env);
sExpression *applyProc(sProc *proc, sExpression *arguments);
sExpression *readScheme(char *code);
sExpression *car(sList *list);
sExpression *cdr(sList *list);
sExpression *cadr(sList *list);
sExpression *cons(sExpression *left, sExpression *right);
sExpression *cloneList(sExpression *exp);
Bool isAtom(sExpression *exp);
Bool isList(sExpression *exp);
Bool isEqual(sExpression *exp1, sExpression *exp2);
Bool isVariable(sExpression *exp, sEnvironment *env);
Bool isQuoted(sExpression *exp);
Bool isSymbol(sExpression *exp);
Bool isSelfEval(sExpression *exp);
Bool isNumber(sExpression *exp);
Bool isBoolean(sExpression *exp);
Bool isNull(sExpression *exp);
Bool isString(sExpression *exp);
Bool isTrue(sExpression *exp);
Bool isApplication(sExpression *exp);
Bool isLambdaType(sExpression *exp);
Bool isLambdaConst(sExpression *exp);
Bool isIf(sExpression *exp);
Bool isAssignment(sExpression *exp);
Bool isDefinition(sExpression *exp);
Bool isDefinitionSyntax(sExpression *exp);
Bool isPrimitiveProc(sExpression *exp);
Bool isErrorType(sExpression *exp);
Bool isSyntaxRule(sExpression *exp);
Bool isSyntaxRules(sExpression *exp);
Bool isNoParent(sEnvironment *env);
Bool isThunk(sExpression *exp);
Bool setVariable(sSymbol *symbol, sExpression *value, sEnvironment *env);
Bool defineVariable(sSymbol *symbol, sExpression *value, sEnvironment *env);
Bool defineProc(cFuncPointer cfunction, char *name, sEnvironment *env);
sExpression *operator(sList *list);
sExpression *operands(sList *list);
sExpression *newNumber(int num);
sExpression *newSymbol(char *name);
sExpression *newString(char *name);
sExpression *newLambda(sList *parameters, sList *body, sEnvironment *env);
sExpression *newSyntax(sList *rules, sList *syntaxes);
sExpression *newExp(void *value, sType type);
sExpression *newProc(cFuncPointer cfunction);
sExpression *newList(int argnum, ...);
sExpression *newThunk(sExpression *exp, sEnvironment *env);
sExpression *lookupVariable(sSymbol *symbol, sEnvironment *env);
sExpression *textOfQuoted(sExpression *exp);
sExpression *listOfValues(sExpression *exp, sEnvironment *env);
sEnvironment *extendEnvironment(sList *parameterNames, sList *arguments, sEnvironment *parentEnv);

/* helper methods ------------------------------------------ */
#define toList(name)   ((sList *)(name->value))
#define toSymb(name)   ((sSymbol *)(name->value))
#define toNum(name)    ((sNumber *)(name->value))
#define toLambda(name) ((sLambda *)(name->value))
#define toString(name) ((sString *)(name->value))
#define toProc(name)   ((sProc *)(name->value))
#define toSyntax(name) ((sSyntax *)(name->value))
#define toThunk(name) ((sThunk *)(name->value))
Bool firstSymbol(sList *list, char *name);
void printExp(sExpression *exp);
void fprintExp(sExpression *exp, FILE *fp);
int getLexInput(char *buffer, int max_size);
void setLexInput(char *inputs);
sExpression *getYaccResult();
void setYaccResult(sExpression *exp);
void initGlobalEnvironment();
void initPrimitiveProcs();
void initPrimitiveVariable();
void driveInterctiveLoop(FILE *fpin, FILE *fpout, int evalLine);
Bool callProcSymbolEqualSub(sExpression *temp1, sExpression *temp2);

/* for lazy evaluate ----------------------------------------- */
sExpression *forceIt(sExpression *obj);
sExpression *delayIt(sExpression *exp, sEnvironment *env);
sExpression *actualValue(sExpression *exp, sEnvironment *env);
sExpression *applyLazly(sExpression *procOrLambda, sExpression *arguments, sEnvironment *env);
#define LAZY_EVAL 1
sList *checkArguments(sList *parameters, sList *arguments, Bool isVarArgument);
sList *checkParameters(sList *parameters, sList *arguments);
