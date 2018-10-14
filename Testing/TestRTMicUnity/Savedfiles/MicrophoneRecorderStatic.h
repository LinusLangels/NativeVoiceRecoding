
//  MicrophoneRecorderStatic.h
//  MicrophoneRecorderStatic
//
//  Created by John Philipsson on 2015-12-08.
//  Copyright © 2015 John Philipsson. All rights reserved.
//



#import <Foundation/Foundation.h>

@class AudioProcessor;

typedef void ( *CALLBACK )( char *allowed );

static CALLBACK cb;

void SetRecordingPhonemeCallback(CALLBACK test) {
    
    cb = test;
    
}

@interface MicrophoneRecorderStatic : NSObject
{
    

}

@property (retain, nonatomic) AudioProcessor *audioProcessor;

-(void)initMicAudioProcessing;
-(void)destroyMicAudioProcessing;
-(NSString*)startMicAudioProcessing:(int)lowComplexProcessing withfilepath:(NSString*)filepath withGuid:(NSString*)guid withAlgoNbr:(int)algoNbr;
-(bool)stopMicAudioProcessing:(int)algoNbr;

@end









