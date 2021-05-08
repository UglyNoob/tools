#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef WIN32
#include <conio.h>
#else
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


const int CMDLINE_COUNT = 4;
const int DEFAULT_MAP_WIDTH = 10;
const int DEFAULT_MAP_HEIGHT = 15;
const int MINIMUM_MAP_WIDTH = 10;
const int MINIMUM_MAP_HEIGHT = 10;
const int BUFFER_HEIGHT = 4;
const int CONTROLLED_BLOCK_COUNT = 4;
const int DEFAULT_DELAY_TIME = 500;

const char MAP_EMPTY = 0;

const char MAP_BLOCK = 100;//WHITE

const char MAP_BLOCK_L = 101;//RED
const char MAP_BLOCK_LONG = 102;//GREEN
const char MAP_BLOCK_SQUARE = 103;//ORANGE
const char MAP_BLOCK_STEP = 104;//BLUE
const char MAP_BLOCK_CONVEX = 105;//PURPLE
const char MAP_BLOCK_L_FLIP = 106;//CYAN
const char MAP_BLOCK_STEP_FLIP = 107;//YELLOW

const char MAP_BLOCK_END = 108;

inline void delay(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
inline void yield() {
	std::this_thread::yield();
}
void log_error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\033[1;91m");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\033[0m\n");
	va_end(args);
}
void end(int code) {
	exit(code);
}

struct SquareArray {
	char *data;
	int width, height;
};
struct Pos {
	int x, y;
};
Pos controlled_block[CONTROLLED_BLOCK_COUNT];
struct Shape {
	const int count;
	const SquareArray *datas;
	const Pos *rotate_pos;
};
const Shape shapes[] = {//IN ORDER
	{4, new SquareArray[4]{{new char[6] {//MAP_BLOCK_L
		1, 0,
		1, 0,
		1, 1
	}, 2, 3}, {new char[6]{
		1, 1, 1,
		1, 0, 0
	}, 3, 2}, {new char[6]{
		1, 1,
		0, 1,
		0, 1
	}, 2, 3}, {new char[6]{
		0, 0, 1,
		1, 1, 1
	}, 3, 2}},
	new Pos[4] {
		{-1, 1},
		{0, -1},
		{0, 0},
		{1, 0}
	}},
	{2, new SquareArray[2]{{new char[4] {//MAP_BLOCK_LONG
		1,
		1,
		1,
		1
	}, 1, 4},{new char[4]{
		1, 1, 1, 1
	}, 4, 1}},
	new Pos[2]{
		{-2, 2},
		{2, -2}
	}},
	{1, new SquareArray[1]{{new char[4] {//MAP_BLOCK_SQUARE
		1, 1,
		1, 1
	}, 2, 2}}},
	{2, new SquareArray[2]{{new char[6] {//MAP_BLOCK_STEP
		1, 0,
		1, 1,
		0, 1
	}, 2, 3}, {new char[6] {
		0, 1, 1,
		1, 1, 0
	}, 3, 2}}, new Pos[2]{
		{-1, 1},
		{1, -1}
	}},
	{4, new SquareArray[4]{{new char[6] {//MAP_BLOCK_CONVEX
		0, 1, 0,
		1, 1, 1
	}, 3, 2},{new char[6] {
		1, 0,
		1, 1,
		1, 0
	}, 2, 3}, {new char[6] {
		1, 1, 1,
		0, 1, 0
	}, 3, 2}, {new char[6] {
		0, 1,
		1, 1,
		0, 1
	}, 2, 3}},new Pos[4]{
		{1, 0},
		{-1, 1},
		{0, -1},
		{0, 0}
	}
	},
	{4, new SquareArray[4]{{new char[6] {//MAP_BLOCK_L_FLIP
		0, 1,
		0, 1,
		1, 1
	}, 2, 3}, {new char[6]{
		1, 0, 0,
		1, 1, 1
	}, 3, 2}, {new char[6]{
		1, 1,
		1, 0,
		1, 0
	}, 2, 3}, {new char[6]{
		1, 1, 1,
		0, 0, 1
	}, 3, 2}},new Pos[4]{
		{0, 0},
		{1, 0},
		{-1, 1},
		{0, -1}
	}},
	{2, new SquareArray[2]{{new char[6]{//MAP_BLOCK_STEP_FLIP
		0, 1,
		1, 1,
		1, 0
	}, 2, 3}, {new char[6]{
		1, 1, 0,
		0, 1, 1
	}, 3, 2}}, new Pos[2]{
		{-1, 1},
		{1, -1}
	}}
};
inline Shape get_shape(char block) {
	return shapes[block - MAP_BLOCK - 1];
}

