//
//  AudioProcessor.m
//  MicInput
//

#import <AVFoundation/AVFoundation.h>
//#import <UIKit/UIKit.h>
#include "VoiceRecordLib.h"
#include "WriteWaveFile.h"
#import "AudioProcessor.h"
#import "MicrophoneRecorder.h"

typedef void ( *CALLBACK )( char *allowed );

static CALLBACK unityCallback;


void SetRecordingPhonemeCallback(CALLBACK delegate) {
    unityCallback = delegate;
}

//void SetRecordingPhonemeCallback(CALLBACK test);


voiceRec_t *voiceRec;

float count2 = 0;
int nbr_sek = 0;

FILE *inputaudiodataptr;
uint32_t  BytesRecorded;

uint32_t wordRecorded;
float phonemeStarttime;
float phonemeStoptime;


#pragma mark Recording callback

static OSStatus recordingCallback(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    
    
    // the data gets rendered here
    AudioBuffer buffer;
    
    // a variable where we check the status
    OSStatus status;
    
    /**
     This is the reference to the object who owns the callback.
     */
    AudioProcessor *audioProcessor = (AudioProcessor*) inRefCon;
    
    /**
     on this point we define the number of channels, which is mono
     for the iphone. the number of frames is usally 512 or 1024.
     */
    buffer.mDataByteSize = inNumberFrames * 2; // sample size
    buffer.mNumberChannels = 1; // one channel
    buffer.mData = malloc( inNumberFrames * 2 ); // buffer size
    
    // we put our buffer into a bufferlist array for rendering
    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0] = buffer;
    
    // render input and check for error
    status = AudioUnitRender([audioProcessor audioUnit], ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, &bufferList);
    //[audioProcessor hasError:status:__FILE__:__LINE__];
    
    // process the bufferlist in the audio processor
    [audioProcessor processBuffer:&bufferList];
    
    // clean up the buffer
    free(bufferList.mBuffers[0].mData);
    
    
    return noErr;
}

#pragma mark Playback callback

static OSStatus playbackCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData) {
    
    /**
     This is the reference to the object who owns the callback.
     */
    AudioProcessor *audioProcessor = (AudioProcessor*) inRefCon;
    
    // iterate over incoming stream an copy to output stream
    for (int i=0; i < ioData->mNumberBuffers; i++) {
        AudioBuffer buffer = ioData->mBuffers[i];
        
        // find minimum size
        UInt32 size = min(buffer.mDataByteSize, [audioProcessor audioBuffer].mDataByteSize);
        
        // copy buffer to audio buffer which gets played after function return
        memcpy(buffer.mData, [audioProcessor audioBuffer].mData, size);
        
        // set data size
        buffer.mDataByteSize = size;
    }
    
    
    return noErr;
}

#pragma mark objective-c class

@implementation AudioProcessor
@synthesize audioUnit, audioBuffer; //, gain;

-(AudioProcessor*)init
{
    self = [super init];
    if (self) {
        //gain = 0;
        [self initializeAudio];
    }
    return self;
}

-(void)initializeAudio
{
    
    //Added this extra audio init code, since mic callback does not respond, only lsp callback
    //This extra audio init code, is only needed when integrating
    //and used with Unity, not needed for pure xcode (osx , ios) projects. Then you get both callbacks
    
#ifndef RTUNITYEDITOR
    
    AudioSessionInitialize(NULL,
                           NULL,
                           nil,
                           ( void *)(self)
                           );
    
    UInt32 sessionCategory = kAudioSessionCategory_PlayAndRecord;
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory,
                            sizeof(sessionCategory),
                            &sessionCategory
                            );
    
    AudioSessionSetActive(true);
    //end of extra Unity code
 
#endif
    
    
    OSStatus status;
    
    // We define the audio component
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output; // we want to ouput
#ifndef RTUNITYEDITOR
    desc.componentSubType = kAudioUnitSubType_RemoteIO; // we want in and ouput
