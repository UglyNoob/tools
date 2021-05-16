#include "argument_utils.h"
#include "utils.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>

const int MAX_CALL_TIME = 1024;

Argument::~Argument() {
	for(int i = 0; i < name_count; i++) {
		delete[] names[i];
	}
}

bool Argument::add_name(const char *specified_name) {
	if(name_count == MAX_NAME_COUNT) {
		return false;
	}
	int specified_name_length = strlen(specified_name);
	char *name = new char[specified_name_length + 1];
	memcpy(name, specified_name, sizeof(char) * (specified_name_length + 1));
	names[name_count] = name;
	name_count ++;
	return true;
}

bool Argument::is_name(const char *given_name) {
	for(int i = 0; i < name_count; i++) {
		if(strcmp(given_name, names[i]) == 0) {
			return true;
		}
	}
	return false;
}
void Argument::set_argc(int given_argc) {
	argc = given_argc;
}
void Argument::set_act_func(void (*given_act_func)(char **argv)) {
	act_func = given_act_func;
}
void Argument::set_description(const char *given_description) {
	description = given_description;
}

void Argument::act(char **argv) {
	if(act_func != nullptr) {
		act_func(argv);
	}
}


bool ArgumentFactory::register_argument(Argument *specified_arg) {
	if(argument_count == MAX_ARGUMENT_COUNT) {
		return false;
	}
	Argument &dst = arguments[argument_count];
	dst = *specified_arg;
	dst.name_count = 0;
	for(int i = 0; i < specified_arg->name_count; i++) {
		dst.add_name(specified_arg->names[i]);
	}

	argument_count ++;
	return true;
}

int get_argument_from_name(ArgumentFactory *af, const char *given_name) {
	for(int i = 0; i < af->argument_count; i++) {
		if(af->arguments[i].is_name(given_name)) {
			return i;
		}
	}
	return -1;
}

bool ArgumentFactory::process(int argc, char **argv) {
	int *called_time = new int[argument_count];
	memset(called_time, 0, sizeof(int) * argument_count);

	int **arg_pos = new int*[argument_count] {nullptr};
	auto free_everything = [=](){
		delete[] called_time;
		for(int i = 0; i < argument_count; i++) {
			if(arg_pos[i] != nullptr) {
				delete[] arg_pos[i];
			}
		}
		delete[] arg_pos;
	};

	for(int i = 1; i < argc; i++) {
		if(strlen(argv[i]) == 0) {
			continue;
		}
		if(argv[i][0] != '-') {
			log_error("Unexcepted %s. Type \"%s --help\" for usage.", argv[i], argv[0]);
			free_everything();
			return false;
		}
		int j = get_argument_from_name(this, argv[i]);
		if(j == -1) {
			log_error("Unknown argument %s. Type \"%s --help\" for usage.", argv[i], argv[0]);
			free_everything();
			return false;
		}
		Argument &called_arg = arguments[j];

		int given_argc = 0;
		for(int k = i + 1; k < argc; k++) {
			if(strlen(argv[k]) == 0) {
				given_argc ++;
				continue;
			}
			if(argv[k][0] == '-') {
				if(get_argument_from_name(this, argv[k]) == -1) {
					given_argc ++;
					continue;
				} else {
					break;
				}
			}
			given_argc ++;
		}
		if(given_argc != called_arg.argc) {
			log_error("Argument %s requires %d sub parameter%s, but got %d", argv[i], called_arg.argc, called_arg.argc < 2 ? "" : "s", given_argc);
			free_everything();
			return false;
		}
		if(called_time[j] == 0) {
			arg_pos[j] = new int[MAX_CALL_TIME];
		}
		if(called_time[j] == MAX_CALL_TIME) {
			log_error("The time of argument \"%s\" to be called reaches its limit %d.", argv[i], MAX_CALL_TIME);
			free_everything();
			return false;
		}
		arg_pos[j][called_time[j]] = i;
		called_time[j] ++;
		i += called_arg.argc;
	}
	for(int i = 0; i < argument_count; i++) {
		if(called_time[i] != 0) {
			for(int j = 0; j < called_time[i]; j++) {
				if(arguments[i].argc == 0) {
					arguments[i].act(argv);
				} else {
					arguments[i].act(argv + arg_pos[i][j] + 1);
				}
			}
		}
	}
	free_everything();
	return true;
}

void ArgumentFactory::output_help(int head_count, ...) {
	va_list p;
	va_start(p, head_count);
	for(int i = 0; i < head_count; i++) {
		printf("%s", va_arg(p, const char *));
	}
	va_end(p);
	printf("\nArguments:\n");
	for(int i = 0; i < argument_count; i++) {
		printf("\t");
		for(int j = 0; j < arguments[i].name_count; j++) {
			if(j != 0) {
				printf(", ");
			}
			printf("%s", arguments[i].names[j]);
		}
		if(arguments[i].description != nullptr) {
			printf(":\n\t\t%s\n", arguments[i].description);
		} else {
			printf("\n");
		}
	}
}
