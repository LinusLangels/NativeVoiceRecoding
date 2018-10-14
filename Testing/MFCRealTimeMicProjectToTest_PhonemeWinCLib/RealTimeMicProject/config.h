#ifndef JT_FINAL_SOUND
#define JT_FINAL_SOUND


#include "windows.h"
#include "mmsystem.h"

#define INP_BUFFER_SIZE		16384;

HWAVEIN				hWaveIn;
HWAVEOUT				hWaveOut;
WAVEFORMATEX			waveform;
PWAVEHDR				pWaveHdr1,pWaveHdr2;//hWaveHdr;
static LPSTR				lpBuffer1,lpNewBuffer,lpSaveBuffer;
DWORD				dwDataLength,dwRepetitions = 1;
static bool brecording=false;

LPSTR block; /*  pointer to the block */
DWORD blockSize; /* holds the size of the block */


void CALLBACK waveInProc(
  HWAVEIN hwi,       
  UINT uMsg,         
  DWORD dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
);


int DoRecord();
int DoRecord_end();
int DoPlay();


LPSTR loadAudioBlock(const char* filename, DWORD* blockSize);
void writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size);

#endif /* JT_FINAL_SOUND */