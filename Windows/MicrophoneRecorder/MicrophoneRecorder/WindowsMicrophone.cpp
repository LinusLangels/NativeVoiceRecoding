#define _CRT_SECURE_NO_WARNINGS //Allow strcpy on windows.

#include "WindowsMicrophone.h"
#include <fstream>
#include "mmsystem.h"
#include "../../../SharedSource/WriteWaveFile.h"

#define INP_BUFFER_SIZE 4096; 
#define INP_BUFFER_SIZE_IN_SHORT 2048 //half size of INP_BUFFER_SIZE

WindowsMicrophone::WindowsMicrophone(StringParameterCallback logCallback, PhonemeParameterCallback callback, CalibrationCallback calibrationCallback)
{
	m_Logger = logCallback;
	phonemeCallback = callback;
	calibrateCallback = calibrationCallback;

	WindowsMicrophone::instance = this;
}

bool WindowsMicrophone::IsMicrophoneAvailible()
{
	int nbrOfDevices = waveInGetNumDevs();

	if (nbrOfDevices >= 1)
		return true;
	else
		return false;
}

void WindowsMicrophone::InitializeRecording()
{
	//m_Logger("initializing");
}

void WindowsMicrophone::StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate)
{
	runtimeCalibrateFactor = calibrationFactor;
	isCalibrating = calibrate;

	if (isCalibrating)
	{
		calibrationData = (CALIBRATION_st*)malloc(sizeof(CALIBRATION_st));
		calibrationData->currentCalibrationScaleFactor = 0.0f;
		calibrationData->maxAbsMean = -1.0;
	}
    
	if (complexity == 1)
	{
		voiceRec = CalculatePhoenems_init(44100, 512, false, algoNumber, true);
	}
	else
	{
		voiceRec = CalculatePhoenems_init(44100, 512, true, algoNumber, true);
	}

	//Allow wav writer to log stuff into unity console.
	SetWavDebugLog(m_Logger);

	//Create and open wav file for writing data.
	OpenWavFile(path);

	int bufferlength = INP_BUFFER_SIZE;

	//Create recording buffers.
	pWaveHdr1 = (PWAVEHDR)malloc(sizeof(WAVEHDR));
	pWaveHdr2 = (PWAVEHDR)malloc(sizeof(WAVEHDR));

	lpBuffer1 = (char*)malloc(bufferlength);
	lpBuffer2 = (char*)malloc(bufferlength);

	memset(lpBuffer1, 0, bufferlength);
	memset(lpBuffer2, 0, bufferlength);
	
	//open waveform audio for input 
	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nChannels = 1;
	waveform.nSamplesPerSec = 44100;
	waveform.wBitsPerSample = 16;
	waveform.cbSize = 0;
	waveform.nBlockAlign = (waveform.nChannels * waveform.wBitsPerSample) / 8;
	waveform.nAvgBytesPerSec = (waveform.nSamplesPerSec*waveform.nBlockAlign);

	MMRESULT i = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION);

	//Setup of first buffer.
	pWaveHdr1->lpData = lpBuffer1;
	pWaveHdr1->dwBufferLength = INP_BUFFER_SIZE;
	pWaveHdr1->dwBytesRecorded = 0;
	pWaveHdr1->dwUser = 0;
	pWaveHdr1->dwFlags = 0;
	pWaveHdr1->dwLoops = 1;
	pWaveHdr1->lpNext = NULL;
	pWaveHdr1->reserved = 0;

	//Setup of second buffer.
	pWaveHdr2->lpData = lpBuffer2;
	pWaveHdr2->dwBufferLength = INP_BUFFER_SIZE;
	pWaveHdr2->dwBytesRecorded = 0;
	pWaveHdr2->dwUser = 0;
	pWaveHdr2->dwFlags = 0;
	pWaveHdr2->dwLoops = 1;
	pWaveHdr2->lpNext = NULL;
	pWaveHdr2->reserved = 0;

	MMRESULT i1 = waveInPrepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
	MMRESULT j1 = waveInPrepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));

	lpSaveBuffer = (char*)malloc(1);

	MMRESULT i2 = waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
	MMRESULT j2 = waveInAddBuffer(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));

	MMRESULT i3 = waveInStart(hWaveIn);

	isRecording = true;

	//m_Logger("starting");
}

