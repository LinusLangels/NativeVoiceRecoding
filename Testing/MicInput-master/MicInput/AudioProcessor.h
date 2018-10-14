//
//  AudioProcessor.h
//  MicInput
//


#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

// return max value for given values
#define max(a, b) (((a) > (b)) ? (a) : (b))
// return min value for given values
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define kOutputBus 0
#define kInputBus 1

// our default sample rate
#define SAMPLE_RATE 44100.00
//#define SAMPLE_RATE 22050

@interface AudioProcessor : NSObject
{
    // Audio unit
    AudioComponentInstance audioUnit;
    
    // Audio buffers
	AudioBuffer audioBuffer;
    
}

@property (readonly) AudioBuffer audioBuffer;
@property (readonly) AudioComponentInstance audioUnit;
//@property (nonatomic) float gain;

-(AudioProcessor*)init;

-(void)initializeAudio;
-(void)processBuffer: (AudioBufferList*) audioBufferList;

// control object

-(NSString*)start:(int)lowComplexProcessing withfilepath:(NSString*)filepath withGuid:(NSString*)guid withAlgoNbr:(int)algoNbr;
-(bool)stop:(int)algoNbr;


// error managment
//-(void)hasError:(int)statusCode:(char*)file:(int)line;

@end
