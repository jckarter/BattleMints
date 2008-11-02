//
//  BattleMintsAppDelegate.h
//  BattleMints
//
//  Created by Joe Groff on 10/26/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BattleMintsView;

@interface BattleMintsAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow *window;
    BattleMintsView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet BattleMintsView *glView;

@end

