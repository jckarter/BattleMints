#import <UIKit/UIKit.h>

@interface BattleMintsPauseView : UIView
{
    IBOutlet UIButton *goToHubButton, *goToStartButton, *unpauseButton;
}

- (IBAction)goToHub:(id)sender;
- (IBAction)goToStart:(id)sender;
- (IBAction)unpause:(id)sender;

@end
