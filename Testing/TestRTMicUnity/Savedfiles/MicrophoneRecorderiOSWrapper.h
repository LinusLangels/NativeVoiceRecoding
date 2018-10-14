//
//  MicrophoneRecorderiOSWrapper.h
//  
//
//  Created by John Philipsson on 2015-12-04.
//
//


#ifndef _MicrophoneRecorderiOSWrapper_h
#define _MicrophoneRecorderiOSWrapper_h




void Bridge_StartRecording(int lowcomplex, int algoNbr, const char *cfilename);
void Bridge_StopRecording();
void Bridge_InitRecording();
void Bridge_Destroy();



@interface MicrophoneRecorderiOSWrapper : NSObject
@end

#endif


