#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>

ArgumentFactory af;
const char *filename = nullptr;
int delay_time = 1000;

void delay_us(int us) {
	std::this_thread::sleep_for(std::chrono::microseconds(us));
}

void process_argument(int argc, char **argv) {
	Argument help, file, set_delay_time;

	help.add_name("-h");
	help.add_name("--help");
	help.set_argc(0);
	help.set_description("Display this help and exit");
	help.set_act_func([](char **argv) {
		af.output_help(2, argv[0], ": Display a file slowly.");
		exit(0);
	});
	af.register_argument(&help);

	file.add_name("-f");
	file.add_name("--file");
	file.set_argc(1);
	file.set_description("Specify file to display");
	file.set_act_func([](char **argv) {
		filename = argv[0];
	});
	af.register_argument(&file);

	set_delay_time.add_name("-t");
	set_delay_time.add_name("--delay-time");
	set_delay_time.set_argc(1);
	set_delay_time.set_description("Specify how long to delay between characters(Default 1000us)(us)");
	set_delay_time.set_act_func([](char **argv) {
		bool success;
		delay_time = parse_int(argv[0], &success);
		if(!success) {
			log_error("The time should be an integer, but got %s", argv[0]);
			exit(1);
		}
		if(delay_time < 0) {
			log_error("The time to delay is to small: %d", delay_time);
			exit(1);
		}
	});
	af.register_argument(&set_delay_time);

	if(!af.process(argc, argv)) {
		exit(0);
	}
}

int main(int argc, char **argv) {
	process_argument(argc, argv);
	if(filename == nullptr) {
		log_error("No file name specified. Type \"%s --help\" for usage.", argv[0]);
		exit(1);
	}
	FILE *f = fopen(filename, "r");
	if(f == nullptr) {
		log_error("Can't open file: %s", filename);
		exit(1);
	}
	char c = fgetc(f);
	while(c != (char)EOF) {
		putchar(c);
		fflush(stdout);
		delay_us(delay_time);
		c = fgetc(f);
	}
}
