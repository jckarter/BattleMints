#import "BattleMintsAppDelegate.h"
#import "BattleMintsView.h"

#ifndef BENCHMARK
static const double ANIMATION_INTERVAL = 1.0/60.0;
#else
static const double ANIMATION_INTERVAL = 1.0/15.0;
#endif

@implementation BattleMintsAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	glView.animationInterval = ANIMATION_INTERVAL;
	[glView startAnimation];
}

- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 0.0;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = ANIMATION_INTERVAL;
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