void WindowsMicrophone::StopRecording(int algoNumber)
{
	if (isCalibrating)
	{
		float scaleFactor;
		float maxAbsMean = calibrationData->maxAbsMean;

		if (maxAbsMean < 1.1)
		{
			scaleFactor = 0.5*(1.0 / (maxAbsMean));
		}
		else 
		{
			scaleFactor = 1.0;
		}

		calibrationData->currentCalibrationScaleFactor = scaleFactor;

		if (calibrateCallback != NULL)
		{
			calibrateCallback(maxAbsMean, scaleFactor);
		}

		free(calibrationData);
		calibrationData = nullptr;
	}

	//m_Logger("stopping");
	isRecording = false;

	waveInUnprepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
	
	free(lpBuffer1);
	free(lpBuffer2);
	free(pWaveHdr1);
	free(lpSaveBuffer);

	CalculatePhoenems_destroy(voiceRec, algoNumber);

	//Finish and close wav file.
	CloseWavFile();
}

bool WindowsMicrophone::IsRecording()
{
	return isRecording;
}

bool WindowsMicrophone::IsCalibrating() 
{
	return isCalibrating;
}

void WindowsMicrophone::AddPhoneme(float startTime, float stopTime, const char* phonemein)
{
	struct PhonemeData phoneme;
	phoneme.start = startTime;
	phoneme.stop = stopTime;
	phoneme.phoneme = phonemein;

	if (phonemeCallback != nullptr)
	{
		phonemeCallback(&phoneme);
	}
}

void WindowsMicrophone::LogMessage(const char* message)
{
	m_Logger(message);
}

WindowsMicrophone::~WindowsMicrophone()
{
	//m_Logger("destroying");
	WindowsMicrophone:instance = nullptr;

	m_Logger = nullptr;
	phonemeCallback = nullptr;
	calibrateCallback = nullptr;
}

WindowsMicrophone* WindowsMicrophone::instance = nullptr;

extern "C"
{
    void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
    {
        WindowsMicrophone *recorderInstance = WindowsMicrophone::instance;
        
        if (recorderInstance == nullptr)
            return;
        
        if (!recorderInstance->IsRecording())
            return;
        
        if (uMsg == WIM_DATA)
        {
            //set calibration factor
            float calibrationFactor = recorderInstance->runtimeCalibrateFactor;

			if (recorderInstance->IsCalibrating())
			{
				calibrationFactor = 1.0f;
			}
            
            float temparray[INP_BUFFER_SIZE_IN_SHORT];
            short tempSample;
            int count = 0;

			for (int k = 0; k < INP_BUFFER_SIZE_IN_SHORT; k++)
			{
				tempSample = ((short *)((PWAVEHDR)dwParam1)->lpData)[k];
				temparray[k] = calibrationFactor * (((float)tempSample) / 32768);
					
				((short *)((PWAVEHDR)dwParam1)->lpData)[k] = (short)(temparray[k] * 32768);
				count++;
			}

			if (recorderInstance->IsCalibrating())
			{
				CalibrateAudio(recorderInstance->calibrationData, temparray, 10, count);
			}
            
            //Stream samples to open wav file.
            //Note: Written samples have been calibrated/Normalized
            SaveSamples((short*)(((PWAVEHDR)dwParam1)->lpData), sizeof(short), (((PWAVEHDR)dwParam1)->dwBytesRecorded / 2));
            
            //Get public variables from recorder instance, easier to manage them like this.
            voiceRec_t *voiceRec = recorderInstance->voiceRec;
            
            //Calculate phoneme timespan.
            recorderInstance->phonemeStarttime = (((float)recorderInstance->wordRecorded) / (voiceRec->fs));
            recorderInstance->wordRecorded += count;
            recorderInstance->phonemeStoptime = (((float)recorderInstance->wordRecorded) / (voiceRec->fs));
            
            //Calculate phoneme at this particular time.
            recorderInstance->voiceRec = CalculatePhoenems(temparray, voiceRec->signalE, voiceRec->vow, voiceRec, voiceRec->preVoice, voiceRec->dec, voiceRec->preDist, voiceRec->evad);
            
            //Add phoneme to streaming buffer for animation calculation.
            recorderInstance->AddPhoneme(recorderInstance->phonemeStarttime, recorderInstance->phonemeStoptime, recorderInstance->voiceRec->phoneme);

            //Swap buffers.
            MMRESULT i4 = waveInAddBuffer(recorderInstance->hWaveIn, (PWAVEHDR)dwParam1, sizeof(WAVEHDR));
        }
    }
}