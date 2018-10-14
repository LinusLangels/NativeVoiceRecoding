//
//  OSXRecorder.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-12-15.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#ifndef OSXRecorder_h
#define OSXRecorder_h

#import <Foundation/Foundation.h>
#import "RecorderProtocol.h"
#import "AudioQueueRecorder.h"

@interface OSXRecorder : NSObject<RecorderProtocol>
{
    StringParameterCallback logCallback;
    PhonemeParameterCallback phonemeCallback;
    CalibrationCallback calibrationCallback;
    AudioQueueRecorder *audioRecorder;
}

@end


#endif /* OSXRecorder_h */
