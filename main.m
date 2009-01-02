#import <UIKit/UIKit.h>

@interface BattleMintsDummy : NSObject {}
- (void)whatever:(id)foo;
@end
@implementation BattleMintsDummy : NSObject {}
- (void)whatever:(id)foo { [NSThread exit]; }
@end

int main(int argc, char *argv[])
{    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [NSThread detachNewThreadSelector:@selector(whatever:)
              toTarget:[[[BattleMintsDummy alloc] init] autorelease]
              withObject:nil];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
