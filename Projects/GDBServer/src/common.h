#ifndef __COMMON_H_
#define __COMMON_H_

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

#include <proto/exec.h>
#include <inline/exec.h>

#ifdef DEBUG
#define dprintf Printf
#else
#define dprintf
#endif

/**
 * The states to handle in the trap handler.
 *
 * CONTINUE: run until a trap is hit or program exits
 * PARK: continue at park function
 * STEP_BEFORE_CONTINUE: step one insn, set action to CONTINUE
 * STEP: step one insn.
 *
 * both step actions do: action -= 2;
 *
 */
enum action { CONTINUE = -1, PARK = 0, STEP_BEFORE_CONTINUE = 1, STEP = 2};


/**
 * an extended message to transfer all necessary information from the debugged
 * process to the debugger.
 */
struct PcMessage {
	struct Message msg;
};
// The message sent from debugged program to debug server and back.
extern struct PcMessage msg;
// plus the data to control the action
extern enum action action;
extern UWORD *newpc;


struct RegInfo {
	ULONG regs[15]; // d0-d7,a0-a6
	ULONG usp;
	UWORD sr;
	UWORD dummy;
	UWORD * pc;
};

extern struct RegInfo reginfo;

// needed to start and stop the debugged process.
__regargs void startProc(int cmdlen, void * cmdline);
void endProc(void);

#define trapInsn insn[0];
extern UWORD insn[2];

#endif
