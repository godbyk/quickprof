/************************************************************************
* QuickProf                                                             *
* Copyright (C) 2006                                                    *
* Tyler Streeter  tylerstreeter@gmail.com                               *
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
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
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
	std::cout << "spread: " << spread << ", delay: " << delay << std::endl;
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
	Profiler::init("results.dat", Profiler::BLOCK_CYCLE_SECONDS);

	for (int i = 0; i < 5; ++i)
	{
		std::cout << "random: " << randomIntUniform(-14, 14) << std::endl;
	}
	return 0;

	for (int i = 0; i < 31; ++i)
	{
		Profiler::beginBlock("block1");
		approxDelay(100);
		Profiler::endBlock("block1");

		Profiler::beginBlock("block2");
		approxDelay(200);
		Profiler::endBlock("block2");

		Profiler::beginBlock("block3");
		approxDelay(300);
		Profiler::endBlock("block3");

		// Non-profiled code.
		approxDelay(400);

		Profiler::endProfilingCycle();
	}

	// Print the overall averages.
	std::cout << Profiler::createStatsString(Profiler::BLOCK_TOTAL_PERCENT) 
		<< std::endl;

	Profiler::destroy();

	return 0;
}
