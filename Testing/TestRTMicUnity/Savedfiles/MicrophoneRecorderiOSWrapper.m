//
//  MicrophoneRecorderiOSWrapper.m
//  
//
//  Created by John Philipsson on 2015-12-04.
//
//

#import <Foundation/Foundation.h>


#import "MicrophoneRecorderiOSWrapper.h"
#import "MicrophoneRecorderiOS2.h"


void Bridge_StartRecording(int lowcomplex, int algoNbr, const char *cfilename, const char *cguid) {
    Native_StartRecording(lowcomplex,algoNbr,cfilename, cguid);
}


void Bridge_StopRecording() {
    Native_StopRecording(0);
}

void Bridge_InitRecording() {
    Native_InitRecording();
}

void Bridge_Destroy() {
    Native_Destroy();
}



