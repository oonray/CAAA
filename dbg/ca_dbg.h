// Created by alexa on 24/01/2021.

#ifndef _DBG_H
#define _DBG_H

#include "colors.h"
#include <bstring/bstrlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

// COLOR [STATUS] END [file | func | line] MESSAGE \n

#define OUT_STD (stdout)
#define OUT_ERR (stderr)

#define DBG_FORMAT(SS, C, S, M, P)                                             \
  "" SS " " C "[" S "]" KNRM " [file: %s | func %s | line %d] [" P "] " M "\n"

#define dbg_get_errno() (errno == 0 ? "None" : strerror(errno))

#define dbg_log(O, SS, C, S, P, M, ...)                                        \
  fprintf(O, DBG_FORMAT(SS, C, S, M, P), __FILE__, __func__, __LINE__,         \
          ##__VA_ARGS__);

#define log_info(M, ...)                                                       \
  { dbg_log(OUT_STD, KNOK, KMAG, "INFO", "NULL", M, ##__VA_ARGS__); }

#define log_error(M, ...)                                                      \
  {                                                                            \
    dbg_log(OUT_ERR, KWAR, KRED, "ERROR", "errno | %d | %s", M, errno,         \
            dbg_get_errno(), ##__VA_ARGS__);                                   \
  }

#define check(A, M, ...)                                                       \
  {                                                                            \
    if (!(A)) {                                                                \
      log_error(M, ##__VA_ARGS__);                                             \
      errno = 0;                                                               \
      goto error;                                                              \
    }                                                                          \
  }

#define check_memory(A) check((A), "Memory Error")

#endif //_DBG_H
