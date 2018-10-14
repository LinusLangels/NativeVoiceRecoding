//
//  AudioQueueRecorder.m
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2015-11-30.
//  Copyright © 2015 John Philipsson. All rights reserved.
//
// Implementation is based on Apple documentation about Audio gueues
//
// https://developer.apple.com/library/mac/documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/AQRecord/RecordingAudio.html
//
// However, own wav-file writer implementation since Apples included filewriter seems not to work propely.
//
// Audio session is set-up to handle Float32 input. Conversion to signed 16-bit integer are done to be able to write to wav-file.
// Audio processings buffer is still float format [-1, 1]

#include "stdio.h"
#include <math.h>
#import "AudioQueueRecorder.h"
#import "WriteWaveFile.h"
#import "VoiceRecordingUtils.h"

#define VERBOSE_LOGGING 1

#define POW2(x)	((((x)-1)&(x))==0)

int gSampleCounter=0;
float QuephonemeStarttime;
float QuephonemeStoptime;

voiceRec_t *voiceRecQue;

void AudioInputCallback(void * inUserData,  // Custom audio metadata
                        AudioQueueRef inAQ,
                        AudioQueueBufferRef inBuffer,
                        const AudioTimeStamp * inStartTime,
                        UInt32 inNumberPacketDescriptions,
                        const AudioStreamPacketDescription * inPacketDescs);

void DeriveBufferSize (AudioQueueRef audioQueue, AudioStreamBasicDescription ASBDescription, Float64 seconds, UInt32  *outBufferSize);

@implementation AudioQueueRecorder

static AudioQueueRecorder *recorderInstance = nil;

- (id) initWithCallbacks:(StringParameterCallback)logger andPhonemeCallback:(PhonemeParameterCallback) phonemeCallback andCalibrationCallback:(CalibrationCallback)calibrateCallback
{
    self = [super init];
    
    if (self)
    {
        //debug text outprint variables
        m_Logger = logger;
        phonemeDataCallback = phonemeCallback;
        calibrationCallback = calibrateCallback;
        
        if (VERBOSE_LOGGING == 1)
        {
            if (m_Logger != NULL)
            {
                m_Logger("Created audio Queue recorder");
            }
            
            NSLog(@"NSLOG: Created audio Queue recorder\n");
        }
        
        //Setup static reference to be used in audio callbacks.
        recorderInstance = self;
        
        return self;
    }
    
    return nil;
}

+ (id)instance
{
    return recorderInstance;
}

+ (void)removeInstance
{
    recorderInstance = nil;
}

- (bool) isCalibrating
{
    return isCalibratingMicrophone;
}

- (float) runtimeCalibrationFactor
{
    return calibrationFactor;
}

- (CALIBRATION_t*) getCalibrationData
{
    return calibrationData;
}

- (void) callPhonemeCallback:(float)start andStop:(float)stop andPhoneme:(const char*)phonemein
{
    if (phonemeDataCallback != NULL)
    {
        //Thread lock caused by strange parallel execution. (Magic!) ..this fixed that problem.
        dispatch_async(dispatch_get_main_queue(), ^{

            if (phonemeDataCallback != NULL)
            {
                struct PhonemeData phoneme;
                phoneme.start = start;
                phoneme.stop = stop;
                phoneme.phoneme = phonemein;
                
                phonemeDataCallback(&phoneme);
            }
        });
    }
}

- (void)initializeRecording
{
    recordState = (RecordState*)malloc(sizeof(RecordState));
    
    [self setupAudioFormat:&recordState->dataFormat];
    
    if (VERBOSE_LOGGING == 1)
    {
        if (m_Logger != NULL)
        {
            m_Logger("Callback: INIT AUDIO SESSION SETTINGS");
        }
    }
}

- (void)setupAudioFormat:(AudioStreamBasicDescription*)format
{
    format->mSampleRate = 44100.0f;
    format->mFormatID = kAudioFormatLinearPCM;
    format->mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    format->mFramesPerPacket  = 1;
    format->mChannelsPerFrame = 1;
    format->mBytesPerFrame    = 4;
    format->mBytesPerPacket   = 4;
    format->mBitsPerChannel   = 4*8;
}

