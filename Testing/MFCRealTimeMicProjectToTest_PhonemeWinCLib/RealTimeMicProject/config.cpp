/* <JT>
   i write this example for my purpose specific.
   I will post it to codeguru.Hope that will help
   some of you to understand wave interface.
  
*/

#include "config.h"

void writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size)
    {
    WAVEHDR header;
    /*
    * initialise the block header with the size
    * and pointer.
    */
    ZeroMemory(&header, sizeof(WAVEHDR));
    header.dwBufferLength = size;   //length, in bytes, of the buffer.
    header.lpData = block;   // pointer to waveform buffer.
    /*
    * prepare the block for playback
    */
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    /*
    * write the block to the device. waveOutWrite returns immediately
    * unless a synchronous driver is used (not often).
    */
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    /*
    * wait a while for the block to play then start trying
    * to unprepare the header. this will fail until the block has
    * played.
    */
    Sleep(500);
    while(waveOutUnprepareHeader(
    hWaveOut, 
    &header, 
    sizeof(WAVEHDR)
    ) == WAVERR_STILLPLAYING)
    Sleep(100);
}


LPSTR loadAudioBlock(const char* filename, DWORD* blockSize)


    {
    HANDLE hFile= INVALID_HANDLE_VALUE;
    DWORD size = 0;
    DWORD readBytes = 0;
    void* block = NULL;
    /*
    * open the file
    */
    if((hFile = CreateFile(
    filename,
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
    )) == INVALID_HANDLE_VALUE)
    return NULL;
    /*
    * get it's size, allocate memory and read the file
    * into memory. don't use this on large files!
    */


        do {
        if((size = GetFileSize(hFile, NULL)) == 0) 
        break;
        if((block = HeapAlloc(GetProcessHeap(), 0, size)) == NULL)
        break;
        ReadFile(hFile, block, size, &readBytes, NULL);
    } while(0);
    CloseHandle(hFile);
    *blockSize = size;
    return (LPSTR)block;
}

void CALLBACK waveInProc(
  HWAVEIN hwi,       
  UINT uMsg,         
  DWORD dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
)
{
/*
	attention:
	Don't call any systemed-function in waveInProc
*/

if (uMsg == WIM_OPEN)
{
  
}

else
if (uMsg == WIM_DATA)
{
	lpNewBuffer = (char *)realloc(lpSaveBuffer,dwDataLength + ((PWAVEHDR)dwParam1)->dwBytesRecorded);

	if (lpNewBuffer ==NULL)
	{
	   // waveInClose(hWaveIn);
		MessageBox(NULL,"error with lpNewBuffer",NULL,MB_OK);
	}    

	lpSaveBuffer = lpNewBuffer;

	CopyMemory(lpSaveBuffer + dwDataLength, ((PWAVEHDR)dwParam1)->lpData,((PWAVEHDR)dwParam1)->dwBufferLength );

	dwDataLength +=((PWAVEHDR)dwParam1)->dwBytesRecorded ;

//	if (((PWAVEHDR)dwParam1)->dwFlags = WHDR_DONE)
//	{brecording = true;}


		MMRESULT i4 = waveInAddBuffer(hWaveIn,(PWAVEHDR)dwParam1,sizeof(WAVEHDR));

		switch (i4)
		{
		case MMSYSERR_INVALHANDLE:
			MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
			break;
		case MMSYSERR_NODRIVER:
			MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
			break;
		case MMSYSERR_NOMEM:
			MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
			break;
		case WAVERR_UNPREPARED:
			MessageBox(NULL,"The buffer pointed to by the pwh parameter hasn't been prepared. ",NULL,MB_OK);
			break;

			}



return;
}

else 

if (uMsg == WIM_CLOSE)

{
	//waveInUnprepareHeader(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));
	free (lpBuffer1);
	
}
}


int DoRecord()

{

pWaveHdr1   = (PWAVEHDR)malloc(sizeof(WAVEHDR));

lpBuffer1  = (char*)malloc(16384);
	
//open waveform audio for input 
waveform.wFormatTag = WAVE_FORMAT_PCM;
waveform.nChannels = 1;
waveform.nSamplesPerSec = 8000;
waveform.wBitsPerSample = 16;
waveform.cbSize = 0;
waveform.nBlockAlign  = (waveform.nChannels * waveform.wBitsPerSample)/8;
waveform.nAvgBytesPerSec = (waveform.nSamplesPerSec*waveform.nBlockAlign);



MMRESULT i = waveInOpen(&hWaveIn,WAVE_MAPPER,&waveform,(DWORD)waveInProc,0, 
						CALLBACK_FUNCTION);

switch (i)
{

case MMSYSERR_ALLOCATED:
	MessageBox(NULL,"Specified resource is already allocated.",NULL,MB_OK);
	break;
case MMSYSERR_BADDEVICEID:
	MessageBox(NULL,"Specified device identifier is out of range. ",NULL,MB_OK);
	break;
case MMSYSERR_NODRIVER:
	MessageBox(NULL,"no device driver is present.",NULL,MB_OK);
	break;
case MMSYSERR_NOMEM:
	MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
	break;
case WAVERR_BADFORMAT:
	MessageBox(NULL,"Attempted to open with an unsupported waveform-audio format ",NULL,MB_OK);
	break;
case MMSYSERR_NOERROR :
	MessageBox(NULL,"waveinopen is opened successful!",NULL,MB_OK);
	break;

default:
	MessageBox(NULL,"waveInOpen other error",NULL,MB_OK);

}

//setup the headers and prepare them
pWaveHdr1->lpData				= lpBuffer1;
pWaveHdr1->dwBufferLength		= INP_BUFFER_SIZE;
pWaveHdr1->dwBytesRecorded	= 0;
pWaveHdr1->dwUser				= 0;
pWaveHdr1->dwFlags			= 0;
pWaveHdr1->dwLoops			= 1;
pWaveHdr1->lpNext				= NULL;
pWaveHdr1->reserved			= 0;


MMRESULT i1= waveInPrepareHeader(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));

