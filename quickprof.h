// Author: Tyler Streeter
// Written: 2-16-06

// To use the profiler, call its functions like this: Profiler::init(...), 
// Profiler::beginBlock(...), Profiler::endBlock(...), etc.

// Credits: The Clock class was inspired by the Timer classes in 
// Ogre (www.ogre3d.org).

#ifndef CPP_PERF_H
#define CPP_PERF_H

#include <time.h>
#include <iostream>
#include <string>
#include <map>

// Only VC++ and GCC are supported compilers.
// TODO: is this restriction necessary?
#if defined(WIN32) || defined(_WIN32)
	#define USE_WINDOWS_TIMERS
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif

namespace hidden
{
	/// A simple data structure representing a single timed block 
	/// of code.
	struct ProfileBlock
	{
		ProfileBlock()
		{
			startTickMicroseconds = 0;
			totalTimeMicroseconds = 0;
		}

		/// The starting tick (in us) of the latest profiling update.
		unsigned long int startTickMicroseconds;

		/// The total accumulated time (in us) spent in this block.
		unsigned long int totalTimeMicroseconds;
	};

	class Clock
	{
	public:
		Clock()
		{
#ifdef USE_WINDOWS_TIMERS
			QueryPerformanceFrequency(&mClockFrequency);
#endif
			reset();
		}

		~Clock()
		{
		}

		/// Resets the initial reference time.
		void reset()
		{
#ifdef USE_WINDOWS_TIMERS
			QueryPerformanceCounter(&mStartTime);
			mStartTick = GetTickCount();
			mPrevElapsedTime = 0;
#else
			gettimeofday(&mStartTime, NULL);
#endif
		}

		/// Returns the time in ms since the last call to reset or since 
		/// the Clock was created.
		unsigned long int getTimeMilliseconds()
		{
#ifdef USE_WINDOWS_TIMERS
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			LONGLONG elapsedTime = currentTime.QuadPart - 
				mStartTime.QuadPart;

			// Compute the number of millisecond ticks elapsed.
			unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
				mClockFrequency.QuadPart);

			// Check for unexpected leaps in the Win32 performance counter.  
			// (This is caused by unexpected data across the PCI to ISA 
			// bridge, aka south bridge.  See Microsoft KB274323.)
			unsigned long elapsedTicks = GetTickCount() - mStartTick;
			signed long msecOff = (signed long)(msecTicks - elapsedTicks);
			if (msecOff < -100 || msecOff > 100)
			{
				// Adjust the starting time forwards.
				LONGLONG msecAdjustment = (std::min)(msecOff * 
					mClockFrequency.QuadPart / 1000, elapsedTime - 
					mPrevElapsedTime);
				mStartTime.QuadPart += msecAdjustment;
				elapsedTime -= msecAdjustment;

				// Recompute the number of millisecond ticks elapsed.
				msecTicks = (unsigned long)(1000 * elapsedTime / 
					mClockFrequency.QuadPart);
			}

			// Store the current elapsed time for adjustments next time.
			mPrevElapsedTime = elapsedTime;

			return msecTicks;
#else
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			return (currentTime.tv_sec - mStartTime.tv_sec) * 1000 + 
				(currentTime.tv_usec - mStartTime.tv_usec) * 0.001;
#endif
		}

		/// Returns the time in us since the last call to reset or since 
		/// the Clock was created.
		unsigned long int getTimeMicroseconds()
		{
#ifdef USE_WINDOWS_TIMERS
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			LONGLONG elapsedTime = currentTime.QuadPart - 
				mStartTime.QuadPart;

			// Compute the number of millisecond ticks elapsed.
			unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
				mClockFrequency.QuadPart);

			// Check for unexpected leaps in the Win32 performance counter.  
			// (This is caused by unexpected data across the PCI to ISA 
			// bridge, aka south bridge.  See Microsoft KB274323.)
			unsigned long elapsedTicks = GetTickCount() - mStartTick;
			signed long msecOff = (signed long)(msecTicks - elapsedTicks);
			if (msecOff < -100 || msecOff > 100)
			{
				// Adjust the starting time forwards.
				LONGLONG msecAdjustment = (std::min)(msecOff * 
					mClockFrequency.QuadPart / 1000, elapsedTime - 
					mPrevElapsedTime);
				mStartTime.QuadPart += msecAdjustment;
				elapsedTime -= msecAdjustment;
			}

			// Store the current elapsed time for adjustments next time.
			mPrevElapsedTime = elapsedTime;

			// Convert to microseconds.
			unsigned long usecTicks = (unsigned long)(1000000 * elapsedTime / 
				mClockFrequency.QuadPart);