SquareArray map = {nullptr, DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT};
SquareArray buffer_area = {nullptr, DEFAULT_MAP_WIDTH, BUFFER_HEIGHT};
SquareArray buffered_map = {nullptr, DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT};
inline char get(int x, int y, SquareArray obj) {
	return obj.data[x + y * obj.width];
}
inline void set(char value, int x, int y, SquareArray obj) {
	obj.data[x + y * obj.width] = value;
}
inline char get_map(int x, int y) {
	if(y >= 0) {
		return get(x, y, map);
	} else {
		return get(x, y + buffer_area.height, buffer_area);
	}
}
inline void set_map(char value, int x, int y) {
	if(y >= 0) {
		set(value, x, y, map);
	} else {
		set(value, x, y + buffer_area.height, buffer_area);
	}
}
inline bool is_block(char value) {
	return value > MAP_BLOCK && value < MAP_BLOCK_END;
}
/*
 * Return value: True means success
 */
bool parse_int(const char *num_str, int *output) {
	int result = 0;
	char *p = (char *)num_str;
	while(*p) {
		if(*p < '0' || *p > '9') {
			return false;
		}
		result += *p - '0';
		result *= 10;
		p++;
	}
	result /= 10;
	*output = result;
	return true;
}

struct Cmdline {
	const char *name;
	const char *description;
	int argc;
	void (*act)(char **argv);
};
bool if_output_buffer_area = false;
int delay_time = DEFAULT_DELAY_TIME;
Cmdline cmdline[CMDLINE_COUNT] = {
	{"--help", "Display this help and exit", 0, [](char **argv) {
		fprintf(stderr, "%s: Console tetris game.\nUse arrawkeys LEFT DOWN RIGHT to move the block\nUse arrawkey UP to rotate the block\nUse SPACE to skip the block\nPress q to quit\nArguments:\n", argv[0]);
		for(int i = 0; i < CMDLINE_COUNT; i++) {
			fprintf(stderr, "\t%s\n\t\t%s\n", cmdline[i].name, cmdline[i].description);
		}
		end(0);
	}},
	{"--map-size", "[width] [height]: Set the size of the map", 2, [](char **argv) {
		if(!parse_int(argv[0], &map.width)) {
			log_error("The width of the map should be an integer, but got %s", argv[0]);
			end(1);
		}
		if(!parse_int(argv[1], &map.height)) {
			log_error("The height of the map should be an integer, but got %s", argv[1]);
			end(1);
		}
		if(map.width < MINIMUM_MAP_WIDTH) {
			log_error("The width of the map \"%d\" is too small.", map.width);
			end(1);
		}
		if(map.height < MINIMUM_MAP_HEIGHT) {
			log_error("The height of the map \"%d\" is too small.", map.height);
			end(1);
		}
		buffer_area.width = map.width;
		buffered_map.width = map.width;
		buffered_map.height = map.height;
	}},
	{"--delay-time", "Set delay time between frames(ms)", 1, [](char **argv){
		if(!parse_int(argv[0], &delay_time)) {
			log_error("Delay time should be an integer, but got %s", argv[0]);
			end(1);
		}
	}},
	{"--show-buffer-area", "Output buffer area", 0, [](char **argv) {
		if_output_buffer_area = true;
	}}
};

void parse_cmdline(int argc, char **argv) {
	bool called[CMDLINE_COUNT] = {false};
	char **cmdline_argv[CMDLINE_COUNT] = {nullptr};
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] != '-') {
			log_error("Unexcepted %s. Type \"%s --help\" for usage.", argv[i], argv[0]);
			end(1);
		}
		bool cmdline_exist = false;
		for(int j = 0; j < CMDLINE_COUNT; j++) {
			if(strcmp(argv[i], cmdline[j].name) == 0) {
				cmdline_exist = true;
				int given_argc = 0;
				while(i + given_argc + 1 < argc) {
					if(argv[i + given_argc + 1][0] != '-') {
						given_argc ++;
					} else {
						break;
					}
				}
				if(given_argc != cmdline[j].argc) {
					log_error("Cmdline %s needs %d argument%s, but got %d", cmdline[j].name, cmdline[j].argc, cmdline[j].argc <= 1 ? "" : "s", given_argc);
					end(1);
				}
				called[j] = true;
				if(cmdline[j].argc == 0) {
					cmdline_argv[j] = argv;
				} else {
					cmdline_argv[j] = argv + i + 1;
				}
				i += given_argc;
			}
		}
		if(!cmdline_exist) {
			log_error("Unknown cmdline %s. Type \"%s --help\" for usage.", argv[i], argv[0]);
			end(1);
		}
	}
	for(int i = 0; i < CMDLINE_COUNT; i++) {
		if(called[i]) {
			cmdline[i].act(cmdline_argv[i]);
		}
	}
}


