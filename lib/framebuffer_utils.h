#ifndef __WIN32
#ifndef __FRAMEBUFFER_UTILS_H__
#define __FRAMEBUFFER_UTILS_H__

#include <linux/fb.h>

class Framebuffer {
private:
	bool is_bind = false;
	int fbfd;
	fb_var_screeninfo vinfo;
	fb_fix_screeninfo finfo;
	unsigned char *data;

	int width, height, bytes_per_pixel;

	int error_code;
public:
	int r, g, b, a;
	~Framebuffer();
	bool bind(const char *device_name);
	bool unbind();
	bool set(int x, int y, int r, int g, int b, int a);
	bool fill(int r, int g, int b, int a);
	bool get(int x, int y);
	void get_size(int *width, int *height);

	const char *get_error_message();
};

#endif
#endif
