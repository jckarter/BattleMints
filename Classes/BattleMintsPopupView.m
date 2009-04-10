#import "BattleMintsPopupView.h"
#import "BattleMintsView.h"
#import "BattleMintsAppDelegate.h"
#import "battlemints.h"

@implementation BattleMintsPopupView

@synthesize popupText;

- (void)awakeFromNib
{
    [super awakeFromNib];

    [self addTarget:self action:@selector(dismiss:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)willMoveToSuperview:(UIView *)newSuperview
{
    if (self.superview && [self.superview respondsToSelector:@selector(unpause)])
        [self.superview unpause];
    if (newSuperview) {
        if ([newSuperview respondsToSelector:@selector(pause)])
            [newSuperview pause];

        CGRect frame = self.frame;
        CGRect superframe = newSuperview.frame;
        
        frame.origin.x = (superframe.size.width  - frame.size.width ) * 0.5f;
        frame.origin.y = (superframe.size.height - frame.size.height) * 0.5f;

        self.frame = frame;
    }
}

- (IBAction)dismiss:(id)sender
{
    if (self.superview && [self.superview respondsToSelector:@selector(popupView)])
        [self.superview setPopupView:nil];
    [self removeFromSuperview];
}

@end

void
battlemints_ui_pop_up(char const *text)
{
    BattleMintsView *gameView = [[[UIApplication sharedApplication] delegate] glView];

    [gameView popUp:[NSString stringWithUTF8String:text]];

}
