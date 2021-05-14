#include <stdio.h>
#include <stdlib.h>
#include "pdp11.h"

word reg[8]; // регистры R0 .. R7

#define NO_PARAMS 0
#define HAS_DD 1
#define HAS_SS 2
#define HAS_R 4
#define HAS_NN 8
extern char * x;
static Par p;
static word is_byte;

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
			if (is_byte)
				res.val = b_read(res.adr);
			else
				res.val = w_read(res.adr);
			trace("(R%o) ", r);
			break;
		case 2:				//мода 2 (R3)+	#3
			res.adr = reg[r];
			if (is_byte) {
				res.val = b_read(res.adr);
				reg[r]++;
			} else {
				res.val = w_read(res.adr);
				reg[r] += 2;
			}
			if (r == 7)
				trace("(#%o) ", res.val);
			else
				trace("(R%o)+ ", r);
			break;
		case 3:				//мода 3 @(R3)+
			res.adr = reg[r];
			if (is_byte) {
				res.adr = b_read(res.adr);
				res.val = b_read(res.adr);
				reg[r]++;
			}
			else {
				res.adr = w_read(res.adr);
				res.val = w_read(res.adr);
				reg[r] += 2;
			}
			if (r == 7)
				trace("@#%o", res.val);
			else
				trace("@(R%o)+", r);
			break;
		case 4:
			if (is_byte) {
				reg[r]--;
				res.adr = reg[r];
				res.val = b_read(res.adr);
			}
			else {
				reg[r] -= 2;
				res.adr = reg[r];
				res.val = w_read(res.adr);
			}
			if (r == 7)
				trace("(-#%o) ", res.val);
			else
				trace("-(R%o) ", r);
			break;
		case 5:
			if (is_byte) {
				reg[r]--;
				res.adr = reg[r];
				res.adr = b_read(res.adr);
				res.val = b_read(res.adr);
			}
			else {
				reg[r] -= 2;
				res.adr = reg[r];
				res.adr = w_read(res.adr);
				res.val = w_read(res.adr);
			}
			if (r == 7)
				trace("@-#%o", res.val);
			else
				trace("@-(R%o)", r);
			break;
		case 6:
			reg[7] += 2;
			res.adr = 2 * reg[7] - 2;
			res.val = w_read(res.adr);
			break;
		case 7:
			reg[7] += 2;
			res.adr = 2 * reg[7] - 2;
			res.adr = w_read(res.adr);
			res.val = w_read(res.adr);
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
	if (p.dd.adr < 8) {
		reg[p.dd.adr] = p.ss.val;
	}
}

void do_add(Par p) {
	trace("add");
	word w = w_read(p.dd.adr);
	w_write(p.dd.adr, w + p.ss.val);
	if (p.dd.adr < 8) {
		reg[p.dd.adr] = w + p.ss.val;
	}
}

void do_sob(Par p) {
	trace("sob");
	reg[p.r]--;
	if (reg[p.r] != 0) {
		pc = pc - 2 * p.nn;
	}
}

void do_clr(Par p) {
	trace("clr");
	w_write(p.dd.adr, 0);
	if (p.dd.adr < 8) {
		reg[p.dd.adr] = 0;
	}
}

void do_nothing() {}

Command cmd[] = {
	{0170000, 0010000, HAS_DD | HAS_SS, do_mov},
	{0170000, 0060000, HAS_DD | HAS_SS, do_add},
	{0177700, 0005000, HAS_DD, do_clr},
	{0177000, 0077000, HAS_R | HAS_NN, do_sob},
	{0177777, 0000000, NO_PARAMS, do_halt},
	{0000000, 0000000, NO_PARAMS, do_nothing}
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
				if ((cmd[i].param & HAS_SS) == HAS_SS) // xxxx & 0011 == 0011?
					p.ss = get_mr(w >> 6);
				if ((cmd[i].param & HAS_DD) == HAS_DD) // xxxx & 0001 == 0001?
					p.dd = get_mr(w);
				if ((cmd[i].param & HAS_R) == HAS_R) // xxxx & 0111 == 0111?
					p.r = (w >> 6) & 1;
				if ((cmd[i].param & HAS_NN) == HAS_NN) // xxxx & 1111 == 1111?
					p.nn = w & 077;
				cmd[i].do_func(p);
				break;
			}
			i++;
		}
	}
}
