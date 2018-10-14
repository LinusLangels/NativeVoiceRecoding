//
//  MicrophoneRecorder.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-12-15.
//  Copyright © 2015 John Philipsson. All rights reserved.
//

#ifndef MicrophoneRecorder_h
#define MicrophoneRecorder_h

void Bridge_InitRecording();

void Bridge_StartRecording(int complexity, const char* path, int algoNumber);

void Bridge_StopRecording(int algoNumber);

void Bridge_Destroy();

#endif /* MicrophoneRecorder_h */
