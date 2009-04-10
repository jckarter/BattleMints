#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@class BattleMintsPauseView;
@class BattleMintsPopupView;

@interface BattleMintsView : UIView
{
@public
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    GLuint viewRenderbuffer, viewFramebuffer;
    
    NSTimer *animationTimer;

    BattleMintsPauseView *pauseView;
    BattleMintsPopupView *popupView;

    BOOL touchStartedAsPause;
}

@property(nonatomic, retain) IBOutlet BattleMintsPauseView *pauseView;
@property(nonatomic, retain) IBOutlet BattleMintsPopupView *popupView;

- (void)pause;
- (void)unpause;
- (void)pauseMenu;
- (void)drawView;

- (void)popUp:(NSString*)text;

@end
