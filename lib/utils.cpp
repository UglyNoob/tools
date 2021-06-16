#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#ifdef __WIN32
#include <windows.h>
void log_error(const char *description, ...) {
	va_list args;
	va_start(args, description);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	WORD general = info.wAttributes;
	SetConsoleTextAttribute(handle, FOREGROUND_RED);
	vfprintf(stderr, description, args);
	SetConsoleTextAttribute(handle, general);
	putchar('\n');
	va_end(args);
}
#else
void log_error(const char *description, ...) {
	va_list args;
	va_start(args, description);
	fprintf(stderr, "\033[91m\033[1m");
	vfprintf(stderr, description, args);
	fprintf(stderr, "\033[0m\n");
	va_end(args);
}
#endif


int parse_int(const char *str, bool *success) {
	if(str[0] == '\0') {
		if(success != nullptr) {
			*success = false;
		}
		return 0;
	}
	char *p = (char *)str;
	int result = 0;
	bool minus = false;
	if(str[0] == '-'){
		minus = true;
		p++;
	}
	while(*p) {
		if(('0' > *p) || ('9' < *p)) {
			if(success != nullptr) {
				*success = false;
			}
			return 0;
		}
		result += *p - '0';
		result *= 10;
		p++;
	}
	result /= 10;
	if(minus) {
		result = -result;
	}
	if(success != nullptr) {
		*success = true;
	}
	return result;
}

#ifndef __WIN32
#include <termios.h>

char getch() {
	termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ECHO;
	newt.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &newt);
	char value = getchar();
	tcsetattr(0, TCSANOW, &oldt);
	return value;
}
#endif

#ifdef INCLUDE_FRAMEBUFFER
#include "framebuffer_utils.cpp"
#endif

#ifdef INCLUDE_ARGUMENT
#include "argument_utils.cpp"
#endif
