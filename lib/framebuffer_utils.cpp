#ifndef __WIN32
#include "framebuffer_utils.h"

#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


const char *ERROR_MESSAGES[] = {
	"ERROR BIND TWICE",
	"ERROR OPEN DEVICE",
	"ERROR IOCTL",
	"ERROR MMAP",
	"ERROR UNBIND WITHOUT BINDING",
	"ERROR COORD OUT OF BOUND",
	"ERROR COLOR OUT OF BOUND"
};

const int ERROR_BIND_TWICE = 0;
const int ERROR_OPEN_DEVICE = 1;
const int ERROR_IOCTL = 2;
const int ERROR_MMAP = 3;
const int ERROR_UNBIND_WITHOUT_BINDING = 4;
const int ERROR_COORD_OUT_OF_BOUND = 5;
const int ERROR_COLOR_OUT_OF_BOUND = 6;

Framebuffer::~Framebuffer() {
	if(is_bind) {
		unbind();
	}
}

bool Framebuffer::bind(const char *device_name) {
	if(is_bind) {
		error_code = ERROR_BIND_TWICE;
		return false;
	}
	fbfd = open(device_name, O_RDWR);
	if(fbfd == -1) {
		error_code = ERROR_OPEN_DEVICE;
		is_bind = false;
		return false;
	}
	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		error_code = ERROR_IOCTL;
		is_bind = false;
		return false;
	}
	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		error_code = ERROR_IOCTL;
		is_bind = false;
		return false;
	}
	data = (unsigned char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if(data == MAP_FAILED) {
		error_code = ERROR_MMAP;
		is_bind = false;
		return false;
	}

	width = vinfo.xres;
	height = vinfo.yres;
	bytes_per_pixel = vinfo.bits_per_pixel / 8;

	buffer = new unsigned char[width * height * bytes_per_pixel];
	//memset(buffer, 0, sizeof(unsigned char) * width * height * bytes_per_pixel);
	reset_buffer();

	is_bind = true;
	return true;
}

bool Framebuffer::unbind() {
	if(!is_bind) {
		error_code = ERROR_UNBIND_WITHOUT_BINDING;
		return false;
	}
	munmap(data, finfo.smem_len);
	close(fbfd);
	if(buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	is_bind = false;
	return true;
}

bool Framebuffer::get_is_bind() {
	return is_bind;
}

void Framebuffer::set_if_blend(bool if_blend) {
	this->if_blend = if_blend;
}

bool Framebuffer::get_if_blend() {
	return if_blend;
}

bool Framebuffer::set(Coord pos, Color c) {
	if(pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
		error_code = ERROR_COORD_OUT_OF_BOUND;
		return false;
	}
	if(c.r > 255 || c.r < 0 || c.g > 255 || c.g < 0 || c.b > 255 || c.b < 0 || c.a > 255 || c.a < 0) {
		error_code = ERROR_COLOR_OUT_OF_BOUND;
		return false;
	}
	unsigned int value;
	if(if_blend && c.a != 255) {
		Color prev;
		get({pos.x, pos.y}, &prev);
		double source = c.a / 255.0;
		double dest = 1 - source;
		value = (255 << 24) +
			((int)(c.r * source + prev.r * dest) << 16) +
			((int)(c.g * source + prev.g * dest) << 8) +
			(int)(c.b * source + prev.b * dest);
	} else {
		value = (c.a << 24) + (c.r << 16) + (c.g << 8) + c.b;
	}
	*(unsigned int *)(buffer + pos.x * bytes_per_pixel + pos.y * width * bytes_per_pixel) = value;
	return true;
}

bool Framebuffer::fill(Color c) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(!set({x, y}, {c.r, c.g, c.b ,c.a})) {
				return false;
			}
		}
	}
	return true;
}
/*
void Framebuffer::draw_line(int x1, int y1, int x2, int y2) {
	int smaller_x = x1 < x2 ? x1 : x2;
	int bigger_x = x1 > x2 ? x1 : x2;
	int smaller_y = y1 < y2 ? y1 : y2;
	int bigger_y = y1 > y2 ? y1 : y2;
	for(int x = smaller_x, y = sma; x <= bigger_x; x++) {
		
	}
}*/

void Framebuffer::draw_rectangle(Coord pos, int width, int height, Color c) {
	for(int x = 0; x < width; x++) {
		set({pos.x + x, pos.y}, c);
		set({pos.x + x, pos.y + height - 1}, c);
	}
	for(int y = 0; y < height; y++) {
		set({pos.x, pos.y + y}, c);
		set({pos.x + width - 1, pos.y + y}, c);
	}
}
void Framebuffer::fill_rectangle(Coord pos, int width, int height, Color c) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			set({pos.x + x, pos.y + y}, c);
		}
	}
}

bool Framebuffer::get(Coord pos, Color *output) {
	if(pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
		error_code = ERROR_COORD_OUT_OF_BOUND;
		return false;
	}
	unsigned int value = *(unsigned int *)(buffer + pos.x * bytes_per_pixel + pos.y * width * bytes_per_pixel);
	output->r = (value & 0x00ff0000) >> 16;
	output->g = (value & 0x0000ff00) >> 8;
	output->b = value & 0x000000ff;
	output->a = value >> 24;
	return true;
}

void Framebuffer::get_size(int *width, int *height) {
	*width = this->width;
	*height = this->height;
}

void Framebuffer::update() {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			unsigned int value = *(unsigned int*)(buffer + x * bytes_per_pixel + y * width * bytes_per_pixel);
			*(unsigned int *)(data + x * bytes_per_pixel + y * finfo.line_length) = value;
		}
	}
}

void Framebuffer::reset_buffer() {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			unsigned int value = *(unsigned int *)(data + x * bytes_per_pixel + y * finfo.line_length);
			*(unsigned int*)(buffer + x * bytes_per_pixel + y * width * bytes_per_pixel) = value;
		}
	}
}

const char *Framebuffer::get_error_message() {
	return ERROR_MESSAGES[error_code];
}

#endif
