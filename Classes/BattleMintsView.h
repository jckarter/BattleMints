#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@class BattleMintsPauseView;

@interface BattleMintsView : UIView
{
@public
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    GLuint viewRenderbuffer, viewFramebuffer;
    
    NSTimer *animationTimer;

    BattleMintsPauseView *pauseView;

    BOOL touchStartedAsPause;
}

@property(nonatomic, retain) IBOutlet BattleMintsPauseView *pauseView;

- (void)pause;
- (void)unpause;
- (void)pauseMenu;
- (void)drawView;

@end
