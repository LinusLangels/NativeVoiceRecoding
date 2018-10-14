#include "stdafx.h"
#include "record.h"
#include "debug_utilities.h"
#include <fstream>
#include "PhonemeWinCLib.h"
#include <stdlib.h>
#include <string.h>



#pragma comment(lib, "PhonemeWinCLib.lib")

using namespace std;



#define INP_BUFFER_SIZE 2048; 
#define INP_BUFFER_SIZE_IN_SHORT 1024 //half size of INP_BUFFER_SIZE

HWAVEIN				hWaveIn;
HWAVEOUT				hWaveOut;
WAVEFORMATEX			waveform;
PWAVEHDR				pWaveHdr1, pWaveHdr2;//hWaveHdr;
static LPSTR				lpBuffer1, lpNewBuffer, lpSaveBuffer, lpBuffer2;
DWORD				dwDataLength, dwRepetitions = 1;
static bool brecording = false;

LPSTR block; 
DWORD blockSize; 

int isStopped = 0;
FILE *input;
FILE *input_debug;
uint32_t  BytesRecorded;

voiceRec_t *voiceRec;

const uint8_t WAV_ChunksToHeader[WAV_HEADER_BYTES] = {
	// RIFF chunk /
	0x52, 0x49, 0x46, 0x46,   // "RIFF"
	0x00, 0x00, 0x00, 0x00,   // Total size
	0x57, 0x41, 0x56, 0x45,   // "WAVE"

							  // Format chunk @ 12 /
	0x66, 0x6D, 0x74, 0x20,   // "fmt "
	0x10, 0x00, 0x00, 0x00,   // Length of format chunk (constant)
	0x01, 0x00,
	0x00, 0x00,               // 0x01 = mono, 0x02 = stereo
	0x00, 0x00, 0x00, 0x00,   // Sample rate
	0x00, 0x00, 0x00, 0x00,   // Bytes per second
	0x00, 0x00,               // Bytes per sample
	0x00, 0x00,               // Bits per sample

							  // Data chunk @ 36 /
	0x64, 0x61, 0x74, 0x61,   // "data"
	0x00, 0x00, 0x00, 0x00 };  // Data size

void CreateHeaderToWave(uint8_t * Header, uint32_t BytesRecorded, WAV_FORMAT_DATA PCMfmt)
{
	memcpy(Header, WAV_ChunksToHeader, WAV_HEADER_BYTES);

	uint32_t PackageLength = BytesRecorded + 36;
	uint16_t Channels = PCMfmt.Channels;
	uint32_t SampleRate = PCMfmt.SampleRate;
	uint32_t BytesPerSample = PCMfmt.BitsPerSample / 8;

	//PackageLength = swap_uint32(PackageLength);
	memcpy(Header + 4, &PackageLength, 4);

	// Write Channels (22: 2 bytes <channels> // Channels: 1 = mono, 2 = stereo)
	memcpy(Header + 22, &Channels, 2);

	// Write SampleRate (24: 4 bytes <sample rate> // Samples per second: e.g., 44100)
	memcpy(Header + 24, &SampleRate, 4);

	// Write Bytes / Sec (28: 4 bytes <bytes/second> // sample rate * block align)
	uint32_t BytesPerSecond = SampleRate * BytesPerSample * Channels;
	memcpy(Header + 28, &BytesPerSecond, 4);

	// Write Bytes / Sample (32: 2 bytes <block align> // channels * bits/sample / 8)
	uint16_t BlockAlign = BytesPerSample * Channels;
	memcpy(Header + 32, &BlockAlign, 2);

	// Write Bits / Sample  (34: 2 bytes  <bits/sample>  // 8, 16, 24 or 32)
	uint16_t BitsPerSample = BytesPerSample * 8;
	memcpy(Header + 34, &BitsPerSample, 2);

	// Write Data Length
	uint32_t DataLength = BytesRecorded;
	memcpy(Header + 40, &DataLength, 4);

}



void MicInit()
{
	int nbrOfDevices = waveInGetNumDevs();
	consolePrintInt("Number of devices is %d\n", nbrOfDevices);
}


