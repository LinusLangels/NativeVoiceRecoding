//
//  MicrophoneRecorder.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-12-15.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#ifndef MicrophoneRecorder_h
#define MicrophoneRecorder_h


extern "C"
{
    void SetDebugLog(StringParameterCallback functionDelegate);

    void SetPhonemeCallback(PhonemeParameterCallback functionDelegate);
    
    void SetCalibrationCallback(CalibrationCallback functionDelegate);
    
    void DestroyCallbacks();
    
    bool Bridge_IsMicrophoneAvailible();

    void Bridge_InitRecording();

    void Bridge_StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate);

    void Bridge_StopRecording(int algoNumber);

    void Bridge_Destroy();

    void Bridge_TrimRecording(const char* path, float startTime, float stopTime);
    
    void Bridge_CalulcatePhonemesFromWav(const char* path);
}

#endif /* MicrophoneRecorder_h */
