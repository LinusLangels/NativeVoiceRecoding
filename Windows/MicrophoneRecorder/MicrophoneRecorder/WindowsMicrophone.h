#pragma once

#include "SharedProjectHeader.h"
#include "../../../SharedSource/VoiceRecordingUtils.h"
#include "windows.h"
#include "stdio.h"
#include "stdint.h"

extern "C" {
#include "../../../SharedSource/CalibrateAudioData.h"
}

extern "C"
{
	void CALLBACK waveInProc(
	HWAVEIN hwi,
	UINT uMsg,
	DWORD dwInstance,
	DWORD dwParam1,
	DWORD dwParam2
	);
}

class WindowsMicrophone 
{
	StringParameterCallback m_Logger;
	PhonemeParameterCallback phonemeCallback;
	CalibrationCallback calibrateCallback;

	WAVEFORMATEX		waveform;
	PWAVEHDR			pWaveHdr1, pWaveHdr2;
	LPSTR		lpBuffer1, lpNewBuffer, lpSaveBuffer, lpBuffer2;
	DWORD				dwDataLength, dwRepetitions = 1;
	bool isRecording;

	public:
	static WindowsMicrophone* instance;

	HWAVEIN				hWaveIn;
	HWAVEOUT			hWaveOut;
	uint32_t wordRecorded;
	float phonemeStarttime;
	float phonemeStoptime;
	voiceRec_t *voiceRec;
	bool isCalibrating;
    float runtimeCalibrateFactor;
	CALIBRATION_t *calibrationData;

	WindowsMicrophone(StringParameterCallback logCallback, PhonemeParameterCallback callback, CalibrationCallback calibrationCallback);
	void InitializeRecording();
	void StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate);
	void StopRecording(int algoNumber);
	void AddPhoneme(float startTime, float endTime, const char* phonemein);
	bool IsRecording();
	bool IsCalibrating();
	bool IsMicrophoneAvailible();
	void LogMessage(const char* message);

	~WindowsMicrophone();
};