inline char random_block() {
	return (char)(rand() % (MAP_BLOCK_END - MAP_BLOCK - 1) + MAP_BLOCK + 1);
}

char now_block, next_block;
bool will_output, will_generate_block, is_lost;
int score, rotate_count;

const char* get_color_from_block(char block) {
	switch(block) {
		case MAP_EMPTY:
			return "  ";
			break;
		case MAP_BLOCK:
			return "\033[47m  \033[0m";
			break;
		case MAP_BLOCK_L:
			return "\033[41m  \033[0m";
			break;
		case MAP_BLOCK_LONG:
			return "\033[42m  \033[0m";
			break;
		case MAP_BLOCK_SQUARE:
			return "\033[43m  \033[0m";
			break;
		case MAP_BLOCK_STEP:
			return "\033[44m  \033[0m";
			break;
		case MAP_BLOCK_CONVEX:
			return "\033[45m  \033[0m";
			break;
		case MAP_BLOCK_L_FLIP:
			return "\033[46m  \033[0m";
			break;
		case MAP_BLOCK_STEP_FLIP:
			return "\033[103m  \033[0m";
			break;
		default:
			log_error("Unknown enum detected: %d", block);
			end(1);
	}
	return nullptr;
}
void output_clear() {
	static bool first = true;
	if(first) {
		printf("\033[2J\033[0;0H");
		first = false;
	}
	printf("\033[0;0H");
}
void output_map_soft() {
	//printf("\033[1C\033[1B");
	int cursor_x = 0, cursor_y = 0;
	void (*gotoxy)(int, int, int, int) = [](int x, int y, int cursor_x, int cursor_y){
		//printf("\033[%d%c\033[%d%c", abs(x - cursor_x) * 2, x > cursor_x ? 'C' : 'D', abs(y - cursor_y), y > cursor_y ? 'B' : 'A');
		printf("\033[%d;%dH", y + 7, x * 2 + 2);
	};
	for(int y = 0; y < map.height; y++) {
		for(int x = 0; x < map.width; x++) {
			char map_value = get(x, y, map);
			if(map_value != get(x, y, buffered_map)) {
				gotoxy(x, y, cursor_x, cursor_y);
				printf("%s", get_color_from_block(map_value));
				cursor_x = x;
				cursor_y = y;
			}
		}
	}
	printf("\033[%d;2H", map.height + 7);
	for(int x = 0; x < map.width; x++) {
		bool highlight = false;
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			if (controlled_block[i].x == x) {
				highlight = true;
				break;
			}
		}
		if(highlight) {
			printf("==");
		} else {
			printf("--");
		}
	}
	printf("\033[%d;0H", map.height + 8);
	fflush(stdout);
	memcpy(buffered_map.data, map.data, sizeof(char) * map.width * map.height);
}
void output(SquareArray obj = map) {
	printf("\033[K|");
	for(int i = 0; i < obj.width; i++) {
		printf("==");
	}
	printf("|\n");
	for(int y = 0; y < obj.height; y++) {
		printf("\033[K|");
		for(int x = 0; x < obj.width; x++) {
			printf("%s", get_color_from_block(get(x, y, obj)));
		}
		printf("|\n");
	}
	printf("\033[K|");
	for(int x = 0; x < obj.width; x++) {
		bool highlight = false;
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			if (controlled_block[i].x == x) {
				highlight = true;
				break;
			}
		}
		if(highlight) {
			printf("==");
		} else {
			printf("--");
		}
	}
	printf("|\n");
	if(obj.data == map.data) {
		memcpy(buffered_map.data, map.data, sizeof(char) * map.width * map.height);
	}
}

