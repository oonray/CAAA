#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include "dbg.h"
#include "map.h"
#include "bstrlib.h"

typedef struct ArgumentParser{
    Map *args;
    bstring description;
    bstring help;
    int argc;
    char **argv;

}ArgumentParser;

ArgumentParser *Argparse_new_argument_parser(bstring description, int argc, char **argv);
int Argparse_add_argument(bstring token,bstring name,bstring help);
int Argparse_parse();

#endif // ARGPARSE_H_
