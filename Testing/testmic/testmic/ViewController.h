//
//  ViewController.h
//  testmic
//
//  Created by Linus Langels on 06/07/16.
//  Copyright © 2016 Plotagon. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
{
}

- (IBAction)initializeBtn:(id)sender;

- (IBAction)startBtn:(id)sender;

- (IBAction)stopBtn:(id)sender;

@property (nonatomic) NSString *path;

@end

