#ifndef CA_LOGGING_H_
#define CA_LOGGING_H_

#include <syslog.h>

#define open_log_default() open_log(argv[0], LOG_PERROR | LOG_CONS | LOG_PID, 0)
#define open_log(N, O, P) openlog(N, O, P)
#define close_log() closelog()

#define DBG_FORMAT(M, P) "[file: %s | func %s | line %d] [" P "] " M "\n"

#define dbg_syslog(S, P, M, ...)                                               \
  syslog(S, DBG_FORMAT(M, P), __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define log_info(M, ...)                                                       \
  { dbg_syslog(LOG_INFO, "NULL", M, ##__VA_ARGS__); }

#define log_error(M, ...)                                                      \
  {                                                                            \
    dbg_syslog(LOG_ERR, "NULL", "errno | %d | %s", M, errno, dbg_get_errno(),  \
               ##__VA_ARGS__);

#define check(A, M, ...)                                                       \
  {                                                                            \
    if (!(A)) {                                                                \
      log_error(M, ##__VA_ARGS__);                                             \
      errno = 0;                                                               \
      goto error;                                                              \
    }                                                                          \
  }

#define check_memory(A) check((A), "Memory Error")
#endif // CA_LOGGING_H_
