//
//  MicrophoneRecorderiOS2.h
//  MicrophoneRecorderiOS2
//
//  Created by John Philipsson on 2015-12-07.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#import <Foundation/Foundation.h>






void Native_InitRecording();
void Native_StartRecording(int complexity, int algoNumber, const char* path, const char* guid);
void Native_StopRecording(int algoNumber);
void Native_Destroy();

@interface MicrophoneRecorderiOS2 : NSObject


@end
