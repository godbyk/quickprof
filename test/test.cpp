/************************************************************************
* QuickProf                                                             *
* Copyright (C) 2006-2007                                               *
* Tyler Streeter  http://www.tylerstreeter.net  tylerstreeter@gmail.com *
* All rights reserved.                                                  *
* Web: http://quickprof.sourceforge.net                                 *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*   (3) The zlib/libpng license that is included with this library in   *
*       the file license-zlib-libpng.txt.                               *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt, license-BSD.txt, and license-zlib-libpng.txt for    *
* more details.                                                         *
************************************************************************/

#include "../quickprof.h"

int randomIntUniform(int min, int max)
{
	double fraction = (double)rand() / (double)(RAND_MAX + 1.0);
	return (int)((double)(max - min + 1) * fraction) + min;
}

void approxDelay(int milliseconds)
{
	int spread = (int)(0.15 * (double)milliseconds);
	int delay = milliseconds + randomIntUniform(-spread, spread);
#ifdef WIN32
	::Sleep(delay);
#else
	usleep(1000 * delay);
#endif
}

int main(int argc, char* argv[])
{
	// Seed the random number generator for the 'randomIntUniform' function.
	srand((unsigned int)time(NULL));

	// To disable profiling, simply comment out the following line.
	PROFILER.init(3, "results.dat", 1, quickprof::MILLISECONDS);

	for (int i = 0; i < 30; ++i)
	{
		// Note the nested block arrangement here...

		PROFILER.beginBlock("blocks1and2");

		PROFILER.beginBlock("block1");
		approxDelay(100);
		PROFILER.endBlock("block1");

		PROFILER.beginBlock("block2");
		approxDelay(200);
		PROFILER.endBlock("block2");

		PROFILER.endBlock("blocks1and2");

		PROFILER.beginBlock("block3");
		approxDelay(150);
		PROFILER.endBlock("block3");

		// Non-profiled code.
		approxDelay(50);

		PROFILER.endCycle();
	}

	// Print the overall averages.
	std::cout << PROFILER.getSummary(quickprof::PERCENT) << std::endl;

	return 0;
}
