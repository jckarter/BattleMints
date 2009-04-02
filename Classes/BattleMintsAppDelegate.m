#import "BattleMintsAppDelegate.h"
#import "BattleMintsView.h"

@implementation BattleMintsAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    [glView unpause];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView pause];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView unpause];
}

- (void)dealloc {
    [window release];
    [glView release];
    [super dealloc];
}

@end
