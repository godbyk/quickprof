/************************************************************************
* QuickProf                                                             *
* http://quickprof.sourceforge.net                                      *
* Copyright (C) 2006-2008                                               *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
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

#include <cstdlib>

int randomIntUniform(int min, int max)
{
	// Note: rand() isn't a very good generator, but it's good enough for 
	// simple tasks that only require a few thousand values.

	int range = max - min;
	int randValue = std::rand();
	int randMaxValue = RAND_MAX;
	while (randMaxValue < range)
	{
		// Increase the random value range until we reach the desired 
		// range.
		randValue = randValue * RAND_MAX;
		randMaxValue = randMaxValue * RAND_MAX;
		if (randValue < randMaxValue)
		{
			randValue += rand();
		}
	}

	// Get a uniform random real value from [0, 1).
	double zeroToOneHalfOpen = randValue / (randMaxValue + 1.0);

	// The [0, 1) value times (max - min + 1) gives each integer within 
	// [0, max - min] an equal chance of being chosen.
	return min + int((range + 1) * zeroToOneHalfOpen);
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
