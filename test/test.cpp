#include "../quickprof.h"

double randomRealUniform(double min, double max)
{
	return (double)rand() / RAND_MAX * (max - min) + min;
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));

	//hidden::Clock c;
	//for (int i = 0; i < 100; ++i)
	//{
	//	std::cout << "Time " << i << ": " << c.getTimeMicroseconds() * 0.000001 << std::endl;
	//	Sleep(1000);
	//}

	Profiler::init(true);

	for (int i = 0; i < 50; ++i)
	{
		Profiler::beginBlock("test");
		Sleep(1000 + (int)randomRealUniform(-500, 500));
		Profiler::endBlock("test");
		Sleep(1000 + (int)randomRealUniform(-500, 500));

		std::cout << "Iteration " << i << ": " 
			<< Profiler::getBlockTime("test", Profiler::BLOCK_TOTAL_SECONDS) << " s, " 
			<< Profiler::getBlockTime("test", Profiler::BLOCK_TOTAL_MILLISECONDS) << " ms, " 
			<< Profiler::getBlockTime("test", Profiler::BLOCK_TOTAL_MICROSECONDS) << " us, " 
			<< Profiler::getBlockTime("test", Profiler::BLOCK_TOTAL_PERCENT) << "%" 
			<< std::endl;
	}

	return 0;
}
