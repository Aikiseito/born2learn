#pragma once

#define MEMSIZE (64 * 1024) 	// память
#define pc reg[7] 				// Program Counter

typedef unsigned char byte; 	// 8 bit
typedef unsigned short int word; // 16 bit
typedef word adr; 				// 16 bit

typedef struct {
	word val;
	word adr;
} Arg;

typedef struct {
	Arg ss;
	Arg dd;
	unsigned char r;
	unsigned char nn;
} Par;

typedef struct {
	word mask;
	word opcode;
	char * name;
	char param;
	void (* do_func)(Par p);
} Command;

extern byte mem[MEMSIZE]; 		//Память
extern word reg[8]; 			//Регистры R0, ..., R8
extern Arg ss, dd;
extern Command cmd[];

byte b_read  (adr a);
void b_write (adr a, byte val);
word w_read  (adr a);
void w_write (adr a, word val);
void run();
Arg get_mr(word w);

void do_halt();
void do_mov();
void do_add();
void do_nothing();

void trace(const char * format, ...);
void load_file (const char * filename);
void mem_dump(adr start, word n);

void test_mem();