#endif
    desc.componentFlags = 0; // must be zero
    desc.componentFlagsMask = 0; // must be zero
    desc.componentManufacturer = kAudioUnitManufacturer_Apple; // select provider
    
    // find the AU component by description
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
    
    // create audio unit by component
    status = AudioComponentInstanceNew(inputComponent, &audioUnit);
    
    //[self hasError:status:__FILE__:__LINE__];
    
    // define that we want record io on the input bus
    UInt32 flag = 1;
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_EnableIO, // use io
                                  kAudioUnitScope_Input, // scope to input
                                  kInputBus, // select input bus (1)
                                  &flag, // set flag
                                  sizeof(flag));
    //[self hasError:status:__FILE__:__LINE__];
    
    // define that we want play on io on the output bus
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_EnableIO, // use io
                                  kAudioUnitScope_Output, // scope to output
                                  kOutputBus, // select output bus (0)
                                  &flag, // set flag
                                  sizeof(flag));
    //[self hasError:status:__FILE__:__LINE__];
    
    /*
     We need to specifie our format on which we want to work.
     We use Linear PCM cause its uncompressed and we work on raw data.
     for more informations check.
     
     We want 16 bits, 2 bytes per packet/frames at 44khz
     */
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate			= SAMPLE_RATE;
    audioFormat.mFormatID			= kAudioFormatLinearPCM;
    audioFormat.mFormatFlags		= kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    audioFormat.mFramesPerPacket	= 1;
    audioFormat.mChannelsPerFrame	= 1;
    audioFormat.mBitsPerChannel		= 16;
    audioFormat.mBytesPerPacket		= 2;
    audioFormat.mBytesPerFrame		= 2;
    
    
    
    // set the format on the output stream
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &audioFormat,
                                  sizeof(audioFormat));
    
    //[self hasError:status:__FILE__:__LINE__];
    
    // set the format on the input stream
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  kOutputBus,
                                  &audioFormat,
                                  sizeof(audioFormat));
    //[self hasError:status:__FILE__:__LINE__];
    
    
    

     //We need to define a callback structure which holds
     //a pointer to the recordingCallback and a reference to
     //the audio processor object
    
    AURenderCallbackStruct callbackStruct;
    
    
     //We do the same on the output stream to hear what is coming
     //from the input stream
    callbackStruct.inputProc = playbackCallback;
    callbackStruct.inputProcRefCon = self;
    
    // set playbackCallback as callback on our renderer for the output bus
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Global,
                                  kOutputBus,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    //[self hasError:status:__FILE__:__LINE__];
    
    
    
    // set recording callback
    callbackStruct.inputProc = recordingCallback; // recordingCallback pointer
    callbackStruct.inputProcRefCon = self;
    
    // set input callback to recording callback on the input bus
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Global,
                                  kInputBus,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    //[self hasError:status:__FILE__:__LINE__];
    
    
    // reset flag to 0
    flag = 0;
     //we need to tell the audio unit to allocate the render buffer,
     //that we can directly write into it.
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_ShouldAllocateBuffer,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &flag,
                                  sizeof(flag));
    
    
    
    
    /*
     we set the number of channels to mono and allocate our block size to
     1024 bytes.
     */
    audioBuffer.mNumberChannels = 1;
    //audioBuffer.mDataByteSize = 512 * 2;
    //audioBuffer.mData = malloc( 512 * 2 );
    audioBuffer.mDataByteSize = 512 * 2;
    audioBuffer.mData = malloc( 512 * 2);
    
    // Initialize the Audio Unit and cross fingers =)
    status = AudioUnitInitialize(audioUnit);
    //[self hasError:status:__FILE__:__LINE__];
    
    NSLog(@"Audio Unit Init Done");
    
    
    
}

#pragma mark controll stream

