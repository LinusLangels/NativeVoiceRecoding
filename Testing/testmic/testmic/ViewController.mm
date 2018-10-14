//
//  ViewController.m
//  testmic
//
//  Created by Linus Langels on 06/07/16.
//  Copyright © 2016 Plotagon. All rights reserved.
//

#import "ViewController.h"
//#import "WriteWaveFile.h"
//#import <AVFoundation/AVFoundation.h>


extern "C" void Bridge_InitRecording();

extern "C" void Bridge_StartRecording(int complexity, const char* path, int algoNumber, float calibrationFactor, bool calibrate);

extern "C" void Bridge_StopRecording(int algoNumber);

extern "C" void Bridge_Destroy();


@interface ViewController ()

@end

@implementation ViewController

@synthesize path = _path;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)initializeBtn:(id)sender {
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    //Get app documents directory.
    NSArray *URLs = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
    NSString *documentsDir = [URLs objectAtIndex:0];
    
    NSURL *outputPath = [NSURL URLWithString:[NSString stringWithFormat:@"%@test.wav", documentsDir]];
    NSString *wavPath = [outputPath path];
    
    BOOL isDir;
    if ([fileManager fileExistsAtPath:wavPath isDirectory:&isDir])
    {
        NSError *error = nil;
        [fileManager removeItemAtPath:wavPath error:&error];
    }
    
    self.path = wavPath;
    
    Bridge_InitRecording();
    
    NSLog(@"init");
}

- (IBAction)startBtn:(id)sender
{
    NSLog(@"start");
    
    Bridge_StartRecording(0, [self.path UTF8String], 0, 1.0f, false);
}

- (IBAction)stopBtn:(id)sender
{
    NSLog(@"stop");
    
    Bridge_StopRecording(0);
    
    Bridge_Destroy();
}

@end
