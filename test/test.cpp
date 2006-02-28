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
	return int((max - min + 1) * rand() / (RAND_MAX + 1.0)) + min;
}

hidden::Clock c;

void approxDelay(int milliseconds)
{
	int spread = (int)(0.15 * (double)milliseconds);
	int delay = milliseconds + randomIntUniform(-spread, spread);
	std::cout << "delay: " << delay << " ms" << std::endl;
	std::cout << "Start time: " << c.getTimeMicroseconds() << " us, ";
#ifdef WIN32
	::Sleep(delay);
#else
	usleep(1000 * delay);
#endif
	std::cout << "End time: " << c.getTimeMicroseconds() << " us" << std::endl;
}

int main(int argc, char* argv[])
{
	// Seed the random number generator for the 'randomIntUniform' function.
	srand((unsigned int)time(NULL));

	// To disable profiling, simply comment out the following line.
	Profiler::init("results.dat", Profiler::BLOCK_CYCLE_SECONDS);

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
