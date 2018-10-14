//
//  VoiceRecordWrapper.h
//  
//
//  Created by John Philipsson on 2015-10-19.
//
//

#ifndef _VoiceRecordWrapper_h
#define _VoiceRecordWrapper_h

typedef void ( *CALLBACK )( bool allowed );
static CALLBACK cb;

void SetRecordingPhonemeCallback(CALLBACK test) {
    cb = test;
}


char* VoiceRecordStartMic(int lowcomplex, const char *cfilename, const char *guid);
bool VoiceRecordStopMic();
void VoiceRecordInitMic();
char* cStringCopy(const char* string);
static NSString* CreateNSString(const char* string);


@interface VoiceRecordWrapper : NSObject
- (void) SetCompleteHandler:(void (^)()) handler;
@end

#endif


/*

//
//  MicrophoneProperties.h
//  Unity-iPhone
//
//  Created by PlotAMac on 28/09/15.
//
//

#import <Foundation/Foundation.h>

typedef void ( *CALLBACK )( bool allowed );
static CALLBACK cb;

void SetMicAccessCallback(CALLBACK test) {
    cb = test;
}

@interface MicrophoneProperties : NSObject

- (void) CheckMicrophoneAccess;
- (void) SetCompleteHandler:(void (^)()) handler;

@end


*/