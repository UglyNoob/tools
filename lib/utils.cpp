#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#ifdef __WIN32
#include <windows.h>
#endif

void log_error(const char *description, ...) {
	va_list args;
	va_start(args, description);
#ifdef __WIN32
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	WORD general = info.wAttributes;
	SetConsoleTextAttribute(handle, FOREGROUND_RED);
#else
	fprintf(stderr, "\033[91m\033[1m");
#endif
	vfprintf(stderr, description, args);
#ifdef __WIN32
	SetConsoleTextAttribute(handle, general);
	putchar('\n');
#else
	fprintf(stderr, "\033[0m\n");
#endif
	va_end(args);
}

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

#include "framebuffer_utils.cpp"
#include "argument_utils.cpp"
