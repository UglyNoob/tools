#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
	printf("%d argument(s) in total\n", argc);
	for(int i = 0; i < argc; i++) {
		printf("argv[%d](%d byte(s)): %s\n", i, strlen(argv[i]) + 1, argv[i]);
	}
	return 0;
}
