#define INCLUDE_ARGUMENT
#include "utils.h"
#include <stack>

class Program {
private:
	std::vector<unsigned char> memory;
	std::vector<std::pair<size_t, size_t>> loop;

	size_t point_pos = 0;
	bool point_initialize();
public:
	bool increase();
	bool decrease();
	bool left();
	bool right();
	bool input();
	bool output();

	bool process(const char *source_code);
};

bool Program::point_initialize() {
	while(memory.size() <= point_pos) {
		memory.push_back(0);
	}
	return true;
}
bool Program::increase() {
	if(!point_initialize()) {
		return false;
	}
	++memory[point_pos];
	return true;
}
bool Program::decrease() {
	if(!point_initialize()) {
		return false;
	}
	--memory[point_pos];
	return true;
}
bool Program::left() {
	if(point_pos == 0) {
		return false;
	}
	--point_pos;
	return true;
}
bool Program::right() {
	++point_pos;
	return true;
}
bool Program::input() {
	if(!point_initialize()) {
		return false;
	}
	memory[point_pos] = getch();
	return true;
}
bool Program::output() {
	if(!point_initialize()) {
		return false;
	}
	putchar(memory[point_pos]);
	return true;
}
bool Program::process(const char *source_code) {
	size_t loop_level = 0;
	for(size_t i = 0; source_code[i];) {
		char c = source_code[i];
		if(c == '[' && loop.size() == 0) {
			for(; source_code[i]; ++i) {
				c = source_code[i];
				if(c == '[') {
					loop.push_back(std::pair<size_t, size_t>(i, 0));
					++loop_level;
				} else if(c == ']') {
					if(loop_level == 0) {
						return false;
					}
					loop[--loop_level].second = i;
					if(loop_level == 0) {
						i = loop[0].first;
						break;
					}
				}
			}
			if(loop_level != 0) {
				return false;
			}
		}
		c = source_code[i];
		switch(c){
			case '+':
				if(!increase()) {
					return false;
				}
				++i;
				break;
			case '-':
				if(!decrease()) {
					return false;
				}
				++i;
				break;
			case '>':
				if(!right()) {
					return false;
				}
				++i;
				break;
			case '<':
				if(!left()) {
					return false;
				}
				++i;
				break;
			case '.':
				if(!output()) {
					return false;
				}
				++i;
				break;
			case '!':
				if(!input()) {
					return false;
				}
				++i;
				break;
			case '[':
				if(!point_initialize()) {
					return false;
				}
				if(memory[point_pos]) {
					loop_level ++;
					++i;
				} else {
					i = loop[loop_level].second + 1;
					loop.pop_back();
				}
				break;
			case ']':
				if(loop_level == 0) {
					return false;
				}
				i = loop[--loop_level].first;
				break;
			default:
				++i;
		}
	}
	if(loop_level != 0) {
		return false;
	}
	return true;
}

void end(int code) {
	exit(code);
}
std::string source_code;

void process_argument(int argc, char **argv) {
	static ArgumentFactory *af = nullptr;
	af = new ArgumentFactory;

	Argument help, file, text;
	help.add_name("-h").add_name("--help");
	help.set_argc(0);
	help.set_called_limit(1);
	help.set_description("Display this help and exit");
	help.set_act_func([](char **argv) {
		const char **head = new const char*[]{argv[0], ": Brainfuck"};
		af->output_help(2, head);
		delete []head;
		exit(0);
	});

	text.add_name("-t").add_name("--text");
	text.set_argc(1);
	text.set_description("Specify source code");
	text.set_act_func([](char **argv) {
		source_code += argv[0];
	});

	af->register_argument(help);
	af->register_argument(file);
	af->register_argument(text);
	af->set_default_argument(text);

	if(!af->process(argc, argv)) {
		end(1);
	}

	delete af;
}

int main(int argc, char **argv) {
	process_argument(argc, argv);
	if(source_code.empty()) {
		source_code = "++++++++++[>++++++++++<-]>>++++[>+++++++++++<-]<++++.---.+++++++..+++.>>.------------.<<++++++++.--------.+++.------.--------.>++++++++++.";
	}
	Program p;
	if(!p.process(source_code.c_str())) {
		log_error("Error");
		end(1);
	}
}
