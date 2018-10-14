//
//  MicrophoneProperties.h
//  Unity-iPhone
//
//  Created by PlotAMac on 28/09/15.
//
//

#import <Foundation/Foundation.h>

typedef void ( *CALLBACK )( bool allowed );
static CALLBACK callback;

void SetMicAccessCallback(CALLBACK handler) {
    callback = handler;
}

@interface MicrophoneProperties : NSObject

- (void) CheckMicrophoneAccess;

@end
