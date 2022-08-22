#ifndef ASYNCDBG_H_
#define ASYNCDBG_H_

#include "colors.h"
#include "dbg.h"

// checks and does noting
#define check_none(A, M, ...)                                                  \
  {                                                                            \
    if (!(A)) {                                                                \
      log_error(M, ##__VA_ARGS__);                                             \
      errno = 0;                                                               \
    }                                                                          \
  }

// checks contines
#define check_continue(A, M, ...)                                              \
  {                                                                            \
    if (!(A)) {                                                                \
      log_error(M, ##__VA_ARGS__);                                             \
      errno = 0;                                                               \
      continue;                                                                \
    }                                                                          \
  }

#define check_break(A, M, ...)                                                 \
  {                                                                            \
    if (!(A)) {                                                                \
      log_error(M, ##__VA_ARGS__);                                             \
      errno = 0;                                                               \
      break;                                                                   \
    }                                                                          \
  }

#endif // ASYNCDBG_H_
