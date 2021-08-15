#ifndef FILE_CLEARENV_H
#define FILE_CLEARENV_H
#include <string.h>
#include <stdlib.h>
#include "config.h"

#if !HAVE_CLEARENV

extern char** environ;
#define clearenv() environ = NULL

#endif

#endif /* FILE_CLEARENV_H */
