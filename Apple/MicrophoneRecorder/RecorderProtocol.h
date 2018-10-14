//
//  RecorderDelegate.m
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-11-30.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void ( *StringParameterCallback )( const char* );

@protocol RecorderProtocol

- (id)initWithLoggerAndPhoneme:(StringParameterCallback)logger andPhonemeCallback:(PhonemeParameterCallback)phoneme andCalibrationCallback:(CalibrationCallback)calibrationCallback;
- (void)initializeRecording;
- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate;
- (void)stopRecording:(int)algonbr;
- (void)destroyRecording;

@end