char* MicStart(bool lowCompelxityOFF, char *directorySavePath, char* guid)
{

	voiceRec = CalculatePhoenems_init(44100, 512, lowCompelxityOFF);



	int bufferlength = INP_BUFFER_SIZE;
	isStopped = 0;

	//fix filepath and wave-file name
	char *filetype = { ".wav" };
	char filepathWithWaveName[300];
	sprintf_s(filepathWithWaveName, "%s/%s%s", directorySavePath, guid, filetype);


	fopen_s(&input, filepathWithWaveName, "wb");

	if (input == NULL)
	{
		return{ "No such directory exists" };
	}

	
	fopen_s(&input_debug, "john_matlab_test.txt", "w");

	BytesRecorded = 0;

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


	MMRESULT i = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD)waveInProc, 0,
		CALLBACK_FUNCTION);

	pWaveHdr1->lpData = lpBuffer1;
	pWaveHdr1->dwBufferLength = INP_BUFFER_SIZE;
	pWaveHdr1->dwBytesRecorded = 0;
	pWaveHdr1->dwUser = 0;
	pWaveHdr1->dwFlags = 0;
	pWaveHdr1->dwLoops = 1;
	pWaveHdr1->lpNext = NULL;
	pWaveHdr1->reserved = 0;

	
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
	



	//add the buffers
	MMRESULT i2 = waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
	MMRESULT j2 = waveInAddBuffer(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));

	dwDataLength = 0;


	//begin sampling
	MMRESULT i3 = waveInStart(hWaveIn);

	return { "File created for recording" };
}


void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{

	if (isStopped == 1)
	{

		return;
	}

	if (uMsg == WIM_DATA)
	{
		float temparray[INP_BUFFER_SIZE_IN_SHORT];
		
		short tempSample;
		//consolePrintInt("Length %d\n", (int)(((PWAVEHDR)dwParam1)->dwBytesRecorded));
		fwrite((short*)(((PWAVEHDR)dwParam1)->lpData), sizeof(short),( ((PWAVEHDR)dwParam1)->dwBytesRecorded / 2), input);

		for (int k = 0; k < INP_BUFFER_SIZE_IN_SHORT; k++)
		{
			tempSample = ((short *)((PWAVEHDR)dwParam1)->lpData)[k];
			temparray[k] = ((float)tempSample) / 32768;
		
			fprintf(input_debug, "%f\n", temparray[k]);
		}
		voiceRec = CalculatePhoenems(temparray, voiceRec->signalE, voiceRec->vow, voiceRec, voiceRec->preVoice, voiceRec->dec, voiceRec->preDist);

		//consolePrintString("Phoneme is: %s\n", voiceRec->phoneme);

		BytesRecorded = BytesRecorded + ((PWAVEHDR)dwParam1)->dwBytesRecorded;
		MMRESULT i4 = waveInAddBuffer(hWaveIn, (PWAVEHDR)dwParam1, sizeof(WAVEHDR));

	
	}
}


bool MicStop()
{
	
	isStopped = 1;
	waveInUnprepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));  
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);

	free(lpBuffer1);
	free(lpBuffer2);
	free(pWaveHdr1);
	free(lpSaveBuffer);

	WAV_HEADER_DATA wav_header;
	WAV_FORMAT_DATA PCMfmt;

	wav_header.TotalHeaderSize = 44;
	PCMfmt.BitsPerSample = 16;
	PCMfmt.Channels = 1;
	PCMfmt.SampleRate = 44100;

	uint8_t * header = (uint8_t *)malloc(wav_header.TotalHeaderSize * sizeof(uint8_t));
	fseek(input, 0, SEEK_SET);

	CreateHeaderToWave(header, BytesRecorded, PCMfmt);

	fwrite(header, sizeof(int8_t), wav_header.TotalHeaderSize, input);
	free(header);
	fseek(input, wav_header.TotalHeaderSize, SEEK_SET);

	fclose(input);
	fclose(input_debug);
	CalculatePhoenems_destroy(voiceRec);

	return true;

}