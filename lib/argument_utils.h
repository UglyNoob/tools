#ifndef __ARGUMENT_UTILS_H__
#define __ARGUMENT_UTILS_H__

#include <vector>
#include <string>
#include <initializer_list>
#include <cstddef>

const size_t MAX_CALL_TIME = 1024;

class Argument {
	friend class ArgumentProcessor;
private:
	size_t argc = 0;
	size_t max_called_time = MAX_CALL_TIME;
	void (*act_func)(char **argv) = nullptr;

	std::vector<std::string> names;
	std::string description;

	void act(char **argv) const;
public:
	Argument() = default;
	Argument(const Argument &arg_) = default;
	Argument(Argument &&arg_) = default;

	bool is_name(const std::string &name_) const;
	bool equals(const Argument &) const;

	Argument& add_name(const std::string &name_);
	Argument& set_argc(size_t argc_);
	Argument& set_act_func(void (*given_act_func)(char **argv));
	Argument& set_description(const std::string &description_);
	Argument& set_called_limit(size_t given_max_called_time);
};

class ArgumentProcessor {
private:
	int default_argument_pos = -1;
	std::vector<Argument> arguments;
public:
	bool register_argument(const Argument &given_arg);
	bool set_default_argument(const Argument &specified_arg);

	bool process(int argc, char **argv) const;
	void output_help(std::initializer_list<std::string> head = {}, std::initializer_list<std::string> tail = {}) const;
};

#endif
