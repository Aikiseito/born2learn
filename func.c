#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "pdp11.h"

byte mem[MEMSIZE];
char * x = "-q";

byte b_read  (adr a) {
	return mem[a];
}

void b_write (adr a, byte val) {
	mem[a] = val;
}

word w_read  (adr a) {
	word w = mem[a + 1] << 8;
	w = w | mem[a];
	return w;
}

void w_write (adr a, word val) {
	mem[a + 1] = (val >> 8);
	mem[a] = val;
}

void test_mem() {
	//пишем байт, читаем байт
	byte b0 = 0xba;
	b_write(2, b0);
	byte bres = b_read(2);
	assert(b0 == bres);

	//пишем слово, читаем слово
	adr a = 4;
	word w2 = 0xabcd;
	w_write(a, w2);
	word wres2 = w_read(a);
	assert(w2 == wres2);

	//пишем 2 байта, читаем слово
	byte b1 = 0xcb;
	word w = 0xcbba;
	b_write(a, b0);
	b_write(a+1, b1);
	word wres = w_read(a);
	assert(w == wres);

	//пишем слово, читаем 2 байта
	word w3 = 0xcbba;
	w_write(a, w3);
	byte bres1 = b_read(a+1), bres2 = b_read(a);
	assert((word)(bres1 << 8 | bres2) == w3);
}

void trace(const char * format, ...) {
	if (!strcmp(x, "-t")) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
		printf("\n");
	}
}

void load_file (const char * filename) {
	adr a;
	int num, i;
	byte b;
	FILE * f = stdin;
	if (filename != NULL)
		f = fopen(filename, "r");
	while(fscanf(f, "%hx%x", &a, &num) == 2){
		printf("num = %hhx\n", num);
		for (i = 0; i < num; i++) {
			fscanf(f, "%hhx", &b);
			b_write(a, b);
			trace("%hhx %hx", b, a);
			a++;
		}
	}
	if (filename != NULL)
		fclose(f);
}

void mem_dump(adr start, word n) {
	int i;
	adr a = start;
	for (i = 0; i < n; i+=2) {
		printf("%06ho : %06ho\n", a, w_read(a));
		a += 2;
	}
}
