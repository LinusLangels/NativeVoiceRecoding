//
//  VoiceRecordWrapper.m
//  
//
//  Created by John Philipsson on 2015-10-19.
//
//

#import <Foundation/Foundation.h>


#import "VoiceRecordWrapper.h"
#import "MicrophoneRecorderStatic.h"

static MicrophoneRecorderStatic *RT_p;

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


void Bridge_StartRecording(int lowcomplex, const char *cfilename, const char *cguid, int algonbr )
{
    
    
    NSString *filename = CreateNSString(cfilename);
    NSString *guid = CreateNSString(cguid);
    
    NSString *status = [RT_p startMicAudioProcessing:lowcomplex withfilepath:filename withGuid:guid withAlgoNbr:algonbr];
    
    //return cStringCopy([status UTF8String]);
}


bool Bridge_StopRecording(int algonbr) {
    

    
    bool *status = [RT_p stopMicAudioProcessing:algonbr];

    
    return status;
    
}

void Bridge_InitRecording()
{
    
     RT_p = [[MicrophoneRecorderStatic alloc] init];
    
    [RT_p initMicAudioProcessing];
}


void Bridge_Destroy()
{

}


