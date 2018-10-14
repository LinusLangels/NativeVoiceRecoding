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
