#include "../quickprof.h"

int randomIntUniform(int min, int max)
{
	return int((max - min + 1) * rand() / (RAND_MAX + 1.0)) + min;
}

void approxDelay(int milliseconds)
{
	int spread = (int)(0.15 * (double)milliseconds);
#ifdef WIN32
		::Sleep(milliseconds + randomIntUniform(-spread, spread));
#else
		usleep(milliseconds + randomIntUniform(-spread, spread));
#endif
}

int main(int argc, char* argv[])
{
	// Seed the random number generator for the 'randomIntUniform' function.
	srand((unsigned int)time(NULL));

	hidden::Clock c;
	for (int i = 0; i < 10; ++i)
	{
		std::cout << "Time " << i << ": " << c.getTimeMicroseconds() * 0.000001 << std::endl;
#ifdef WIN32
		::Sleep(1000);
#else
		usleep(1000);
#endif
	}

	Profiler::init(true, "results.dat", Profiler::BLOCK_CYCLE_PERCENT);

	for (int i = 0; i < 51; ++i)
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

		Profiler::startProfilingCycle();
	}

	// Print the overall averages.
	std::cout << Profiler::createStatsString(Profiler::BLOCK_TOTAL_PERCENT) << std::endl;

	Profiler::destroy();

	return 0;
}
