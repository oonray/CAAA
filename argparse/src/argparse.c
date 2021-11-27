#include "argparse.h"

static const struct tagbstring format_str_help =
    bsStatic("%s\t%s\t%s\t%s[default]%s\n");

void PrintArgs(void *value, void *data) {
  Argument *arg = (Argument *)value;
  printf("\n\t%s\t%s\t[default]%s\t%s\t\n", bdata(arg->token), bdata(arg->name),
         bdata(arg->def), bdata(arg->help));
}

ArgumentParser *Argparse_New_Argument_Parser(const bstring description) {
  ArgumentParser *args = (ArgumentParser *)calloc(1, sizeof(ArgumentParser));
  check(args != NULL, "Could not Create ArgumentParser");

  args->description = description;

  Argparse_Add_Bool(args, "-h", "help", "false", "print help");

  return args;
error:
  return NULL;
}

Argument *Argument_New(const bstring type, const bstring token,
                       const bstring name, const bstring def,
                       const bstring help) {
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
  parser->args_n = TriTree_Insert(parser->args_n, bdata(arg->name),
                                  blength(arg->token), arg);
  check(parser->args_t != NULL, "Could not add item");
  check(parser->args_n != NULL, "Could not add item");
  return 0;
error:
  return 1;
}

int Argparse_Add_Int(ArgumentParser *parser, const char *token,
                     const char *name, const char *def, const char *help) {
  Argument *arg = (Argument *)Argument_New(bfromcstr("int"), bfromcstr(token),
                                           bfromcstr(name), bfromcstr(def),
                                           bfromcstr(help));
  check(arg != NULL, "Could not create Argument");
  Argparse_Add_Argument(parser, arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_String(ArgumentParser *parser, const char *token,
                        const char *name, const char *def, const char *help) {
  Argument *arg = (Argument *)Argument_New(bfromcstr("string"),
                                           bfromcstr(token), bfromcstr(name),
                                           bfromcstr(def), bfromcstr(help));
  check(arg != NULL, "Could not create Argument");
  Argparse_Add_Argument(parser, arg);
  return 1;
error:
  return 0;
}

int Argparse_Add_Bool(ArgumentParser *parser, const char *token,
                      const char *name, const char *def, const char *help) {
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
  for (int i = 0; i < argc; i++) {
    bstring token = bfromcstr(argv[i]);
    if (token->data[0] == '-') {
      if (bstrcmp(token, bfromcstr("-h")) == 0) {
        Argparse_Print_Help(parser);
      }

      Argument *arg = Argparse_Find(parser->args_t, token);
      if (arg != NULL) {
        if (arg->type != "bool") {
          ++i;
          arg->value = bfromcstr(argv[i]);
        } else {
          arg->value = bfromcstr("true");
        }
      }
    }
  }
  return 1;
}

Argument *Argparse_Get(ArgumentParser *args, bstring name) {
  return TriTree_Search(args->args_n, bdata(name), blength(name));
}
Argument *Argparse_Find(ArgumentParser *args, bstring token) {
  return TriTree_Search(args->args_t, bdata(token), blength(token));
}

void Argparse_Print_Help(ArgumentParser *args) {
  TriTree_Traverse(args->args_t, PrintArgs, NULL);
  Argparse_Destroy(args);
  exit(0);
}
