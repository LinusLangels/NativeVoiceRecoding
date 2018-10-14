#pragma once

typedef void(*StringParameterCallback)(const char*);

#ifdef __cplusplus
class WavPostProcessor
{
	StringParameterCallback m_Logger;
	const char* m_wavFilePath;
	float m_startTime;
	float m_stopTime;

public:
	WavPostProcessor(const char* path, float startTime, float stopTime);

	void setLogger(StringParameterCallback logger);
	void trimWavFile();

	~WavPostProcessor();
};
#endif

