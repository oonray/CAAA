#include "argparse.h"


ArgumentParser *Argparse_New_Argument_Parser(const bstring description,const int argc,const char *argv[]){
  ArgumentParser *args = (ArgumentParser *)calloc(1,sizeof(ArgumentParser));
  check(args!=NULL,"Could not Create ArgumentParser");
  args->args_m = Map_New(NULL,NULL);
  args->args_v = Vector_New(sizeof(bstring),argc);
  args->args_s = Vector_New(sizeof(Argument),10);

  for(int i=0;i<argc;++i){
    Vector_Push(args->args_v, bfromcstr(argv[i]) );
  }

  args->description = description;
  args->help = bformat("%s\n%s\n",bdata(description), bdata((bstring)Vector_Get(args->args_v,0)));

  return args;
error:
  return NULL;
}

Argument *Argument_New(const bstring token,const bstring name,const bstring help){
  Argument *arg = (Argument *)calloc(1,sizeof(Argument));
  check(arg!=NULL,"Could not create Argument");

  arg->token=token;
  arg->name=name;
  arg->help=help;

  return arg;
error:
  return NULL;
}

void Argparse_Destroy(ArgumentParser *parser){
  Map_Destroy(parser->args_m);
  Vectory_Destroy(parse->args_v);
  Vectory_Destroy(parse->args_s);
  bdestroy(args->help);
  bdestroy(args->description);
  free(parser);
}

void Argparse_Argyment_Destroy(Argument *arg){
  bdestroy(arg->token);
  bdestroy(arg->name);
  bdestroy(arg->help);
  free(arg->value);
  free(arg);
}

int Argparse_Add_Argument(ArgumentParser *parser,bstring token,bstring name,bstring help){

}
