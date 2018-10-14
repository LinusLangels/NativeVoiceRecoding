#pragma once
#ifndef __SharedProjectHeader_H__
#define __SharedProjectHeader_H__

struct PhonemeData
{
	float start;
	float stop;
	const char* phoneme;
};

typedef void(*SimpleCallback)();
typedef void(*StringParameterCallback)(const char*);
typedef void(*PhonemeParameterCallback) (struct PhonemeData *);
typedef void(*CalibrationCallback) (float, float);

#define BUILD_WINDOWS
//#define BUILD_OSX
//#define BUILD_IOS

// Include any system framework and library headers here that should be included in all compilation units.
// You will also need to set the Prefix Header build setting of one or more of your targets to reference this file.

#endif /* SharedProjectHeader_H */