//- (void)startRecording:(const char*)path
- (void)startRecording:(int)lowComplexProcessing withfilepath:(NSString*)filepath withAlgoNbr:(int)algoNbr  withCalibrationFactor:(float)runtimeCalibrationFactor withCalibration:(bool)calibrate
{
    calibrationFactor = runtimeCalibrationFactor;
    isCalibratingMicrophone = calibrate;
    
    if (isCalibratingMicrophone)
    {
        calibrationData = (CALIBRATION_t*)malloc(sizeof(CALIBRATION_t));
        calibrationData->currentCalibrationScaleFactor = 0.0f;
        calibrationData->maxAbsMean = -1.0f;
    }
    
    //global sample counter in order to write wav-file header
    gSampleCounter=0;
    
    //init algos
    if(lowComplexProcessing == 1)
    {
        voiceRecQue = CalculatePhoenems_init(44100,512,false,algoNbr,true);
    }
    else{
        voiceRecQue = CalculatePhoenems_init(44100,512,true,algoNbr,true);
    }
    
    recordState->currentPacket = 0;
    
    UInt32 bufferSize = 0;
    
    DeriveBufferSize (recordState->queue, recordState->dataFormat, 0.05f, &bufferSize);
    
    OSStatus status;
    status = AudioQueueNewInput(&recordState->dataFormat,
                                AudioInputCallback,
                                recordState,
                                CFRunLoopGetCurrent(),
                                kCFRunLoopCommonModes,
                                0,
                                &recordState->queue);
    
    UInt32 dataFormatSize = sizeof (recordState->dataFormat);
    AudioQueueGetProperty (recordState->queue, kAudioQueueProperty_StreamDescription, &recordState->dataFormat,&dataFormatSize);
    
    #ifdef BUILD_IOS
    if (status == 0) {
        
        for (int i = 0; i < NUM_BUFFERS; i++)
        {
            AudioQueueAllocateBuffer(recordState->queue, bufferSize, &recordState->buffers[i]);
            
            int sampleCount = recordState->buffers[i]->mAudioDataBytesCapacity / sizeof(float);
            float *samples = (float*)recordState->buffers[i]->mAudioData;
            
            //Make sure buffers are zeroed out before we begin recording.
            for(int i=0;i < sampleCount;i++ )
            {
                samples[i] = 0.0f;
            }
            
            AudioTimeStamp stamp;
            stamp.mSampleTime = 0.0f;
            stamp.mFlags = kAudioTimeStampSampleTimeValid;

            AudioQueueEnqueueBufferWithParameters(recordState->queue, recordState->buffers[i], 0, NULL, 0, 0, 0, NULL, &stamp, NULL);
        }
    }
    #else
    //Calculate closet power of 2 bufferlength based on DeriveBufferSize() recommended bufferlength with its time input
    int bufferBaseOf2 = (int) (floor(log2((float)bufferSize)));
    
    int bufferSizePowerOf2Delta = (int)pow(2,bufferBaseOf2)- bufferSize;
    int bufferSizePowerOf2Plus1Delta = (int)pow(2,bufferBaseOf2+1) - bufferSize;
    
    if(abs(bufferSizePowerOf2Delta) < abs(bufferSizePowerOf2Plus1Delta))
    {
        bufferSize = (int)pow(2,bufferBaseOf2);
    }
    else
    {
        bufferSize = (int)pow(2,bufferBaseOf2+1);
    }
    
    if (VERBOSE_LOGGING == 1)
    {

        //debug text outprint
        NSString *message = [NSString stringWithFormat:@"Native: Callback: Power of 2 %d: ", bufferSize];
        m_Logger([message UTF8String]);
    }
    
    
    if (status == 0) {
        
        for (int i = 0; i < NUM_BUFFERS; i++) {
            AudioQueueAllocateBuffer(recordState->queue, bufferSize, &recordState->buffers[i]);
            AudioQueueEnqueueBuffer(recordState->queue, recordState->buffers[i], 0, NULL);
        }
    }
    #endif
    
    if (VERBOSE_LOGGING == 1)
    {
        m_Logger("Callback: Started Recording");
        NSLog(@"Audio Algorithm Init Done");
        
        //debug text outprint
        NSString *message = [NSString stringWithFormat:@"Native: Start Recording %@", filepath];
        m_Logger([message UTF8String]);
        
        //debug text outprint
        message = [NSString stringWithFormat:@"Native: Calibration factor is %f", runtimeCalibrationFactor];
        m_Logger([message UTF8String]);
        
    }
    
    //Allows the wav writer to send debug logs to unity.
    SetWavDebugLog(m_Logger);

    //Open our wav file for writing.
    OpenWavFile([filepath UTF8String]);
    
    #ifdef BUILD_IOS
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    if (!session) printf("ERROR INITIALIZING AUDIO SESSION! \n");
    else
    {
        NSError *error;
        
        [session setCategory:AVAudioSessionCategoryPlayAndRecord error:&error];
        
        if (error) printf("couldn't set audio category!");
        
        
        
        [session setActive:YES error:&error];
        if (error) printf("AudioSession setActive = YES failed");
    }
    #endif
    
    recordState->recording = true;
    
    status = AudioQueueStart(recordState->queue, NULL);
    
    NSLog(@"started audio queue recorder");
}