			return usecTicks;
#else
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			return (currentTime.tv_sec - mStartTime.tv_sec) * 1000000 + 
				(currentTime.tv_usec - mStartTime.tv_usec);
#endif
		}

	private:
#ifdef USE_WINDOWS_TIMERS
		LARGE_INTEGER mClockFrequency;
		DWORD mStartTick;
		LONGLONG mPrevElapsedTime;
		LARGE_INTEGER mStartTime;
#else
		struct timeval mStartTime;
#endif
	};
};

/// A static class that manages timing for a set of profiling blocks.
class Profiler
{
public:
	/// Initializes the profiler.  This must be called first.  The first 
	/// parameter specifies whether the profiler is enabled; if disabled, 
	/// all other functions will return immediately.
	static void init(bool enabled);

	/// Begins timing the named block of code.
	static void beginBlock(const std::string& name);

	/// Updates the accumulated time spent in the named block by adding 
	/// the elapsed time since the last call to startBlock for this block 
	/// name.
	static void endBlock(const std::string& name);

	/// Returns the accumulated time spent (in us) in the named block 
	/// since the Profiler was initialized.
	static unsigned long int getBlockTimeMicroseconds(
		const std::string& name);

	/// Returns the accumulated time spent (in ms) in the named block 
	/// since the Profiler was initialized.
	static unsigned long int getBlockTimeMilliseconds(
		const std::string& name);

	/// Returns the accumulated time spent in the named block as a 
	/// percentage of the total elapsed time since the Profiler was 
	/// initialized.
	static double getBlockTimePercent(const std::string& name);

private:
	Profiler();

	~Profiler();

	/// Prints an error message to standard output.
	static void printError(const std::string& msg)
	{
		std::cout << "[cpp-prof error] " << msg << std::endl;
	}

	/// Determines whether the profiler is enabled.
	static bool mEnabled;

	/// The clock used to time profile blocks.
	static hidden::Clock mClock;

	/// Internal map of named profile blocks.
	static std::map<std::string, hidden::ProfileBlock*> mProfileBlocks;
};

// Note: We must declare these private static variables again here to 
// avoid link errors.
bool Profiler::mEnabled;
hidden::Clock Profiler::mClock;
std::map<std::string, hidden::ProfileBlock*> Profiler::mProfileBlocks;

Profiler::Profiler()
{
	mEnabled = false;
}

Profiler::~Profiler()
{
	// Destroy all ProfileBlocks.
	while (!mProfileBlocks.empty())
	{
		delete (*mProfileBlocks.begin()).second;
	}
	mProfileBlocks.clear();
}

void Profiler::init(bool enabled)
{
	mEnabled = enabled;
	mClock.reset();
}

void Profiler::beginBlock(const std::string& name)
{
	if (!mEnabled)
	{
		return;
	}

	if (name.empty())
	{
		printError("Cannot allow unnamed profile blocks.");
		return;
	}

	hidden::ProfileBlock* block = mProfileBlocks[name];

	if (!block)
	{
		// Create a new ProfileBlock.
		mProfileBlocks[name] = new hidden::ProfileBlock();
		block = mProfileBlocks[name];
	}

	// We do this at the end to get more accurate results.
	block->startTickMicroseconds = mClock.getTimeMicroseconds();
}

void Profiler::endBlock(const std::string& name)
{
	if (!mEnabled)
	{
		return;
	}

	// We do this at the beginning to get more accurate results.
	unsigned long int endTick = mClock.getTimeMicroseconds();

	hidden::ProfileBlock* block = mProfileBlocks[name];

	if (!block)
	{
		// The named block does not exist.  Print an error.
		printError("The profile block named '" + name + 
			"' does not exist.");
		return;
	}

	block->totalTimeMicroseconds += endTick - block->startTickMicroseconds;
}

unsigned long int Profiler::getBlockTimeMicroseconds(const std::string& name)
{
	if (!mEnabled)
	{
		return 0;
	}

	hidden::ProfileBlock* block = mProfileBlocks[name];

	if (!block)
	{
		// The named block does not exist.  Print an error.
		printError("The profile block named '" + name + 
			"' does not exist.");
		return 0;
	}

	return block->totalTimeMicroseconds;
}

unsigned long int Profiler::getBlockTimeMilliseconds(const std::string& name)
{
	return (getBlockTimeMicroseconds(name) / 1000);
}

double Profiler::getBlockTimePercent(const std::string& name)
{
	return (100 * (double)getBlockTimeMicroseconds(name) / 
		(double)mClock.getTimeMicroseconds());
}

#endif
