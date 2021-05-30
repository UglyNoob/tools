#ifndef __UTILS_H__
#define __UTILS_H__

#include "argument_utils.h"
#include "framebuffer_utils.h"

void log_error(const char *description, ...);
int parse_int(const char *str, bool *success = nullptr);

#ifndef __WIN32
#include <termios.h>
char getch();
#endif

#endif
