//
//  VoiceRecordWrapper.m
//  
//
//  Created by John Philipsson on 2015-10-19.
//
//

#import <Foundation/Foundation.h>


#import "VoiceRecordWrapper.h"
#import "realtimeMicAudioProcessingLib.h"



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
    
        realtimeMicAudioProcessingLib* realtimeMicAudioProcessingLib_p = [[realtimeMicAudioProcessingLib alloc] init];
    
      VoiceRecordWrapper * check = [[VoiceRecordWrapper alloc] init];
    
    
    [check SetCompleteHandler:^() {
        [check release];
    }];
    
    NSString *filename = CreateNSString(cfilename);
    NSString *guid = CreateNSString(cguid);
    
    //Callback to mono letting us know recording status
    cb(false);
    
    NSString *status = [realtimeMicAudioProcessingLib_p startMicAudioProcessing:lowcomplex withfilepath:filename withGuid:guid];
    
    //Callback to mono letting us know recording status
    cb(true);
    
    return cStringCopy([status UTF8String]);
}


bool VoiceRecordStopMic() {
    
        realtimeMicAudioProcessingLib* realtimeMicAudioProcessingLib_p = [[realtimeMicAudioProcessingLib alloc] init];
    
    bool *status = [realtimeMicAudioProcessingLib_p stopMicAudioProcessing];
    
    //This will release the  object.
    //_callback();
    //Block_release(_callback);
    
    return status;
    
}

void VoiceRecordInitMic() {
    
    realtimeMicAudioProcessingLib* realtimeMicAudioProcessingLib_p = [[realtimeMicAudioProcessingLib alloc] init];
    
    [realtimeMicAudioProcessingLib_p initMicAudioProcessing];
}

@implementation VoiceRecordWrapper {
    void (^_callback) ();
}

- (void)SetCompleteHandler:(void (^) ()) handler {
    _callback = Block_copy(handler);
}

@end

/*

//
//  MicrophoneProperties.m
//  Unity-iPhone
//
//  Created by PlotAMac on 28/09/15.
//
//

#import "MicrophoneProperties.h"
#import <AVFoundation/AVFoundation.h>

void CheckMicAccess()
{
    MicrophoneProperties * check = [[MicrophoneProperties alloc] init];
    
    [check SetCompleteHandler:^() {
        [check release];
    }];
    
    [check CheckMicrophoneAccess];
}

@implementation MicrophoneProperties {
    void (^_callback) ();
}

-(void) CheckMicrophoneAccess
{
    [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted) {
        if (granted)
        {
            //Callback to mono letting us know mic access status.
            cb(true);
        }
        else
        {
            //Callback to mono letting us know mic access status.
            cb(false);
            
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Microphone Access is off"
                                                            message:@"In order to use voice recording you must allow microphone access in Settings > Privacy > Microphone"
                                                           delegate:nil
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil];
            [alert show];
            
        }
        
        
        //This will release the MicrophoneProperties object.
        _callback();
        Block_release(_callback);
        
    }];
}

- (void)SetCompleteHandler:(void (^) ()) handler {
    _callback = Block_copy(handler);
}

@end

*/

