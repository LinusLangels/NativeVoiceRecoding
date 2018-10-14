//
//  MyAudioWindow.h
//  MicInput
//


#import <UIKit/UIKit.h>
@class AudioProcessor;

@interface MyAudioViewController : UIViewController {

}
@property (retain, nonatomic) IBOutlet UISwitch *audioSwitch;
@property (retain, nonatomic) IBOutlet UILabel *gainValueLabel;
@property (retain, nonatomic) AudioProcessor *audioProcessor;
@property (retain, nonatomic) IBOutlet UILabel *topLabel;

// actions
- (IBAction)riseGain:(id)sender;
- (IBAction)lowerGain:(id)sender;
- (IBAction)audioSwitch:(id)sender;

// ui element manipulation
- (void)setGainLabelValue:(float)gainValue;
- (void)showLabelWithText:(NSString*)labelText;

@end
