#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef INCLUDE_ARGUMENT
#include "argument_utils.h"
#endif

#ifdef INCLUDE_FRAMEBUFFER
#include "framebuffer_utils.h"
#endif

void log_error(const char *description, ...);
int parse_int(const char *str, bool *success = nullptr);

#ifndef __WIN32
#include <termios.h>
char getch();
#endif

#endif
