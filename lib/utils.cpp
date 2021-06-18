#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

void log(const char *description, ...) {
	va_list args;
	va_start(args, description);
	vfprintf(stderr, description, args);
	fprintf(stderr, "\n");
	va_end(args);
}

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
	fprintf(stderr, "\n");
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

wchar_t utf8to32(const unsigned char *src, int *output_length) {
	int length = 0;
	wchar_t result = L'\0';
	if((src[0] & 0x80) == 0x0) {
		length = 1;
		result = src[0];
	} else if((src[0] & 0xe0) == 0xc0) {
		length = 2;
		result = ((src[0] & 0x1f) << 6) + (src[1] & 0x3f);
	} else if((src[0] & 0xf0) == 0xe0) {
		length = 3;
		result = ((src[0] & 0x0f) << 12) + ((src[1] & 0x3f) << 6) + (src[2] & 0x3f);
	} else if((src[0] & 0xf8) == 0xf0) {
		length = 4;
		result = ((src[0] & 0x07) << 18) + ((src[1] & 0x3f) << 12) + ((src[2] & 0x3f) << 6) + (src[3] & 0x3f);
	}

	if(output_length != nullptr) {
		*output_length = length;
	}

	return result;
}

wchar_t utf8to32(const char *src, int *output_length) {
	return utf8to32((const unsigned char *)src, output_length);
}




#ifdef INCLUDE_FRAMEBUFFER
#include "framebuffer_utils.cpp"
#endif

#ifdef INCLUDE_ARGUMENT
#include "argument_utils.cpp"
#endif
