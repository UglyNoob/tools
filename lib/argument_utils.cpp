#include "argument_utils.h"
#include "utils.h"

#include <cstring>
#include <memory>

bool Argument::is_name(const std::string &name_) const {
	for(const std::string &name : names) {
		if(name == name_) {
			return true;
		}
	}
	return false;
}

Argument& Argument::add_name(const std::string &name_) {
	names.push_back(name_);
	return *this;
}
Argument& Argument::set_argc(size_t argc_) {
	argc = argc_;
	return *this;
}
Argument& Argument::set_act_func(void (*given_act_func)(char **argv)) {
	act_func = given_act_func;
	return *this;
}
Argument& Argument::set_description(const std::string &description_) {
	description = description_;
	return *this;
}

Argument& Argument::set_called_limit(size_t given_max_called_time) {
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

bool ArgumentProcessor::register_argument(const Argument &given_arg) {
	arguments.push_back(given_arg);
	return true;
}
bool ArgumentProcessor::set_default_argument(const Argument &specified_arg) {
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

bool ArgumentProcessor::process(int argc, char **argv) const {
	if(arguments.empty()) {
		return false;
	}
	size_t arguments_size = arguments.size();
	unsigned int *called_time = new unsigned int[arguments_size];
	memset(called_time, 0, sizeof(unsigned int) * arguments_size);

	int **arg_pos = new int*[arguments_size] {nullptr};
	auto free_everything = [called_time, arguments_size, arg_pos]() {
		delete[] called_time;
		for(int i = 0; i < arguments_size; i++) {
			if(arg_pos[i] != nullptr) {
				delete[] arg_pos[i];
			}
		}
		delete[] arg_pos;
	};
	auto longest_name  = [](const Argument &arg) {
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
	constexpr int ARGUMENT_NOT_FOUND = -1;
	auto get_argument_from_name = [this](const char *given_name) {
		int index = 0;
		for(const Argument &arg : arguments) {
			if(arg.is_name(given_name)) {
				return index;
			}
			++index;
		}
		return ARGUMENT_NOT_FOUND;
	};

	for(int i = 1; i < argc; i++) {
		if(strlen(argv[i]) == 0) {
			continue;
		}
		int j = get_argument_from_name(argv[i]);
		if(j == ARGUMENT_NOT_FOUND) {
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
		const Argument &argument_called = arguments[j];

		int given_argc = 0;
		for(int k = i + 1; k < argc; k++) {
			if(given_argc == argument_called.argc) {
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
		if(given_argc != argument_called.argc) {
			log_error("Argument \"%s\" requires %d sub parameter%s, but got %d.", argv[i], argument_called.argc, argument_called.argc < 2 ? "" : "s", given_argc);
			free_everything();
			return false;
		}
		if(called_time[j] == 0) {
			arg_pos[j] = new int[MAX_CALL_TIME];
		}
		if(called_time[j] == argument_called.max_called_time) {
			log_error("The time of argument \"%s\" to be called reaches its limit %d.", argv[i], argument_called.max_called_time);
			free_everything();
			return false;
		}
		arg_pos[j][called_time[j]] = i;
		called_time[j] ++;
		i += argument_called.argc;
	}
	for(int i = 0; i < arguments_size; i++) {
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

void ArgumentProcessor::output_help(std::initializer_list<std::string> head, std::initializer_list<std::string> tail) const {
	for(const auto &str : head) {
		fprintf(stderr, "%s", str.c_str());
	}

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
		if(arg.description.empty()) {
			fprintf(stderr, "\n");
		} else {
			fprintf(stderr, ":\n\t\t%s\n", arg.description.c_str());
		}
	}

	for(const auto &str : tail) {
		fprintf(stderr, "%s", str.c_str());
	}
}
