#include "argparse.h"

inline const struct tagbstring format_str_help = bsStatic("%s%s%s\t%s%s%s\t%s%s%s\t%s[default]%s%s\n");

ArgumentParser *Argparse_New_Argument_Parser(const bstring description,const int argc,const char *argv[]){
  ArgumentParser *args = (ArgumentParser *)calloc(1,sizeof(ArgumentParser));
  check(args!=NULL,"Could not Create ArgumentParser");

  args->args = Vector_New(sizeof(bstring),argc);
  args->args_v = Vector_New(sizeof(bstring),argc);

  for(int i=0;i<argc;++i){
    Vector_Push(args->args_v, bfromcstr(argv[i]));
  }

  args->description = description;

  Argparse_Add_Bool(args,bfromcstr("-h"),bfromcstr("help"),bfromcstr("false"),bfromcstr("print help"));

  return args;
error:
  return NULL;
}

Argument *Argument_New(const bstring type,const bstring token,const bstring name,const bstring def,const bstring help){
  Argument *arg = (Argument *)calloc(1,sizeof(Argument));
  check(arg!=NULL,"Could not create Argument");

  arg->type=type;
  arg->token=token;
  arg->name=name;
  arg->def=def;
  arg->help=help;

  return arg;
error:
  return NULL;
}

void Argparse_Destroy(ArgumentParser *parser){
  Vector_Destroy(parser->args);
  Vector_Destroy(parser->args_v);
  bdestroy(parser->description);
  free(parser);
}

void Argparse_Argument_Destroy(Argument *arg){
  bdestroy(arg->token);
  bdestroy(arg->name);
  bdestroy(arg->help);
  free(arg->value);
  free(arg);
}

int Argparse_Add_Argument(ArgumentParser *parser,Argument *arg){
  int rc = Vector_Push(parser->args,arg);
  check(rc==0,"Could not push");
  return 0;
error:
  return 1;
}

int Argparse_Add_Int(ArgumentParser *parser,bstring token,bstring name,bstring def,bstring help){
  Argument *arg = (Argument *)Argument_New(bfromcstr("int"),token,name,def,help);
  check(arg!=NULL,"Could not create Argument");
  Argparse_Add_Argument(parser,arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_String(ArgumentParser *parser,bstring token,bstring name,bstring def,bstring help){
  Argument *arg = Argument_New(bfromcstr("string"),token,name,def,help);
  check(arg!=NULL,"Could not create Argument");
  Argparse_Add_Argument(parser,arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_Bool(ArgumentParser *parser,bstring token,bstring name,bstring def,bstring help){
  Argument *arg = Argument_New(bfromcstr("bool"),token,name,def,help);
  check(arg!=NULL,"Could not create Argument");
  log_info("Adding %s",bdata(token));
  Argparse_Add_Argument(parser,arg);
  return 1;
error:
  return 0;
}

int Argparse_Parse(ArgumentParser *parser){
  int len = Vector_End(parser->args_v);
  for(int i = 0;i<len;++i){
    bstring value = (bstring)Vector_Get(parser->args_v,i);
    if(value->data[0]=='-'){
      log_info("value %s",bdata(value));
      for(int j=0;j<Vector_End(parser->args);j++){
        Argument *arg = (Argument *)Vector_Get(parser->args,j);
        if(bstrcmp(value,arg->token)==0){
          if(bstrcmp(arg->type,bfromcstr("bool")!=0)){
            arg->value = (bstring)Vector_Get(parser->args_v,i+1);
            i++;
          }else{
            arg->value=bfromcstr("true");
          }
        }
      }
    }
  }
  return 1;
error:
  return 0;
}

void *Argparse_Get(ArgumentParser *args,bstring name){
}

void Argparse_Print_Help(ArgumentParser *args){
  printf("\nUsage:\t%s\nDescription:\t%s\n",bdata((bstring)Vector_Get(args->args_v,0)),bdata(args->description));
  int len = Vector_End(args->args_v);
  for(int i=1;i<len;i++){
    Argument *arg = (Argument *)Vector_Get(args->args,i);
    bstring line = bformat(format_str_help.data,
                    bfromcstr(KYEL),arg->token,bfromcstr(KNRM),
                    bfromcstr(KWHT),arg->name, bfromcstr(KNRM),
                    bfromcstr(KBLU),arg->type, bfromcstr(KNRM),
                    bfromcstr(KMAG),arg->def,  bfromcstr(KNRM));
    printf("%s",bdata(line));
  }
}
