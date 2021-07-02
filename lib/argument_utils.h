#ifndef __ARGUMENT_UTILS_H__
#define __ARGUMENT_UTILS_H__

const unsigned int MAX_CALL_TIME = 1024;

#include <vector>
#include <string>

class Argument {
	friend class ArgumentFactory;
private:
	int argc = 0;
	int max_called_time = MAX_CALL_TIME;
	std::vector<std::string> names;
	void (*act_func)(char **argv) = nullptr;
	std::string description;

	void act(char **argv) const;
public:
	bool is_name(const char *given_name) const;
	Argument& add_name(const char *specified_name);
	Argument& set_argc(int given_argc);
	Argument& set_act_func(void (*given_act_func)(char **argv));
	Argument& set_description(const char *given_description);
	Argument& set_called_limit(unsigned int given_max_called_time);
	bool equals(const Argument &) const;
};

class ArgumentFactory {
private:
	int default_argument_pos = -1;
	std::vector<Argument> arguments;
public:
	bool register_argument(const Argument &given_arg);
	bool set_default_argument(const Argument &specified_arg);
	bool process(int argc, char **argv) const;

	void output_help(int head_count = 0, const char **head = nullptr, int tail_count = 0, const char **tail = nullptr) const;
};

#endif
