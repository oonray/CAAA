#include "argparse.h"


ArgumentParser *Argparse_new_argument_parser(bstring description, int argc, char **argv){
  ArgumentParser *args = (ArgumentParser *)calloc(1,sizeof(ArgumentParser));
  check(args!=NULL,"Could not Create ArgumentParser");
  args->args = Map_New(NULL,NULL);

  args->argc = argc;
  args->argv = argv;

  args->description = description;
  args->help = bformat("%s\n%s\n",bdata(description), argv[0]);

  return args;
error:
  return NULL;
}
