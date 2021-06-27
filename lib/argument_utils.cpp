#include "argument_utils.h"
#include "utils.h"

#include <cstring>

bool Argument::is_name(const char *given_name) const {
	for(std::string name : names) {
		if(name == given_name) {
			return true;
		}
	}
	return false;
}

Argument& Argument::add_name(const char *specified_name) {
	names.push_back(specified_name);
	return *this;
}
Argument& Argument::set_argc(int given_argc) {
	argc = given_argc;
	return *this;
}
Argument& Argument::set_act_func(void (*given_act_func)(char **argv)) {
	act_func = given_act_func;
	return *this;
}
Argument& Argument::set_description(const char *given_description) {
	description = given_description;
	return *this;
}

Argument& Argument::set_called_limit(unsigned int given_max_called_time) {
	max_called_time = given_max_called_time;
	return *this;
}
void Argument::act(char **argv) const{
	if(act_func != nullptr) {
		act_func(argv);
	}
}

bool Argument::equals(const Argument &b) const {
	const Argument &a = *this;
	if(a.names == b.names &&
		a.argc == b.argc &&
		a.max_called_time == b.max_called_time &&
		a.act_func == b.act_func &&
		a.description == b.description) {
		return true;
	}
	return false;
}

bool ArgumentFactory::register_argument(const Argument &given_arg) {
	arguments.push_back(given_arg);
	return true;
}
bool ArgumentFactory::set_default_argument(const Argument &specified_arg) {
	int index = 0;
	for(const Argument &arg : arguments) {
		if(specified_arg.equals(arg)) {
			default_argument_pos = index;
			return true;
		}
		++index;
	}
	return false;
}

bool ArgumentFactory::process(int argc, char **argv) const{
	if(arguments.empty()) {
		return false;
	}
	size_t argument_count = arguments.size();
	unsigned int *called_time = new unsigned int[argument_count];
	memset(called_time, 0, sizeof(unsigned int) * argument_count);

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
	const char *(*longest_name)(const Argument &)  = [](const Argument &arg){
		const std::string *result_p = nullptr;
		size_t max = 0;
		for(const std::string &str : arg.names) {
			size_t len = str.size();
			if(len >= max) {
				result_p = &str;
				max = len;
			}
		}
		if(result_p == nullptr) {
			return (const char *)nullptr;
		} else {
			return result_p->c_str();
		}
	};
	auto get_argument_from_name = [this](const char *given_name) {
		int index = 0;
		for(const Argument &arg : arguments) {
			if(arg.is_name(given_name)) {
				return index;
			}
			++index;
		}
		return -1;
	};

	for(int i = 1; i < argc; i++) {
		if(strlen(argv[i]) == 0) {
			continue;
		}
		int j = get_argument_from_name(argv[i]);
		if(j == -1) {
			if(default_argument_pos == -1) {
				if(argv[i][0] == '-') {
					log_error("Unknown argument \"%s\". Type \"%s --help\" for usage.", argv[i], argv[0]);
					free_everything();
					return false;
				} else {
					log_error("Unexcepted \"%s\". Type \"%s --help\" for usage.", argv[i], argv[0]);
					free_everything();
					return false;
				}
			}
			const Argument &called_arg = arguments[default_argument_pos];
			const char *called_arg_name = longest_name(called_arg);
			if(called_arg_name == nullptr) {
				called_arg_name = "";
			}
			int given_argc = 0;
			for(int k = i; k < argc; k++) {
				if(given_argc == called_arg.argc) {
					break;
				}
				if(strlen(argv[k]) == 0) {
					given_argc ++;
					continue;
				}
				if(argv[k][0] == '-') {
					if(get_argument_from_name(argv[k]) == -1) {
						given_argc ++;
						continue;
					} else {
						break;
					}
				}
				given_argc ++;
			}
			if(given_argc != called_arg.argc) {
				log_error("Argument \"%s\" requires %d sub parameter%s, but got %d.", called_arg_name, called_arg.argc, called_arg.argc < 2 ? "" : "s", given_argc);
				free_everything();
				return false;
			}
			if(called_time[default_argument_pos] == 0) {
				arg_pos[default_argument_pos] = new int[called_arg.max_called_time];
			}
			if(called_time[default_argument_pos] == called_arg.max_called_time) {
				log_error("The time of argument \"%s\" to be called reaches its limit %d.", called_arg_name, called_arg.max_called_time);
				free_everything();
				return false;
			}
			arg_pos[default_argument_pos][called_time[default_argument_pos]] = i - 1;
			called_time[default_argument_pos] ++;
			i += called_arg.argc - 1;
			continue;
		}
		const Argument &called_arg = arguments[j];

		int given_argc = 0;
		for(int k = i + 1; k < argc; k++) {
			if(given_argc == called_arg.argc) {
				break;
			}
			if(strlen(argv[k]) == 0) {
				given_argc ++;
				continue;
			}
			if(argv[k][0] == '-') {
				if(get_argument_from_name(argv[k]) == -1) {
					given_argc ++;
					continue;
				} else {
					break;
				}
			}
			given_argc ++;
		}
		if(given_argc != called_arg.argc) {
			log_error("Argument \"%s\" requires %d sub parameter%s, but got %d.", argv[i], called_arg.argc, called_arg.argc < 2 ? "" : "s", given_argc);
			free_everything();
			return false;
		}
		if(called_time[j] == 0) {
			arg_pos[j] = new int[MAX_CALL_TIME];
		}
		if(called_time[j] == called_arg.max_called_time) {
			log_error("The time of argument \"%s\" to be called reaches its limit %d.", argv[i], called_arg.max_called_time);
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

void ArgumentFactory::output_help(int head_count, ...) const{
	va_list p;
	va_start(p, head_count);
	for(int i = 0; i < head_count; i++) {
		fprintf(stderr, "%s", va_arg(p, const char *));
	}
	va_end(p);
	fprintf(stderr, "\nArguments:\n");
	for(const Argument &arg : arguments) {
		fprintf(stderr, "\t");
		bool first = true;
		for(const std::string &name : arg.names) {
			if(first) {
				first = false;
			} else {
				fprintf(stderr, ", ");
			}
			fprintf(stderr, "%s", name.c_str());
		}
		if(!arg.description.empty()) {
			fprintf(stderr, ":\n\t\t%s\n", arg.description.c_str());
		} else {
			fprintf(stderr, "\n");
		}
	}
}
