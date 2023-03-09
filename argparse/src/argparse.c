#include "argparse.h"

void PrintArgs(void *value, void *data) {
  Argument *arg = (Argument *)value;
  printf("\t%s%s%s\t%s\t\t%s[default] %s%s\t\t%s\n\r", KGRN, bdata(arg->token),
         KNRM, bdata(arg->name), KMAG, KNRM, bdata(arg->def), bdata(arg->help));
}

ArgumentParser *Argparse_New_Argument_Parser(bstring description,
                                             bstring name) {
  ArgumentParser *args = (ArgumentParser *)calloc(1, sizeof(ArgumentParser));
  check(args != NULL, "Could not Create ArgumentParser");

  args->description = description;
  args->progname = name;

  Argparse_Add_Bool(args, "-h", "help", "false", "print help");

  return args;
error:
  return NULL;
}

Argument *Argument_New(bstring type, bstring token, bstring name, bstring def,
                       bstring help) {
  Argument *arg = (Argument *)calloc(1, sizeof(Argument));
  check(arg != NULL, "Could not create Argument");

  arg->type = type;
  arg->token = token;
  arg->name = name;
  arg->def = def;
  arg->help = help;

  return arg;
error:
  return NULL;
}

void Argparse_Destroy(ArgumentParser *parser) {
  TriTree_Destroy(parser->args_t);
  TriTree_Destroy(parser->args_n);
  bdestroy(parser->description);
  free(parser);
}

void Argparse_Argument_Destroy(Argument *arg) {
  bdestroy(arg->token);
  bdestroy(arg->name);
  bdestroy(arg->help);

  free(arg->value);
  free(arg);
}

int Argparse_Add_Argument(ArgumentParser *parser, Argument *arg) {
  parser->args_t = TriTree_Insert(parser->args_t, bdata(arg->token),
                                  blength(arg->token), arg);
  parser->args_n =
      TriTree_Insert(parser->args_n, bdata(arg->name), blength(arg->name), arg);

  check(parser->args_t != NULL, "Could not add item");
  check(parser->args_n != NULL, "Could not add item");
  return 0;
error:
  return 1;
}

int Argparse_Add_Int(ArgumentParser *parser, char *token, char *name, char *def,
                     char *help) {
  Argument *arg = (Argument *)Argument_New(bfromcstr("int"), bfromcstr(token),
                                           bfromcstr(name), bfromcstr(def),
                                           bfromcstr(help));
  check(arg != NULL, "Could not create Argument");
  Argparse_Add_Argument(parser, arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_String(ArgumentParser *parser, char *token, char *name,
                        char *def, char *help) {
  Argument *arg = (Argument *)Argument_New(bfromcstr("string"),
                                           bfromcstr(token), bfromcstr(name),
                                           bfromcstr(def), bfromcstr(help));
  check(arg != NULL, "Could not create Argument");
  Argparse_Add_Argument(parser, arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_Bool(ArgumentParser *parser, char *token, char *name,
                      char *def, char *help) {
  Argument *arg = (Argument *)Argument_New(bfromcstr("bool"), bfromcstr(token),
                                           bfromcstr(name), bfromcstr(def),
                                           bfromcstr(help));
  check(arg != NULL, "Could not create Argument");
  Argparse_Add_Argument(parser, arg);
  return 1;
error:
  return 0;
}

int Argparse_Parse(ArgumentParser *parser, int argc, char *argv[]) {
  check(argc > 0, "Argc too small");
  check(argv != NULL, "No argv");

  parser->progname = bfromcstr(argv[0]);

  for (int i = 1; i < argc; i++) {
    bstring token = bfromcstr(argv[i]);
    bstring value = bfromcstr(argv[i + 1]);

    log_info("[token]%s:[value]%s", bdata(token), bdata(value));

    if (token->data[0] != '-') {
      // token not folaf format
      log_info("Continue");
      continue;
    }

    if (bstrcmp(token, bfromcstr("-h")) == 0) {
      Argparse_Print_Help(parser);
      return 1;
    }

    Argument *arg_t = Argparse_Find(parser, token);
    if (arg_t == NULL) {
      continue;
    }

    int is_bool = bstrcmp(arg_t->type, bfromcstr("bool"));
    if (is_bool != 0) {
      arg_t->value = value;
      i++;
    }
    arg_t->value = bfromcstr("true");
  }
  return 0;
error:
  return 1;
}

Argument *Argparse_Get(ArgumentParser *args, bstring name) {
  return TriTree_Search(args->args_n, bdata(name), blength(name));
}
Argument *Argparse_Find(ArgumentParser *args, bstring token) {
  return TriTree_Search(args->args_t, bdata(token), blength(token));
}

void Argparse_Print_Help(ArgumentParser *args) {
  printf("\n%s %s %s %s [flag] %s [pos_args]%s:\n", KWAR, KGRN,
         bdata(args->progname), KYEL, KBLU, KNRM);
  TriTree_Traverse(args->args_t, PrintArgs, NULL);
  printf("\n");
}
