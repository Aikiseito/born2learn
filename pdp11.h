#pragma once

#define MEMSIZE (64 * 1024) 	// память
#define pc reg[7] 				// Program Counter

typedef unsigned char byte; 	// 8 bit
typedef unsigned short int word; // 16 bit
typedef word adr; 				// 16 bit

extern byte mem[MEMSIZE]; 		//Память
extern word reg[8]; 			//Регистры R0, ..., R8

byte b_read  (adr a);
void b_write (adr a, byte val);
word w_read  (adr a);
void w_write (adr a, word val);

void trace(const char * format, ...);
void load_file (const char * filename);
void mem_dump(adr start, word n);

void test_mem();