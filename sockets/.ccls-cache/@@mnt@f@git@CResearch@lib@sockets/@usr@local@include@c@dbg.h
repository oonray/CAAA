//
// Created by alexa on 24/01/2021.
//

#ifndef BLACKFORGE_DBG_H
#define BLACKFORGE_DBG_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define get_errno() (errno == 0 ? "None" : strerror(errno))

#define log_msg(T, M, ...) printf("\e[35m[" T "]\e[0m [file: %s | func %s | line: %d]: " M "\n",__FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_err(T, M, ...) printf("\e[31m[" T "]\e[0m [file: %s | func %s | line: %d] [errno %s]: " M "\n",__FILE__, __func__, __LINE__, get_errno() , ##__VA_ARGS__)

#define log_info(M,...)  log_msg("INFO",M, ##__VA_ARGS__)
#define log_error(M,...){ log_err("ERROR",M,##__VA_ARGS__); }
#define check(A, M, ...){ if(!(A)){ log_error(M, ##__VA_ARGS__); errno=0; goto error; }}
#define check_memory(A) check((A),"Memory Error")

#endif //BLACKFORGE_DBG_H
