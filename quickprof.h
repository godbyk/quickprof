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

// Please visit the project website (http://quickprof.sourceforge.net) 
// for usage instructions.

// Credits: The Clock class was inspired by the Timer classes in 
// Ogre (www.ogre3d.org).

#ifndef QUICK_PROF_H
#define QUICK_PROF_H

#include <iostream>
#include <string>
#include <map>
#include <assert.h>

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
			currentBlockStartMicroseconds = 0;
			currentCycleTotalMicroseconds = 0;
			lastCycleTotalMicroseconds = 0;
			totalMicroseconds = 0;
		}

		/// The starting time (in us) of the current block update.
		unsigned long int currentBlockStartMicroseconds;

		/// The accumulated time (in us) spent in this block during the 
		/// current profiling cycle.
		unsigned long int currentCycleTotalMicroseconds;

		/// The accumulated time (in us) spent in this block during the 
		/// past profiling cycle.
		unsigned long int lastCycleTotalMicroseconds;

		/// The total accumulated time (in us) spent in this block.
		unsigned long int totalMicroseconds;
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
	/// A set of ways to retrieve block timing data.
	enum BlockTimingMethod
	{
		/// The total time spent in the block (in seconds) since the 
		/// profiler was initialized.
		BLOCK_TOTAL_SECONDS,

		/// The total time spent in the block (in ms) since the 
		/// profiler was initialized.
		BLOCK_TOTAL_MILLISECONDS,

		/// The total time spent in the block (in us) since the 
		/// profiler was initialized.
		BLOCK_TOTAL_MICROSECONDS,

		/// The total time spent in the block, as a % of the total 
		/// elapsed time since the profiler was initialized.
		BLOCK_TOTAL_PERCENT,

		/// The time spent in the block (in seconds) in the most recent 
		/// profiling cycle.
		BLOCK_LAST_CYCLE_SECONDS,

		/// The time spent in the block (in ms) in the most recent 
		/// profiling cycle.
		BLOCK_LAST_CYCLE_MILLISECONDS,

		/// The time spent in the block (in us) in the most recent 
		/// profiling cycle.
		BLOCK_LAST_CYCLE_MICROSECONDS,

		/// The time spent in the block (in seconds) in the most recent 
		/// profiling cycle, as a % of the total cycle time.
		BLOCK_LAST_CYCLE_PERCENT
	};

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

	/// Returns the time spent in the named block according to the 
	/// given timing method.  See comments on BlockTimingMethod for details.
	static double getBlockTime(const std::string& name, 
		BlockTimingMethod method);

	/// Defines the beginning of a new profiling cycle.  Use this 
	/// regularly if you want to generate detailed timing information.  
	/// This should not be called within a timing block.
	static void startProfilingCycle();

private:
	Profiler();

	~Profiler();

	/// Prints an error message to standard output.
	static void printError(const std::string& msg)
	{
		std::cout << "[QuickProf error] " << msg << std::endl;
	}

	/// Determines whether the profiler is enabled.
	static bool mEnabled;

	/// The clock used to time profile blocks.
	static hidden::Clock mClock;

	/// The starting time (in us) of the current profiling cycle.
	static unsigned long int mCurrentCycleStartMicroseconds;

	/// The duration (in us) of the most recent profiling cycle.
	static unsigned long int mLastCycleDurationMicroseconds;

	/// Internal map of named profile blocks.
	static std::map<std::string, hidden::ProfileBlock*> mProfileBlocks;
};

// Note: We must declare these private static variables again here to 
// avoid link errors.
bool Profiler::mEnabled;
hidden::Clock Profiler::mClock;
unsigned long int Profiler::mCurrentCycleStartMicroseconds;
unsigned long int Profiler::mLastCycleDurationMicroseconds;
std::map<std::string, hidden::ProfileBlock*> Profiler::mProfileBlocks;

Profiler::Profiler()
{
	mEnabled = false;
	mCurrentCycleStartMicroseconds = 0;
	mLastCycleDurationMicroseconds = 0;
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
	block->currentBlockStartMicroseconds = mClock.getTimeMicroseconds();
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

	unsigned long int blockDuration = endTick - 
		block->currentBlockStartMicroseconds;
	block->currentCycleTotalMicroseconds += blockDuration;
	block->totalMicroseconds += blockDuration;
}

double Profiler::getBlockTime(const std::string& name, 
	BlockTimingMethod method)
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

	double result = 0;

	switch(method)
	{
		case BLOCK_TOTAL_SECONDS:
			result = block->totalMicroseconds / 1000000;
			break;
		case BLOCK_TOTAL_MILLISECONDS:
			result = block->totalMicroseconds / 1000;
			break;
		case BLOCK_TOTAL_MICROSECONDS:
			result = block->totalMicroseconds;
			break;
		case BLOCK_TOTAL_PERCENT:
		{
			double timeSinceInit = (double)mClock.getTimeMicroseconds();
			if (0 == timeSinceInit)
			{
				// No time has elapsed.  Avoid a divide by zero error.
				result = 0;
			}
			else
			{
				result = 100 * (double)block->totalMicroseconds / 
					timeSinceInit;
			}
			break;
		}
		case BLOCK_LAST_CYCLE_SECONDS:
			result = block->lastCycleTotalMicroseconds / 1000000;
			break;
		case BLOCK_LAST_CYCLE_MILLISECONDS:
			result = block->lastCycleTotalMicroseconds / 1000;
			break;
		case BLOCK_LAST_CYCLE_MICROSECONDS:
			result = block->lastCycleTotalMicroseconds;
			break;
		case BLOCK_LAST_CYCLE_PERCENT:
		{
			if (0 == mLastCycleDurationMicroseconds)
			{
				// No time has elapsed.  Avoid a divide by zero error.
				result = 0;
			}
			else
			{
				result = 100 * (double)block->lastCycleTotalMicroseconds / 
					mLastCycleDurationMicroseconds;
			}
			break;
		}
		default:
			assert(false);
	}

	return result;
}

void Profiler::startProfilingCycle()
{
	// Store the duration of the cycle that just finished.
	if (0 == mCurrentCycleStartMicroseconds)
	{
		// Do things differently on the first time.
		mLastCycleDurationMicroseconds = 0;
	}
	else
	{
		mLastCycleDurationMicroseconds = mClock.getTimeMicroseconds() - 
			mCurrentCycleStartMicroseconds;
	}

	// For each block, update data for the cycle that just finished.
	std::map<std::string, hidden::ProfileBlock*>::iterator iter;
	for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end(); ++iter)
	{
		hidden::ProfileBlock* block = (*iter).second;
		block->lastCycleTotalMicroseconds = 
			block->currentCycleTotalMicroseconds;
		block->currentCycleTotalMicroseconds = 0;
	}

	mCurrentCycleStartMicroseconds = mClock.getTimeMicroseconds();
}

#endif
