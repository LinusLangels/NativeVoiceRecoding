//
//  WavPhonemeProcessor.hpp
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-07-13.
//  Copyright © 2016 John Philipsson. All rights reserved.
//


 
 
#pragma once

#ifdef _WIN32
#include "../Windows/MicrophoneRecorder/MicrophoneRecorder/SharedProjectHeader.h"
#endif

#ifdef __cplusplus
class WavPhonemeProcessor
{
    PhonemeParameterCallback m_PhonemeLogger;
    const char* m_wavFilePath;
    
public:
    WavPhonemeProcessor(const char* path);
    
    void setPhonemeLogger(PhonemeParameterCallback phonemeLogger);
    void CalulcatePhonemesFromWav();
    
    ~WavPhonemeProcessor();
};
#endif

