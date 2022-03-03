#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <caaa/bstring/bstrlib.h>
#include <caaa/colors.h>
#include <caaa/dbg.h>
#include <caaa/tritree.h>

typedef struct Argument {
  bstring type;
  bstring token;
  bstring name;
  bstring help;
  bstring value;
  bstring def;
} Argument;

typedef struct ArgumentParser {
  TriTree *args_t;
  TriTree *args_n;
  bstring description;
  bstring progname;
} ArgumentParser;

void PrintArgs(void *value, void *data);

ArgumentParser *Argparse_New_Argument_Parser(const bstring description);
Argument *Argument_New(const bstring type, const bstring token,
                       const bstring name, const bstring def,
                       const bstring help);

int Argparse_Add_Int(ArgumentParser *parser, const char *token,
                     const char *name, const char *def, const char *help);
int Argparse_Add_String(ArgumentParser *parser, const char *token,
                        const char *name, const char *def, const char *help);
int Argparse_Add_Bool(ArgumentParser *parser, const char *token,
                      const char *name, const char *def, const char *help);

int Argparse_Parse(ArgumentParser *parser, int argc, char *argv[]);
void Argparse_Destroy(ArgumentParser *parser);
void Argparse_Argument_Destroy(Argument *arg);

Argument *Argparse_Get(ArgumentParser *args, const char *name);
Argument *Argparse_Find(ArgumentParser *args, const char *token);
void Argparse_Print_Help(ArgumentParser *args);

#define ArgBoolData(A) bdata((A)) == "true" ? true : false
#define ArgIntData(A) atoi(bdata((A)))
#define ArgStrData(A) (A)

#endif // ARGPARSE_H_
