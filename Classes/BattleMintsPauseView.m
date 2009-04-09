#import "BattleMintsPauseView.h"
#import "BattleMintsView.h"
#import "battlemints.h"

@implementation BattleMintsPauseView

- (void)awakeFromNib
{
    [super awakeFromNib];

    int flags = battlemints_pause_flags();

    if (flags & BATTLEMINTS_START_MAP)
        [goToStartButton removeFromSuperview];
    if (flags & (BATTLEMINTS_START_MAP | BATTLEMINTS_HUB_MAP))
        [goToHubButton removeFromSuperview];
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

- (IBAction)goToHub:(id)sender
{
    battlemints_go_to_hub();
    [self unpause:sender];
}

- (IBAction)goToStart:(id)sender
{
    battlemints_go_to_start();
    [self unpause:sender];
}

- (IBAction)unpause:(id)sender
{
    if (self.superview && [self.superview respondsToSelector:@selector(pauseView)])
        [self.superview setPauseView:nil];
    
    [self removeFromSuperview];
}

@end
