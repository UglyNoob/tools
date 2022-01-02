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

#include <string>
#include <typeinfo>
std::string demangle(const std::type_info &info);

#ifdef __WIN32
#include <conio.h>
template<typename T>
int getch(const T &) {return getch();}
#else
#include <termios.h>
#include <unistd.h>
template<typename Callable_t>
char getch(const Callable_t &charget) {
	termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ECHO;
	newt.c_lflag &= ~ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	char value = charget();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return value;
}
inline char getch() {
	return getch([]{return getchar();});
}
bool kbhit();
#endif

#endif