-(NSString*)start:(int)lowComplexProcessing withfilepath:(NSString*)filepath withGuid:(NSString*)guid withAlgoNbr:(int)algoNbr
{

    
    //create file to write
    /**
     */
    NSString *filetypeString = @".wav";
    NSString *str = [NSString stringWithFormat: @"%@/%@%@", filepath, guid, filetypeString];
    
    const char *c = [str UTF8String];
    inputaudiodataptr = fopen(c,"w+");
    if(inputaudiodataptr == NULL)
    {
        return @"Directory doesn't exists, file can't be created";
    }
        
    BytesRecorded=0;
    
    wordRecorded=0;
    phonemeStarttime=0;
    phonemeStoptime=0;


     NSLog(@"Path is: %@\n", str);
    
    //end create file
    
    
    NSLog(@"Audio Algorithm Init Started");
    
    //init algos
    if(lowComplexProcessing == 1)
    {
        voiceRec = CalculatePhoenems_init(44100,512,false,algoNbr);
    }
    else{
        voiceRec = CalculatePhoenems_init(44100,512,true,algoNbr);
    }
    
    
    
    NSLog(@"Audio Algorithm Init Done");
    count2 = 0;
    nbr_sek = 0;
    
    // start the audio unit.
    OSStatus status = AudioOutputUnitStart(audioUnit);
    //[self hasError:status:__FILE__:__LINE__];
    
    return @"File created for recording";
    
}

-(bool)stop:(int)algoNbr
{

    // stop the audio unit
    OSStatus status = AudioOutputUnitStop(audioUnit);
    //[self hasError:status:__FILE__:__LINE__];
    
    //clean up audio algorithms allocation
    NSLog(@"Audio Algorithm Destroy Started");
    
    //destroy all allocation
    
    CalculatePhoenems_destroy(voiceRec,algoNbr);
    
    
    NSLog(@"Audio Algorithm Destroy Done!");
    
    WAV_HEADER_DATA wav_header;
    WAV_FORMAT_DATA PCMfmt;
    
    wav_header.TotalHeaderSize = 44;
    PCMfmt.BitsPerSample = 16;
    PCMfmt.Channels = 1;
    PCMfmt.SampleRate = 44100;
    
    uint8_t * header = (uint8_t *)malloc(wav_header.TotalHeaderSize * sizeof(uint8_t));
    fseek(inputaudiodataptr, 0, SEEK_SET);
    
    CreateHeaderToWave(header, BytesRecorded, PCMfmt);
    
    fwrite(header, sizeof(int8_t), wav_header.TotalHeaderSize, inputaudiodataptr);
    free(header);
    fseek(inputaudiodataptr, wav_header.TotalHeaderSize, SEEK_SET);
    
    
    fclose(inputaudiodataptr);
    
    return true;
 
}



#pragma mark processing

