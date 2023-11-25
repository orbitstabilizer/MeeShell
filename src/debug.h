#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG
#define DEBUG 0
#endif

#include <stdio.h>
#include <stdlib.h>
#define UNIMPLEMENTED(func_name)                                               \
  do {                                                                         \
    if (DEBUG) {                                                               \
      frintf("INFO: '%s' is not implemented in file: %s line: %d \n",          \
             func_name, __FILE__, __LINE__);                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define TODO(action)                                                           \
  do {                                                                         \
    if (DEBUG) {                                                               \
      printf("TODO:  '%s' in file: %s line: %d \n", action, __FILE__,          \
             __LINE__);                                                        \
    }                                                                          \
  } while (0)
#define LOG_ERROR(...)                                                         \
  do {                                                                         \
    if (DEBUG) {                                                               \
      fprintf(stderr, "ERROR:");                                               \
      fprintf(stderr, __VA_ARGS__);                                            \
    }                                                                          \
  } while (0)

#define LOG_INFO(...)                                                         \
  do {                                                                         \
    if (DEBUG) {                                                               \
      fprintf(stderr, __VA_ARGS__);                                            \
    }                                                                          \
  } while (0)

#endif