switch (i1)
{
case MMSYSERR_INVALHANDLE:
	MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
	break;
case MMSYSERR_NODRIVER:
	MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
	break;
case MMSYSERR_NOMEM:
	MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
	break;

}


 lpSaveBuffer = (char*)malloc(1);



   //add the buffers
MMRESULT i2 = waveInAddBuffer(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));

switch (i2)
{
case MMSYSERR_INVALHANDLE:
	MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
	break;
case MMSYSERR_NODRIVER:
	MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
	break;
case MMSYSERR_NOMEM:
	MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
	break;
case WAVERR_UNPREPARED:
	MessageBox(NULL,"The buffer pointed to by the pwh parameter hasn't been prepared. ",NULL,MB_OK);
	break;
}


   dwDataLength = 0;


   //begin sampling
MMRESULT i3 = waveInStart(hWaveIn);

switch (i3)
{
case MMSYSERR_INVALHANDLE:
	MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
	break;
case MMSYSERR_NODRIVER:
	MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
	break;
case MMSYSERR_NOMEM:
	MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
	break;
	
}

#if 0
		while (brecording == true)
		{
				lpBuffer1 = NULL;
				pWaveHdr1->lpData				= lpBuffer1;
				pWaveHdr1->dwBufferLength		= INP_BUFFER_SIZE;
				pWaveHdr1->dwBytesRecorded	= 0;
				pWaveHdr1->dwUser				= 0;
				pWaveHdr1->dwFlags			= 0;
				pWaveHdr1->dwLoops			= 1;
				pWaveHdr1->lpNext				= NULL;
				pWaveHdr1->reserved			= 0;
				
				MMRESULT i4 = waveInAddBuffer(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));

				switch (i4)
				{
				case MMSYSERR_INVALHANDLE:
					MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
					break;
				case MMSYSERR_NODRIVER:
					MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
					break;
				case MMSYSERR_NOMEM:
					MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
					break;
				case WAVERR_UNPREPARED:
					MessageBox(NULL,"The buffer pointed to by the pwh parameter hasn't been prepared. ",NULL,MB_OK);
					break;

				}

       

		}

#endif


return 0;
}



int DoRecord_end()

{

#if 0
			MMRESULT i = waveInReset(hWaveIn);

		 switch (i)
		 {
		 case MMSYSERR_INVALHANDLE:
			 MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NODRIVER:
			 MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NOMEM:
			 MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NOERROR:
			 MessageBox(NULL,"DoRecord_end is done successful",NULL,MB_OK);
			 break;
		 }
#endif
		 MMRESULT j = waveInStop(hWaveIn);

		 switch (j)
		 {
		 case MMSYSERR_INVALHANDLE:
			 MessageBox(NULL,"Specified device handle is invalid. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NODRIVER:
			 MessageBox(NULL,"No device driver is present. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NOMEM:
			 MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
			 break;
		 case MMSYSERR_NOERROR:
			 MessageBox(NULL,"waveInStop is done successful",NULL,MB_OK);
			 break;
		 
		 }


return 0;
}


int DoPlay()
{
//open waveform audio for output

	
MMRESULT i = waveOutOpen(&hWaveOut,WAVE_MAPPER,&waveform,0,0, 
						CALLBACK_NULL);

switch (i)
{

case MMSYSERR_ALLOCATED:
	MessageBox(NULL,"Specified resource is already allocated.",NULL,MB_OK);
	break;
case MMSYSERR_BADDEVICEID:
	MessageBox(NULL,"Specified device identifier is out of range. ",NULL,MB_OK);
	break;
case MMSYSERR_NODRIVER:
	MessageBox(NULL,"no device driver is present.",NULL,MB_OK);
	break;
case MMSYSERR_NOMEM:
	MessageBox(NULL,"Unable to allocate or lock memory. ",NULL,MB_OK);
	break;
case WAVERR_BADFORMAT:
	MessageBox(NULL,"Attempted to open with an unsupported waveform-audio format ",NULL,MB_OK);
	break;
case MMSYSERR_NOERROR :
	MessageBox(NULL,"waveOutopen is opened successful!",NULL,MB_OK);
	break;
case WAVERR_SYNC:
	MessageBox(NULL,"The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag. ",NULL,MB_OK);
	break;
}



block = lpSaveBuffer;
blockSize=dwDataLength;

writeAudioBlock(hWaveOut,block,blockSize);


waveOutClose(hWaveOut);



return 0;


}


