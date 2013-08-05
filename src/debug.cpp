/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

/*
	Debug output
*/

FILE *g_debugstreams[DEBUGSTREAM_COUNT] = {stderr, NULL};

void debugstreams_init(bool disable_stderr, const char *filename)
{
	if(disable_stderr)
		g_debugstreams[0] = NULL;
	else
		g_debugstreams[0] = stderr;

	if(filename)
		g_debugstreams[1] = fopen(filename, "a");
		
	if(g_debugstreams[1])
	{
		fprintf(g_debugstreams[1], "\n\n-------------\n");
		fprintf(g_debugstreams[1],     "  Separator  \n");
		fprintf(g_debugstreams[1],     "-------------\n\n");
	}
}

void debugstreams_deinit()
{
	if(g_debugstreams[1] != NULL)
		fclose(g_debugstreams[1]);
}

Debugbuf debugbuf(false);
std::ostream dstream(&debugbuf);
Debugbuf debugbuf_no_stderr(true);
std::ostream dstream_no_stderr(&debugbuf_no_stderr);
Nullstream dummyout;

/*
	Assert
*/

void assert_fail(const char *assertion, const char *file,
		unsigned int line, const char *function)
{
	DEBUGPRINT("\nIn thread %lx:\n"
			"%s:%d: %s: Assertion '%s' failed.\n",
			(unsigned long)get_current_thread_id(),
			file, line, function, assertion);
	
	if(g_debugstreams[1])
		fclose(g_debugstreams[1]);

	abort();
}

#ifdef _MSC_VER
#if CATCH_UNHANDLED_EXCEPTIONS == 1
void se_trans_func(unsigned int u, EXCEPTION_POINTERS* pExp)
{
	dstream<<"In trans_func.\n";
	if(u == EXCEPTION_ACCESS_VIOLATION)
	{
		PEXCEPTION_RECORD r = pExp->ExceptionRecord;
		dstream<<"Access violation at "<<r->ExceptionAddress
				<<" write?="<<r->ExceptionInformation[0]
				<<" address="<<r->ExceptionInformation[1]
				<<std::endl;
		throw FatalSystemException
		("Access violation");
	}
	if(u == EXCEPTION_STACK_OVERFLOW)
	{
		throw FatalSystemException
		("Stack overflow");
	}
	if(u == EXCEPTION_ILLEGAL_INSTRUCTION)
	{
		throw FatalSystemException
		("Illegal instruction");
	}
}
#endif
#endif

