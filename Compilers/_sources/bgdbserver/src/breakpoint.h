#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

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


#include <exec/types.h>

struct Breakpoint;

void addBreakpoint(UWORD * addr, short isTemp, UWORD * restore);
void delBreakpoint(UWORD * addr);

void enableBreakpoints();
void disableBreakpoints(void);

void clearBreakpoints();

#endif // __BREAKPOINT_H__
