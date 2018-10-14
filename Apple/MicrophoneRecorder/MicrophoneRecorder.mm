//
//  realtimeMicAudioProcessingLib.h
//  realtimeMicAudioProcessingLib
//
//  Created by John Philipsson on 2015-10-19.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import "MicrophoneRecorder.h"
#import "RecorderProtocol.h"
#import "WavPostProcessor.h"
#import "Recorder.h"
#import "WavPhonemeProcessor.h"

static StringParameterCallback debugLog;
static PhonemeParameterCallback phonemeCallback;
static CalibrationCallback calibrationCallback;

void SetDebugLog(StringParameterCallback functionDelegate)
{
    debugLog = functionDelegate;
}

void SetPhonemeCallback(PhonemeParameterCallback functionDelegate)
{
    phonemeCallback = functionDelegate;
}

void SetCalibrationCallback(CalibrationCallback functionDelegate)
{
    calibrationCallback = functionDelegate;
}

void DestroyCallbacks()
{
    debugLog = NULL;
    phonemeCallback = NULL;
    calibrationCallback = NULL;
    
}

//Recording interface for outsiders.
@interface RecorderWrapper : NSObject
{
    NSObject<RecorderProtocol> * _recorder;
}

-(id) initWithRecorder:(NSObject<RecorderProtocol> *) recorderImpl;
- (void) initializeRecording;
- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate;
- (void)stopRecording:(int)algonbr;


@property (nonatomic, strong) NSObject<RecorderProtocol> *recorder;

@end

@implementation RecorderWrapper

-(NSObject<RecorderProtocol> *)recorder {
    return _recorder;
}

-(void) setRecorder:(NSObject<RecorderProtocol> *)newRecorder
{
    _recorder = newRecorder;
}

-(id) initWithRecorder:(NSObject<RecorderProtocol> *) recorderImpl
{
    self = [super init];
    
    if (self)
    {
        self.recorder = recorderImpl;
        return self;
    }
    
    return nil;
}

- (void) initializeRecording
{
    [self.recorder initializeRecording];
}

- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate
{
    [self.recorder startRecording:lowComplexProcessing withfilepath:filepath withAlgoNbr:algoNbr withCalibrationFactor:runtimeCalibrationFactor withCalibration:(bool)calibrate];
}

- (void)stopRecording:(int)algonbr
{
    [self.recorder stopRecording:algonbr];
}

-(void) dealloc
{
    //debugLog("Callback: Destroyed wrapper");
    
    [self.recorder destroyRecording];
    #if  ! __has_feature(objc_arc)
    [self.recorder release];
    #endif
    self.recorder = nil;
    #if  ! __has_feature(objc_arc)
    [super dealloc];
    #endif
}

@end

static RecorderWrapper* currentRecorder;

bool Bridge_IsMicrophoneAvailible()
{
    return true;
}

void Bridge_InitRecording()
{
    NSLog(@"hello from init!");

    if (debugLog != NULL)
    {
        debugLog("Callback: Created recorder");
    }
    
    NSObject<RecorderProtocol> *recorder = [[OSXRecorder alloc] initWithLoggerAndPhoneme:debugLog andPhonemeCallback:phonemeCallback andCalibrationCallback:calibrationCallback];
    
    //Give concrete recorder to wrapper object.
    currentRecorder = [[RecorderWrapper alloc] initWithRecorder:recorder];
    
    if (currentRecorder)
    {
        [currentRecorder initializeRecording];
    }
}

void Bridge_StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate)
{
    NSString *convertedPath = [NSString stringWithUTF8String:path];
    
    if (currentRecorder)
    {
        [currentRecorder startRecording:complexity withfilepath:convertedPath withAlgoNbr:algoNumber withCalibrationFactor:calibrationFactor withCalibration:calibrate];
    }
}

void Bridge_StopRecording(int algoNumber)
{
    if (currentRecorder)
    {
        [currentRecorder stopRecording:algoNumber];
    }
}

void Bridge_Destroy()
{
    if (currentRecorder)
    {
        NSLog(@"destroying recorder");
        
        #if  ! __has_feature(objc_arc)
        [currentRecorder release];
        #endif
        currentRecorder = nil;
    }
}

void Bridge_TrimRecording(const char* path, float startTime, float stopTime)
{
    WavPostProcessor *processor = new WavPostProcessor(path, startTime, stopTime);
    processor->setLogger(debugLog);
    processor->trimWavFile();
    
    delete processor;
}


void Bridge_CalulcatePhonemesFromWav(const char* path)
{
    WavPhonemeProcessor *processor = new WavPhonemeProcessor(path);
    processor->setPhonemeLogger(phonemeCallback);
    processor->CalulcatePhonemesFromWav();
    delete processor;
}








