#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include "dbg.h"
#include "map.h"
#include "bstrlib.h"

typedef struct Argument{
    bstring token;
    bstring name;
    bstring help;
    bstring value;
}Argument;

typedef struct ArgumentParser{
    Map *args_m;
    Vector *args_v;
    bstring description;
    bstring help;
}ArgumentParser;

ArgumentParser *Argparse_New_Argument_Parser(const bstring description,const int argc,const char *argv[]);
Argument *Argument_New(const bstring token,const bstring name,const bstring help);

int Argparse_Add_Argument(ArgumentParser *parser,bstring token,bstring name,bstring help);
int Argparse_Parse();
void Argparse_Destroy(ArgumentParser *parser);
void Argparse_Argyment_Destroy(Argument *arg);

#endif // ARGPARSE_H_
