#import <UIKit/UIKit.h>

@interface BattleMintsPopupView : UIControl
{
    IBOutlet UILabel *popupText;
}

@property(readonly) UILabel *popupText;

- (IBAction)dismiss:(id)sender;

@end
