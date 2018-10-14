//
//  VoiceRecordWrapper.h
//  
//
//  Created by John Philipsson on 2015-10-19.
//
//

#ifndef _VoiceRecordWrapper_h
#define _VoiceRecordWrapper_h




void Bridge_StartRecording(int lowcomplex, const char *cfilename, const char *cguid, int algonbr );
bool Bridge_StopRecording(int algonbr);
void Bridge_InitRecording();
void Bridge_Destroy();
char* cStringCopy(const char* string);
static NSString* CreateNSString(const char* string);



@interface VoiceRecordWrapper : NSObject
@end

#endif


