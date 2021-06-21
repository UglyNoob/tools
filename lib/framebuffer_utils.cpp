#ifndef __WIN32
#include "framebuffer_utils.h"

#include <cstdio>

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
	is_bind = false;
	return true;
}

bool Framebuffer::get_is_bind() {
	return is_bind;
}

bool Framebuffer::set(int x, int y, int r, int g, int b, int a) {
	if(x < 0 || y < 0 || x >= width || y >= height) {
		error_code = ERROR_COORD_OUT_OF_BOUND;
		return false;
	}
	if(r > 255 || r < 0 || g > 255 || g < 0 || b > 255 || b < 0 || a > 255 || a < 0) {
		error_code = ERROR_COLOR_OUT_OF_BOUND;
		return false;
	}
	unsigned int value;
	if(if_blend && a != 255) {
		int pr, pg, pb;
		get(x, y, &pr, &pg, &pb, nullptr);
		double percent = a / 255.f;
		value = (255 << 24) + 
			((int)(r * percent + pr * (1 - percent)) << 16) +
			((int)(g * percent + pg * (1 - percent)) << 8) +
			(int)(b * percent + pb * (1 - percent));
	} else {
		value = (a << 24) + (r << 16) + (g << 8) + b;
	}
	*(unsigned int *)(data + x * bytes_per_pixel + y * finfo.line_length) = value;
	return true;
}

bool Framebuffer::fill(int r, int g, int b, int a) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(!set(x, y, r, g, b ,a)) {
				return false;
			}
		}
	}
	return true;
}

bool Framebuffer::get(int x, int y, int *r, int *g, int *b, int *a) {
	if(x < 0 || y < 0 || x >= width || y >= height) {
		error_code = ERROR_COORD_OUT_OF_BOUND;
		return false;
	}
	unsigned int value = *(unsigned int *)(data + x * bytes_per_pixel + y * finfo.line_length);
	if(r != nullptr) *r = (value & 0x00ff0000) >> 16;
	if(g != nullptr) *g = (value & 0x0000ff00) >> 8;
	if(b != nullptr) *b = value & 0x000000ff;
	if(a != nullptr) *a = value >> 24;
	return true;
}

void Framebuffer::get_size(int *width, int *height) {
	*width = this->width;
	*height = this->height;
}

void Framebuffer::set_if_blend(bool if_blend) {
	this->if_blend = if_blend;
}
const char *Framebuffer::get_error_message() {
	return ERROR_MESSAGES[error_code];
}

#endif