void output_next_block() {
	SquareArray data = get_shape(next_block).datas[0];
	int space_count = map.width - 3;
	int min = (map.width - 4) / 2;
	int max = min + 4;
	Pos data_min = {2 - data.width / 2 + min, 2 - data.height / 2};
	Pos data_max = {data_min.x + data.width, data_min.y + data.height};
	for(int i = 0; i < space_count; i++) {
		if(i == space_count - 1) {
			printf("|");
		} else {
			printf(" ");
		}
	}
	for(int i = min; i < max; i++) {
		printf("--");
	}
	printf("|\n");
	for(int y = 0; y < 4; y++) {
		for(int i = 0; i < space_count; i++) {
			if(i == space_count - 1) {
				printf("|");
			} else {
				printf(" ");
			}
		}
		for(int x = min; x < max; x++) {
			if(x >= data_min.x && x < data_max.x && y >= data_min.y && y < data_max.y) {
				printf("%s", get(x - data_min.x, y - data_min.y, data) == 1 ? get_color_from_block(next_block) : "  ");
			} else {
				printf("  ");
			}
		}
		printf("|\n");
	}
}

void generate_block() {
	now_block = next_block;
	next_block = random_block();
	rotate_count = 0;

	SquareArray shape = get_shape(now_block).datas[0];
	int index = 0;
	for(int y = 0; y < shape.height; y++) {
		for(int x = 0; x < shape.width; x++) {
			int map_x = (map.width - shape.width) / 2 + x;
			int map_y = y - shape.height;
			if(is_block(get_map(map_x, map_y))) {
				is_lost = true;
			}
			char shape_data_temp = get(x, y, shape);
			if(shape_data_temp == 1) {
				controlled_block[index] = {map_x, map_y};
				index++;
				set_map(now_block, map_x, map_y);
			} else {
				set_map(MAP_EMPTY, map_x, map_y);
			}
		}
	}
}

void init_game() {
	static bool first = true;
	if(first) {
		map.data = new char[map.width * map.height];
		buffer_area.data = new char[buffer_area.width * buffer_area.height];
		buffered_map.data = new char[buffered_map.width * buffered_map.height];
		first = false;
	}
	srand((unsigned int)time(NULL));
	memset(map.data, MAP_EMPTY, sizeof(char) * map.width * map.height);
	memset(buffer_area.data, MAP_EMPTY, sizeof(char) * buffer_area.width * buffer_area.height);

	next_block = random_block();
	generate_block();
	will_generate_block = false;
	will_output = false;
	is_lost = false;
	score = 0;
	rotate_count = 0;
}

void move_down() {
	if(will_generate_block) {
		return;
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos &p = controlled_block[i];
		set_map(MAP_EMPTY, p.x, p.y);
		p.y++;
	}
	bool cancel = false;
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		if(p.y >= map.height) {
			cancel = true;
			break;
		}
		if(is_block(get_map(p.x, p.y))) {
			cancel = true;
			break;
		}
	}
	if(cancel) {
		will_generate_block = true;
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			Pos &p = controlled_block[i];
			p.y--;
		}
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		set_map(now_block, p.x, p.y);
	}
}
void move_left() {
	if(will_generate_block) {
		return;
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos &p = controlled_block[i];
		set_map(MAP_EMPTY, p.x, p.y);
		p.x--;
	}
	bool cancel = false;
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		if(p.x < 0) {
			cancel = true;
			break;
		}
		if (is_block(get_map(p.x, p.y))) {
			cancel = true;
			break;
		}
	}
	if(cancel) {
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			Pos &p = controlled_block[i];
			p.x++;
		}
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		set_map(now_block, p.x, p.y);
	}
}
void move_right() {
	if(will_generate_block) {
		return;
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos &p = controlled_block[i];
		set_map(MAP_EMPTY, p.x, p.y);
		p.x++;
	}
	bool cancel = false;
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		if(p.x >= map.width) {
			cancel = true;
			break;
		}
		if (is_block(get_map(p.x, p.y))) {
			cancel = true;
			break;
		}
	}
	if(cancel) {
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			Pos &p = controlled_block[i];
			p.x--;
		}
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		set_map(now_block, p.x, p.y);
	}
}
void rotate_block() {
	if(will_generate_block || get_shape(now_block).count == 1) {
		return;
	}
	Pos old_controlled_block[CONTROLLED_BLOCK_COUNT], min = {2147483647, 2147483647};
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		old_controlled_block[i] = p;
		if(p.x < min.x) min.x = p.x;
		if(p.y < min.y) min.y = p.y;

		set_map(MAP_EMPTY, p.x, p.y);
	}
	Pos rotate_pos = get_shape(now_block).rotate_pos[rotate_count];
	rotate_count++;
	if(rotate_count >= get_shape(now_block).count) {
		rotate_count = 0;
	}
	SquareArray data = get_shape(now_block).datas[rotate_count];
	rotate_pos.x += min.x;
	rotate_pos.y += min.y;
	int index = 0;
	for(int y = 0; y < data.height; y++) {
		for(int x = 0; x < data.width; x++) {
			if(get(x, y, data) == 1) {
				controlled_block[index] = {rotate_pos.x + x, rotate_pos.y + y};
				index++;
			}
		}
	}
	bool cancel = false;
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		if(p.x >= map.width || p.x < 0 || p.y >= map.height || p.y < 0) {
			cancel = true;
			break;
		}
		if (is_block(get_map(p.x, p.y))) {
			cancel = true;
			break;
		}
	}
	if(cancel) {
		rotate_count --;
		if(rotate_count < 0) {
			rotate_count = get_shape(now_block).count - 1;
		}
		for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
			Pos &p = controlled_block[i];
			p = old_controlled_block[i];
		}
	}
	for(int i = 0; i < CONTROLLED_BLOCK_COUNT; i++) {
		Pos p = controlled_block[i];
		set_map(now_block, p.x, p.y);
	}
}