-(void)processBuffer: (AudioBufferList*) audioBufferList
{
    
    int processingDoneFlag = 0;
    float timeDurationForAudioFrameInSec = 0;
    
    
    AudioBuffer sourceBuffer = audioBufferList->mBuffers[0];
    
    // we check here if the input data byte size has changed
    if (audioBuffer.mDataByteSize != sourceBuffer.mDataByteSize) {
        // clear old buffer
        free(audioBuffer.mData);
        // assing new byte size and allocate them on mData
        audioBuffer.mDataByteSize = sourceBuffer.mDataByteSize;
        audioBuffer.mData = malloc(sourceBuffer.mDataByteSize);
    }
    
    //allocate editing buffer for range [-1,1]
    float editbuffer2[sourceBuffer.mDataByteSize];
    float processingBuffer[sourceBuffer.mDataByteSize];

    
    //editing buffer (original for range [-32767, 32768]
    SInt16 *editBuffer = audioBufferList->mBuffers[0].mData;
    
    
    // loop over every frame
    int count=0;
    
    for (int nb = 0; nb < (audioBufferList->mBuffers[0].mDataByteSize / 2); nb++) {
        
        // we need more accuracy in our calculation so we calculate with doubles
        double gainSample = ((double)editBuffer[nb]) / 32767.0;
        editbuffer2[nb] = (float)gainSample;
        processingBuffer[nb] = (float)gainSample;
        count++;
    }
  
    phonemeStarttime = (((float)wordRecorded) / (voiceRec->fs));

    wordRecorded=wordRecorded + count;
    BytesRecorded=BytesRecorded+(count*2);
    
    phonemeStoptime = (((float)wordRecorded) / (voiceRec->fs));
    
    // Call processing
    if(sourceBuffer.mDataByteSize == 1024)
    {
        //buffer is 512 samples
        voiceRec = CalculatePhoenems(editbuffer2,voiceRec->signalE,voiceRec->vow,voiceRec,voiceRec->preVoice,voiceRec->dec,voiceRec->preDist);
        count2 = count2 + 1;
        processingDoneFlag = 1;
    }
    if(sourceBuffer.mDataByteSize == 2048)
    {
        //buffer is 1024 samples
        voiceRec = CalculatePhoenems(editbuffer2,voiceRec->signalE,voiceRec->vow,voiceRec,voiceRec->preVoice,voiceRec->dec,voiceRec->preDist);
        
        voiceRec = CalculatePhoenems(editbuffer2+512,voiceRec->signalE,voiceRec->vow,voiceRec,voiceRec->preVoice,voiceRec->dec,voiceRec->preDist);
        
        count2 = count2 + 2;
        processingDoneFlag = 1;
    }
    if(sourceBuffer.mDataByteSize > (2048+1024))
    {
        //buffer is > 1536 samples
        voiceRec = CalculatePhoenems(editbuffer2,voiceRec->signalE,voiceRec->vow,voiceRec,voiceRec->preVoice,voiceRec->dec,voiceRec->preDist);
        
        voiceRec = CalculatePhoenems(editbuffer2+1024,voiceRec->signalE,voiceRec->vow,voiceRec,voiceRec->preVoice,voiceRec->dec,voiceRec->preDist);
        
        count2 = count2 + ((float) sourceBuffer.mDataByteSize / 1024);
        processingDoneFlag = 1;
        
    }
    else{
        if(processingDoneFlag == 0)
        {
          NSLog(@"Bufferlength not supported ");
          NSLog(@"Buffersize in bytes: %d\n",(int) sourceBuffer.mDataByteSize);
        }
    }
    
    timeDurationForAudioFrameInSec = (((float)sourceBuffer.mDataByteSize) / (2*voiceRec->fs));
    
    
   
    NSString *passingPhonemeAndTomeStringToUnity = [NSString stringWithFormat: @"%@ %f %f", [NSString stringWithUTF8String: voiceRec->phoneme], phonemeStarttime, phonemeStoptime];
    
    const char *passingPhonemeAndTomeStringToUnityC = [passingPhonemeAndTomeStringToUnity UTF8String];
    
    //set phoneme and trigger callback function to Unity
    //cb((char*)passingPhonemeAndTomeStringToUnityC);
    
    NSLog(@"Buffersize in bytes: %d\n",(int) sourceBuffer.mDataByteSize);
    NSLog(@"Start in sec: %f\n",(float) phonemeStarttime);
    NSLog(@"Stop in sec: %f\n",(float) phonemeStoptime);
    NSLog(@"Buffersize in sec: %f\n",(float) timeDurationForAudioFrameInSec);
    
    
    //run speech processing effect nbr 1 = echo, can't be run if realtime buffer is larger than 10000 (saftey)
    if((voiceRec->audioFXon == 1) && (count < 10000))
    {
        echoGenereation(voiceRec->dafx, processingBuffer, count, voiceRec->fs, 0.3);
    }
    
    
    
    // copy incoming audio data to the audio buffer, to lsp
    // if loop back un-processed data, un-comment line below.
    //memcpy(audioBuffer.mData, audioBufferList->mBuffers[0].mData, audioBufferList->mBuffers[0].mDataByteSize);
    
    
    // write back in range [-32767,32768], if processed buffer should be written back
    // however, not done now, since it is downsampled.
    // input is looped back instead and written to file.
    for (int nb = 0; nb < (audioBufferList->mBuffers[0].mDataByteSize / 2); nb++) {
        
        editBuffer[nb] = (SInt16) ((processingBuffer[nb])*32767);
    }
    

    //if loopback processed data to lsp un-comment line below
    memcpy(audioBuffer.mData, editBuffer, audioBufferList->mBuffers[0].mDataByteSize);

    
    //write to file
    fwrite(editBuffer, sizeof(short), (audioBufferList->mBuffers[0].mDataByteSize / 2), inputaudiodataptr);
}

#pragma mark Error handling
/*
-(void)hasError:(int)statusCode:(char*)file:(int)line 
{
    if (statusCode) {
        printf("Error Code responded %d in file %s on line %d\n", statusCode, file, line);
        exit(-1);
    }
}
*/

@end
