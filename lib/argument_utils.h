#ifndef __ARGUMENT_UTILS_H__
#define __ARGUMENT_UTILS_H__

const int MAX_NAME_COUNT = 100;
const int MAX_ARGUMENT_COUNT = 1024;

class Argument {
public:
	int name_count = 0;
	int argc = 0;
	char *names[MAX_NAME_COUNT];
	void (*act_func)(char **argv) = nullptr;
	const char *description = nullptr;

	~Argument();
	bool add_name(const char *specified_name);
	bool is_name(const char *given_name);
	void set_argc(int given_argc);
	void set_act_func(void (*given_act_func)(char **argv));
	void set_description(const char *given_description);
	void act(char **argv);
};

class ArgumentFactory {
public:
	int argument_count = 0;
	int default_argument_pos = -1;
	Argument arguments[MAX_ARGUMENT_COUNT];
	bool register_argument(Argument &specified_arg);
	bool set_default_argument(Argument &specified_arg);
	bool process(int argc, char **argv);

	void output_help(int head_count = 0, ...);
};

#endif
