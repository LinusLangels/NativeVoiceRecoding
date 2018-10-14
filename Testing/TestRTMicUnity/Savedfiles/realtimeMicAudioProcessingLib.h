//
//  realtimeMicAudioProcessingLib.h
//  realtimeMicAudioProcessingLib
//
//  Created by John Philipsson on 2015-10-19.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//



#import <Foundation/Foundation.h>


//#define RTUNITYEDITOR

#ifndef __realtimeMicAudioProcessingLib__
#define __realtimeMicAudioProcessingLib__

typedef void ( *CALLBACK )( char *allowed );

static CALLBACK cb;


void SetRecordingPhonemeCallback(CALLBACK test) {
    cb = test;
}


void SetRecordingPhonemeCallback(CALLBACK test);


@interface realtimeMicAudioProcessingLib : NSObject


-(void) initMicAudioProcessing;
-(void) destroyMicAudioProcessing;
-(NSString*)startMicAudioProcessing:(int)lowComplexProcessing withfilepath:(NSString*)filepath withGuid:(NSString*)guid withAlgoNbr:(int)algoNbr;
-(bool) stopMicAudioProcessing:(int)algoNbr;




@end


#endif /* defined(__realtimeMicAudioProcessingLib__) */