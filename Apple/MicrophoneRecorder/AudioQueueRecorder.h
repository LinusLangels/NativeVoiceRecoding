//
//  AudioQueueRecorder.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-11-30.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioQueue.h>
#import <AVFoundation/AVFoundation.h>
#import "CalibrateAudioData.h"

#ifdef BUILD_IOS
//Need more buffers on iOS to prevent glitches in recorded sound.
#define NUM_BUFFERS 8
#else
#define NUM_BUFFERS 3
#endif

typedef struct
{
    AudioStreamBasicDescription dataFormat;
    AudioQueueRef               queue;
    AudioQueueBufferRef         buffers[NUM_BUFFERS];
    AudioFileID                 audioFile;
    SInt64                      currentPacket;
    bool                        recording;
}RecordState;


@interface AudioQueueRecorder : NSObject
{
    StringParameterCallback m_Logger;
    PhonemeParameterCallback phonemeDataCallback;
    CalibrationCallback calibrationCallback;
    float calibrationFactor;
    bool isCalibratingMicrophone;
    CALIBRATION_t *calibrationData;
    RecordState *recordState;
}

- (id) initWithCallbacks:(StringParameterCallback)logger andPhonemeCallback:(PhonemeParameterCallback) callback andCalibrationCallback:(CalibrationCallback)calibrateCallback;
- (void)initializeRecording;
- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate;
- (void)stopRecording:(int)algonbr;
- (void) destroyRecording;
- (void) callPhonemeCallback:(float)start andStop:(float)stop andPhoneme:(const char*)phonemein;
- (bool) isCalibrating;
- (float) runtimeCalibrationFactor;
- (CALIBRATION_t*) getCalibrationData;

+ (id)instance;
+ (void)removeInstance;

@end






