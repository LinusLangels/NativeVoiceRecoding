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
