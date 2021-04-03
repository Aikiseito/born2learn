#include <stdio.h>
#include "pdp11.h"

extern byte mem[MEMSIZE];
extern char * x;

int main(int argc, char * argv[]) {
	if (argc < 3) {
		printf("Usage: options, name_of_input_file\n\n");
		printf("Options:\n-t: show trace to stderr\n-q: quiet, don't show anything except target's output\n\n");
	}
	else {
		x = argv[1];
		load_file(argv[2]);
		mem_dump(0x40, 4);
	}
	return 0;
}