void process_input() {
	int arraw_key_level = 0;
	while(true) {
		char ch = getch();
		if(ch == 27 && arraw_key_level == 0) {//PROCESS ARROWKEYS
			arraw_key_level = 1;
		} else if(ch == 91 && arraw_key_level == 1) {
			arraw_key_level = 2;
		} else if((ch >= 'A' && ch <= 'D') && arraw_key_level == 2) {
			switch(ch) {
				case 'A':
					rotate_block();
					break;
				case 'B':
					move_down();
					break;
				case 'C':
					move_right();
					break;
				case 'D':
					move_left();
					break;
			}
			arraw_key_level = 0;
		} else {//PROCESS NORMAL KEYS
			arraw_key_level = 0;
			switch(ch) {
				case ' ':
					while(!will_generate_block){
						move_down();
					}
					break;
				case 'q':
					is_lost = true;
					break;
				case 'w':
					rotate_block();
					break;
				case 's':
					move_down();
					break;
				case 'd':
					move_right();
					break;
				case 'a':
					move_left();
					break;
			}
		}
		will_output = true;
	}
}
void check_map() {
	bool *all_block_line = new bool[map.height];
	memset(all_block_line, true, sizeof(bool) * map.height);
	int all_block_line_count = map.height;
	for(int y = map.height - 1; y >= 0; y--) {
		for(int x = 0; x < map.width; x++) {
			if(!is_block(get_map(x, y))) {
				all_block_line[y] = false;
				all_block_line_count--;
				break;
			}
		}
	}
	switch(all_block_line_count) {
		case 0:
			break;
		case 1:
			score += 10;
			break;
		case 2:
			score += 40;
			break;
		case 3:
			score += 100;
			break;
		case 4:
			score += 200;
			break;
	}
	int down = 0;
	for(int y = map.height - 1; y >= 0; y--) {
		if(all_block_line[y]) {
			down++;
			continue;
		}
		if(down != 0) {
			for(int x = 0; x < map.width; x++) {
				set_map(get_map(x, y), x, y + down);
				set_map(MAP_EMPTY, x, y);
			}
		}
	}
	delete[] all_block_line;
}
void game_loop() {
	while(true) {
		move_down();
		if(will_generate_block) {
			generate_block();
			will_generate_block = false;
			check_map();
		}
		will_output = true;
		delay(delay_time);
	}
}

void output_loop() {
	output_clear();
	output_next_block();
	output();
	while(true) {
		while(!will_output) {
			yield();
		}
		output_clear();
		output_next_block();
 		if(if_output_buffer_area) {
			output(buffer_area);
		}
		output_map_soft();
		fprintf(stderr, "Score: %d\n", score);
		will_output = false;
		if(is_lost) {
			end(0);
		}
	}
}

int main(int argc, char **argv) {
	parse_cmdline(argc, argv);
	init_game();
	std::thread t(process_input);
	std::thread t2(output_loop);
	game_loop();
	return 0;
}
