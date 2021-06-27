#ifndef __WIN32
#ifndef __FRAMEBUFFER_UTILS_H__
#define __FRAMEBUFFER_UTILS_H__

#include <linux/fb.h>

struct Coord {
	int x = 0, y = 0;
};

struct Color {
	unsigned char r = 0.f, g = 0.f, b = 0.f, a = 255.f;
};

class Framebuffer {
private:
	bool is_bind = false, if_blend = false;
	int fbfd;
	fb_var_screeninfo vinfo;
	fb_fix_screeninfo finfo;
	unsigned char *data, *buffer;

	int width, height, bytes_per_pixel;

	int error_code;
public:
	~Framebuffer();
	bool bind(const char *device_name);
	bool unbind();
	bool get_is_bind();
	void set_if_blend(bool if_blend);
	bool get_if_blend();
	void get_size(int *width, int *height);
	bool get(Coord pos, Color *output);

	void update();
	void reset_buffer();

	bool set(Coord pos, Color c);
	bool fill(Color c);
	//void draw_line(int x1, int y1, int x2, int y2);
	void draw_rectangle(Coord pos, int width, int height, Color c);
	void fill_rectangle(Coord pos, int width, int height, Color c);

	const char *get_error_message();
};

#endif
#endif
