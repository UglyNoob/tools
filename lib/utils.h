#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef INCLUDE_ARGUMENT
#include "argument_utils.h"
#endif

#ifdef INCLUDE_FRAMEBUFFER
#include "framebuffer_utils.h"
#endif

void log(const char *description, ...);
void log_error(const char *description, ...);
int parse_int(const char *str, bool *success = nullptr);

char32_t utf8to32(const unsigned char *src, int *output_length = nullptr);
char32_t utf8to32(const char *src, int *output_length = nullptr);

#ifndef __WIN32
#include <termios.h>
char getch();
#endif

#endif