- (void)stopRecording:(int)algonbr
{
    NSLog(@"stopped audio queue recorder");
    recordState->recording = false;
    
    AudioQueueFlush(recordState->queue);
    AudioQueueStop(recordState->queue, false);
    
    for (int i = 0; i < NUM_BUFFERS; i++) {
        AudioQueueFreeBuffer(recordState->queue, recordState->buffers[i]);
    }
    
    AudioQueueDispose(recordState->queue, true);
    
    free(recordState);
    
    #ifdef BUILD_IOS
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    if (!session) printf("ERROR INITIALIZING AUDIO SESSION! \n");
    else
    {
        NSError *error;
        
        [session setCategory:AVAudioSessionCategoryPlayback error:&error];
        
        if (error) printf("couldn't set audio category!");
        
        
        
        [session setActive:YES error:&error];
        if (error) printf("AudioSession setActive = YES failed");
    }
    #endif
    
    if (isCalibratingMicrophone)
    {
        float scaleFactor;
        float maxABSMean = calibrationData->maxAbsMean;
        
        
        if (maxABSMean < 1.1f)
        {
            scaleFactor = 0.5f*(1.0 / maxABSMean);
        }
        else
        {
            scaleFactor = 1.0f;
        }
       
        calibrationData->currentCalibrationScaleFactor = scaleFactor;
        
        if (calibrationCallback != NULL)
            calibrationCallback(maxABSMean, scaleFactor);
        
        free(calibrationData);
    }
    
    //Done writing audio to file.
    CloseWavFile();
    
    if (VERBOSE_LOGGING == 1)
    {
        if (m_Logger != NULL)
        {
            m_Logger("Callback: Stopped Recording");
        }
        
        //clean up audio algorithms allocation
        NSLog(@"Audio Algorithm Destroy Started");
    }
    
    //destroy all allocation
    
    CalculatePhoenems_destroy(voiceRecQue,algonbr);
}

- (void) destroyRecording
{
    phonemeDataCallback = NULL;
}

-(void) dealloc
{
    if (VERBOSE_LOGGING == 1)
    {
        if (m_Logger != NULL)
        {
            m_Logger("Callback: Destroyed Audio Queue");
        }
    }
    
    phonemeDataCallback = NULL;
    m_Logger = NULL;
    
    //Make sure static reference becomes invalid.
    recorderInstance = nil;
    
    #if  ! __has_feature(objc_arc)
    [super dealloc];
    #endif
}

@end

void AudioInputCallback(void * inUserData,  // Custom audio metadata
                        AudioQueueRef inAQ,
                        AudioQueueBufferRef inBuffer,
                        const AudioTimeStamp * inStartTime,
                        UInt32 inNumberPacketDescriptions,
                        const AudioStreamPacketDescription * inPacketDescs)
{
    RecordState * state = (RecordState*)inUserData;
    

    
    
    AudioQueueRecorder* instance = [AudioQueueRecorder instance];
    
    int sampleCount = inBuffer->mAudioDataBytesCapacity / state->dataFormat.mBytesPerPacket;
    float *samples = (float*)inBuffer->mAudioData;
    
    float processingBuffer[sampleCount];
    short filewriteBuffer[sampleCount];
    
    float calibrateFactor = 1.0f;
    bool isCalibrating = false;
    
    if (instance)
    {
        calibrateFactor = [instance runtimeCalibrationFactor];
        isCalibrating= [instance isCalibrating];
    }
    
    //Would be kind of weird to calibrate without a default calibration value.
    if (isCalibrating)
    {
        calibrateFactor = 1.0f;
    }
    
    for(int i=0;i < sampleCount;i++ )
    {
        processingBuffer[i] = ((float)samples[i]) * calibrateFactor;
        short sample = (short)(samples[i] * 32767.0f * calibrateFactor);
        filewriteBuffer[i] = sample;
    }
    
    if (isCalibrating)
    {
        if (instance)
        {
            CALIBRATION_t *calibrationData = [instance getCalibrationData];
            CalibrateAudio(calibrationData,processingBuffer,10,sampleCount);
        }
    }
    
    QuephonemeStarttime = (((float)gSampleCounter) / (voiceRecQue->fs));
    gSampleCounter = gSampleCounter + sampleCount;
    QuephonemeStoptime = (((float)gSampleCounter) / (voiceRecQue->fs));
    
    //Call phoneme lib HERE
    if(sampleCount > 512)
    {
        //buffer is 512 samples
        voiceRecQue = CalculatePhoenems(processingBuffer,voiceRecQue->signalE,voiceRecQue->vow,voiceRecQue,voiceRecQue->preVoice,voiceRecQue->dec,voiceRecQue->preDist,voiceRecQue->evad);
    }
    
    if (instance)
    {
        [instance callPhonemeCallback:QuephonemeStarttime andStop:QuephonemeStoptime andPhoneme:voiceRecQue->phoneme];
    }
    
    SaveSamples(filewriteBuffer, sizeof(short), sampleCount);
    
    AudioQueueEnqueueBuffer(state->queue, inBuffer, 0, NULL);
}



void DeriveBufferSize (AudioQueueRef audioQueue, AudioStreamBasicDescription ASBDescription, Float64 seconds, UInt32  *outBufferSize)
{
    static const int maxBufferSize = 0x50000;
    
    int maxPacketSize = ASBDescription.mBytesPerPacket;
    if (maxPacketSize == 0) {
        UInt32 maxVBRPacketSize = sizeof(maxPacketSize);
        AudioQueueGetProperty ( audioQueue, kAudioQueueProperty_MaximumOutputPacketSize,&maxPacketSize, &maxVBRPacketSize);
    }
    Float64 numBytesForTime =
    ASBDescription.mSampleRate * maxPacketSize * seconds;
    *outBufferSize = (UInt32) (numBytesForTime < maxBufferSize ? numBytesForTime : maxBufferSize);
}

 




