#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../tritree/src/tritree.h"
#include "bstrlib.h"
#include "colors.h"
#include "dbg.h"

typedef struct Argument {
  bstring type;
  bstring token;
  bstring name;
  bstring help;
  bstring value;
  bstring def;
} Argument;

typedef struct ArgumentParser {
  TriTree *args;
  bstring description;
} ArgumentParser;

ArgumentParser *Argparse_New_Argument_Parser(const bstring description,
                                             const int argc,
                                             const char *argv[]);
Argument *Argument_New(const bstring type, const bstring token,
                       const bstring name, const bstring def,
                       const bstring help);

int Argparse_Add_Int(ArgumentParser *parser, bstring token, bstring name,
                     bstring def, bstring help);
int Argparse_Add_String(ArgumentParser *parser, bstring token, bstring name,
                        bstring def, bstring help);
int Argparse_Add_Bool(ArgumentParser *parser, bstring token, bstring name,
                      bstring def, bstring help);

int Argparse_Parse(ArgumentParser *parser);
void Argparse_Destroy(ArgumentParser *parser);
void Argparse_Argument_Destroy(Argument *arg);

void *Argparse_Get(ArgumentParser *args, bstring name);
void Argparse_Print_Help(ArgumentParser *args);

#endif // ARGPARSE_H_