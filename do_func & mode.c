#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

word reg[8]; // регистры R0 .. R7

#define NO_PARAMS 0
#define HAS_DD 1
#define HAS_SS 2
extern char * x;
static Par p;

Arg ss, dd;

Arg get_mr(word w) {
	Arg res;
	int r = w & 7;			// номер регистра
	int m = (w >> 3) & 7;	// номер моды
	switch (m) {
		case 0:				//мода 0 R3
			res.adr = r;
			res.val = reg[r];
			trace("R%o ", r);
			break;
		case 1:				//мода 1 (R3)
			res.adr = reg[r];
			res.val = w_read(res.adr); //todo b_read
			trace("(R%o) ", r);
			break;
		case 2:				//мода 2 (R3)+	#3
			res.adr = reg[r];
			res.val = w_read(res.adr); //todo b_read
			reg[r] += 2; 				// todo +1
			if (r == 7)
				trace("(#%o) ", res.val);
			else
				trace("(R%o)+ ", r);
			break;
		default:
			fprintf(stderr, "Mode %o NOT IMPLEMENTED YET!\n", m);
			exit(1);
	}
	return res;
}

void do_halt() {
	trace("r0=%06o   r1=%06o   r2=%06o   r3=%06o\nr4=%06o   r5=%06o   sp=%06o   pc=%06o\n",
		reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
	trace("THE END\n");
	exit(0);
}

void do_mov(Par p) {
	trace("mov");
	w_write(p.dd.adr, p.ss.val);
	reg[p.dd.adr] = p.ss.val;
}

void do_add(Par p) {
	trace("add");
	word w = w_read(p.dd.adr);
	w_write(p.dd.adr, w + p.ss.val);
	reg[p.dd.adr] = w + p.ss.val;
}

void do_nothing() {}

Command cmd[] = {
	{0170000, 0010000, "mov", HAS_DD | HAS_SS, do_mov},
	{0170000, 0060000, "add", HAS_DD | HAS_SS, do_add},
	{0177777, 0000000, "halt", NO_PARAMS, do_halt},
	{0000000, 0000000, "unknown", NO_PARAMS, do_nothing}
};

void run() {
	pc = 01000;
	while (1) {
		word w = w_read(pc);
		trace("%06o %06o: ", pc, w);
		pc += 2;
		int i = 0;
		while (1) {
			if ((w & cmd[i].mask) == cmd[i].opcode) {
				p.ss = get_mr(w >> 6);
				p.dd = get_mr(w);
				cmd[i].do_func(p);
				break;
			}
			i++;
		}
	}
}
