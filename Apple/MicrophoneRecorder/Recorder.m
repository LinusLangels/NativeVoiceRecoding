//
//  OSXRecorder.m
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-12-15.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#import "Recorder.h"

@implementation OSXRecorder

- (id)initWithLoggerAndPhoneme:(StringParameterCallback)logger andPhonemeCallback:(PhonemeParameterCallback)phoneme andCalibrationCallback:(CalibrationCallback)calibrateCallback;
{
    self = [super init];
    
    if (self)
    {
        logCallback = logger;
        phonemeCallback = phoneme;
        calibrationCallback = calibrateCallback;
        
        return self;
    }
    
    return nil;
}

- (void)initializeRecording;
{
    audioRecorder = [[AudioQueueRecorder alloc] initWithCallbacks:logCallback andPhonemeCallback:phonemeCallback andCalibrationCallback:calibrationCallback];
    
    if (audioRecorder)
    {
        [audioRecorder initializeRecording];
    }
}

- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate
{
    if (audioRecorder)
    {
        [audioRecorder startRecording:lowComplexProcessing withfilepath:filepath withAlgoNbr:algoNbr withCalibrationFactor:runtimeCalibrationFactor withCalibration:calibrate];
    }
}

- (void)stopRecording:(int)algonbr;
{
    if (audioRecorder)
    {
        [audioRecorder stopRecording:algonbr];
    }
}

- (void)destroyRecording
{
}

-(void) dealloc
{
    if (logCallback != NULL)
    {
        logCallback("Callback: Destroyed osx recorder");
    }
    
    logCallback = NULL;
    phonemeCallback = NULL;
    calibrationCallback = NULL;
    
    //So that arc can free it, remove static instance.
    [AudioQueueRecorder removeInstance];
    
    if (audioRecorder)
    {
        [audioRecorder destroyRecording];
        #if  ! __has_feature(objc_arc)
        [audioRecorder release];
        #endif
        audioRecorder = nil;
    }
    
    #if  ! __has_feature(objc_arc)
    [super dealloc];
    #endif
}

@end
