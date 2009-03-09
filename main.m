#import <UIKit/UIKit.h>

@interface BattleMintsDummy : NSObject {}
- (void)whatever:(id)foo;
@end
@implementation BattleMintsDummy : NSObject {}
- (void)whatever:(id)foo { [NSThread exit]; }
@end

#ifdef RUN_TESTS

void test_sphere_line(void);

#endif

int main(int argc, char *argv[])
{    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [NSThread detachNewThreadSelector:@selector(whatever:)
              toTarget:[[[BattleMintsDummy alloc] init] autorelease]
              withObject:nil];
#ifdef RUN_TESTS
    test_sphere_line();
    int retVal = 0;
#else
    int retVal = UIApplicationMain(argc, argv, nil, nil);
#endif
    [pool release];
    return retVal;
}
