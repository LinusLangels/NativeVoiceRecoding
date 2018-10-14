#pragma once

#ifdef MICROPHONE_EXPORT
#define MICROPHONE_API __declspec(dllexport) 
#else
#define MICROPHONE_API __declspec(dllimport) 
#endif

#include "SharedProjectHeader.h"

extern "C"
{
	MICROPHONE_API void SetDebugLog(StringParameterCallback functionDelegate);

	MICROPHONE_API void SetPhonemeCallback(PhonemeParameterCallback functionDelegate);

	MICROPHONE_API void SetCalibrationCallback(CalibrationCallback functionDelegate);

	MICROPHONE_API bool Bridge_IsMicrophoneAvailible();

	MICROPHONE_API void Bridge_InitRecording();

	MICROPHONE_API void Bridge_StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate);

	MICROPHONE_API void Bridge_StopRecording(int algoNumber);

	MICROPHONE_API void Bridge_Destroy();

	MICROPHONE_API void Bridge_TrimRecording(const char* path, float startTime, float stopTime);

	MICROPHONE_API void Bridge_CalulcatePhonemesFromWav(const char* path);

	MICROPHONE_API void DestroyCallbacks();
}