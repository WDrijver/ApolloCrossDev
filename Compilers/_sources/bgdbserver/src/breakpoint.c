/*
Copyright (C) 2018-2019 by Stefan "Bebbo" Franke <stefan@franke.ms> 

This file is part of bgdbserver.

bgdbserver is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

bgdbserver is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with bgdbserver.  If not, see <http://www.gnu.org/licenses/>.
*/  

#include "common.h"
#include "breakpoint.h"


struct Breakpoint {
	UWORD * address;
	USHORT value;
	short tmp; // temporary breakpoint
	UWORD * restore; //address where the bp should be.
};


struct Breakpoint * bps;
short numBps;
short maxBps;
short bpEnabled;

void clearBreakpoints() {
	numBps = 0;
}

/**
 * find a breakpoint.
 */
static short findBp(UWORD * addr) {
	for (short i = 0; i < numBps; ++i) {
		if (bps[i].address == addr)
			return i;
	}
	return -1;
}

/**
 * add a new breakpoint.
 */
void addBreakpoint(UWORD * addr, short isTmp, UWORD * restore) {
	if (findBp(addr) >= 0)
		return;

	if (numBps == maxBps) {
		maxBps = maxBps + maxBps + 4;
		bps = (struct Breakpoint *)realloc(bps, maxBps * sizeof(struct Breakpoint));
	}
	struct Breakpoint * b = & bps[numBps++];
	b->address = addr;
	b->tmp = isTmp;
	b->restore = restore;
}

/**
 * delete a breakpoint.
 */
void delBreakpoint(UWORD * addr) {
	short index = findBp(addr);
	if (index < 0)
		return;

	--numBps;
	if (index != numBps) {
		bps[index] = bps[numBps];
	}
}

/**
 * Enable the breakpoints.
 */
void enableBreakpoints() {
	bpEnabled = 1;
	for (int i = 0; i < numBps; ++i) {
		bps[i].value = *bps[i].address;
		*bps[i].address = trapInsn;

//		printf("set bp %08x %04x -> %04x\n", bps[i].address, bps[i].value, *bps[i].address);
	}
	CacheClearU();
}

/**
 * Disable all breakpoints.
 */
void disableBreakpoints() {
	if (!bpEnabled)
		return;

	bpEnabled = 0;
	// restore the code
	for (int i = 0; i < numBps; ++i) {
		*bps[i].address = bps[i].value;

//		printf("clear bp %08x %04x -> %04x\n", bps[i].address, bps[i].value, *bps[i].address);

		// fixup pc to original insn.
		if (reginfo.pc == bps[i].address + 1) {
			--reginfo.pc;
		}

		if (bps[i].tmp) {
			delBreakpoint(bps[i].address);
			--i;
		}
	}
	CacheClearU();
}
