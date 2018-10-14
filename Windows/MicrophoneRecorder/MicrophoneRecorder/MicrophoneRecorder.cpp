// MicrophoneRecorder.cpp : Defines the exported functions for the DLL application.
//

#include "MicrophoneRecorder.h"
#include "WindowsMicrophone.h"
#include "../../../SharedSource/WavPostProcessor.h"
#include "../../../SharedSource/WavPhonemeProcessor.h"

static StringParameterCallback debugLog;
static PhonemeParameterCallback phonemeCallback;
static CalibrationCallback calibrationCallback;
static WindowsMicrophone *microphoneInstance;

extern "C"
{
	void SetDebugLog(StringParameterCallback functionDelegate)
	{
		debugLog = functionDelegate;
	}

	void SetPhonemeCallback(PhonemeParameterCallback functionDelegate)
	{
		phonemeCallback = functionDelegate;
	}

	void SetCalibrationCallback(CalibrationCallback functionDelegate)
	{
		calibrationCallback = functionDelegate;
	}

	bool Bridge_IsMicrophoneAvailible()
	{
		WindowsMicrophone *microphone = new WindowsMicrophone(debugLog, phonemeCallback, calibrationCallback);
		bool availible = microphone->IsMicrophoneAvailible();
		delete microphone;

		return availible;
	}

    void Bridge_InitRecording()
    {
		//Safety thing, incase we have a weird state we will clean up previous instance.
		Bridge_Destroy();

        //Create new microphone and give it references to callbacks from c#.
        microphoneInstance = new WindowsMicrophone(debugLog, phonemeCallback, calibrationCallback);
        
        microphoneInstance->InitializeRecording();
    }

	void Bridge_StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate)
	{
		microphoneInstance->StartRecording(complexity, path, algoNumber, calibrationFactor, calibrate);
	}

	void Bridge_StopRecording(int algoNumber)
	{
		microphoneInstance->StopRecording(algoNumber);
	}

	void Bridge_Destroy()
	{
		if (microphoneInstance != nullptr)
		{
			//delete instance.
			delete microphoneInstance;
			microphoneInstance = nullptr;
		}
	}

	void Bridge_TrimRecording(const char* path, float startTime, float stopTime)
	{
		WavPostProcessor *processor = new WavPostProcessor(path, startTime, stopTime);
		processor->setLogger(debugLog);
		processor->trimWavFile();

		delete processor;
	}

	void Bridge_CalulcatePhonemesFromWav(const char* path)
	{
		WavPhonemeProcessor *processor = new WavPhonemeProcessor(path);
		processor->setPhonemeLogger(phonemeCallback);
		processor->CalulcatePhonemesFromWav();
		delete processor;
	}

	void DestroyCallbacks()
	{
		debugLog = nullptr;
		phonemeCallback = nullptr;
		calibrationCallback = nullptr;
	}
}