//
//  VoiceRecordWrapper.m
//  
//
//  Created by John Philipsson on 2015-10-19.
//
//

/*
#undef TARGET_OS_IPHONE

#ifdef TARGET_OS_IPHONE

#import <Foundation/Foundation.h>


#import "VoiceRecordWrapper.h"
#import "MicrophoneRecorder.h"


char* cStringCopy(const char* string)
{
    if (string == NULL)
        return NULL;
    
    char* res = (char*)malloc(strlen(string) + 1);
    strcpy(res, string);
    
    return res;
}

static NSString* CreateNSString(const char* string)
{
    if (string != NULL)
        return [NSString stringWithUTF8String:string];
    else
        return [NSString stringWithUTF8String:""];
}


char* VoiceRecordStartMic(int lowcomplex, const char *cfilename, const char *cguid) {
    
        MicrophoneRecorder* realtimeMicAudioProcessingLib_p = [[MicrophoneRecorder alloc] init];
    

    
    NSString *filename = CreateNSString(cfilename);
    NSString *guid = CreateNSString(cguid);
    
    NSString *status = [realtimeMicAudioProcessingLib_p startMicAudioProcessing:lowcomplex withfilepath:filename withGuid:guid];
    
    return cStringCopy([status UTF8String]);
}


bool VoiceRecordStopMic() {
    
        MicrophoneRecorder* realtimeMicAudioProcessingLib_p = [[MicrophoneRecorder alloc] init];
    
    bool status = [realtimeMicAudioProcessingLib_p stopMicAudioProcessing];

    
    return status;
    
}

void VoiceRecordInitMic() {
    
    MicrophoneRecorder* realtimeMicAudioProcessingLib_p = [[MicrophoneRecorder alloc] init];
    
    [realtimeMicAudioProcessingLib_p initMicAudioProcessing];
}

#endif
*/


