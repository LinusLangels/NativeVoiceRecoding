#define _CRT_SECURE_NO_WARNINGS //Allow strcpy on windows.
#define _CRT_SECURE_NO_DEPRECATE //Allow fopen on windows.

#include "WavPostProcessor.h"
#include "WriteWaveFile.h"
#include "stdio.h"
#include <stdlib.h>
#include <cstring>
#include <string>

WavPostProcessor::WavPostProcessor(const char* path, float startTime, float stopTime)
{
	m_wavFilePath = path;
	m_startTime = startTime;
	m_stopTime = stopTime;
}

void WavPostProcessor::setLogger(StringParameterCallback logger)
{
	m_Logger = logger;
}

void WavPostProcessor::trimWavFile()
{
	//m_Logger("Trimming native recording!");

	//Create temporary filepath where we write our trimmed wav file.
	std::string temp = "temp";
	std::string concattedPath = m_wavFilePath + temp;
	const char* temporaryPath = concattedPath.c_str();

	//Open wav file for reading.
	FILE *wavFileHandle = fopen(m_wavFilePath, "rb");

	//Read Wav header.
	WavHeader *header = (WavHeader*)malloc(sizeof(WavHeader));
	fread(header, sizeof(WavHeader), 1, wavFileHandle);

	uint32_t audioDataLength = header->datachunk_size;
	uint32_t sampleRate = header->sample_rate;
	uint16_t channels = header->num_channels;
	uint16_t byteRate = header->bps / 8;
	float totalDuration = (float)audioDataLength / (sampleRate * channels * byteRate);

	//Make sure we dont go out of bounds.
	if (m_stopTime >= totalDuration)
	{
		m_stopTime = totalDuration;
	}

	int numberOfBytesToRead = m_stopTime * (sampleRate * channels * byteRate);
	//Make its an even number, gets rid of the problem with shorts.
	numberOfBytesToRead = numberOfBytesToRead - (numberOfBytesToRead % 2);

	//Write header.
	header->chunk_size = numberOfBytesToRead + 36;
	header->datachunk_size = numberOfBytesToRead;

	//Create temporary file and open for writing.
	FILE *temporaryFile = fopen(temporaryPath, "wb");

	//Write updated header with new length to temporary file.
	fwrite(header, sizeof(WavHeader), 1, temporaryFile);

	//Setup.
	int bufferSize = 1024;
	int currentPosition = 0;

	//Allocate memory for buffer.
	char *sampleBuffer = (char*)malloc(bufferSize);

	//Sanity check, make sure buffer was allocated.
	if (sampleBuffer == NULL)
	{
		fclose(wavFileHandle);
		fclose(temporaryFile);
		free(header);

		return;
	}

	//Read and write sample data until we reach trim boundry.
	while (currentPosition < numberOfBytesToRead)
	{
		int increment = bufferSize;
		int diff = numberOfBytesToRead - currentPosition;

		if (diff <= bufferSize)
			increment = diff;

		//Rescale buffer at the end to match the last samples.
		if (increment != bufferSize)
		{
			char *scaledBuffer = (char*)realloc(sampleBuffer, increment * sizeof(char));
			sampleBuffer = scaledBuffer;
		}

		fread(sampleBuffer, sizeof(char), increment, wavFileHandle);
		fwrite(sampleBuffer, sizeof(char), increment, temporaryFile);

		currentPosition += increment;
	}

	//close the wav-file.
	fclose(wavFileHandle);

	//Close the temporary file.
	fclose(temporaryFile);

	//Remove old file.
	remove(m_wavFilePath);

	//Have the temporary file takes the original files place.
	rename(temporaryPath, m_wavFilePath);

	//Free all dynamic memory.
	free(sampleBuffer);
	free(header);
}

WavPostProcessor::~WavPostProcessor()
{
	if (m_Logger != nullptr)
	{
		//m_Logger("Destroyed wav post processor!");
	}

	m_Logger = nullptr;
}
