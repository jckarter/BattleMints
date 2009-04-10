#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "BattleMintsView.h"
#import "BattleMintsPauseView.h"
#import "BattleMintsPopupView.h"
#include "battlemints.h"

#ifndef BENCHMARK
const double BATTLEMINTS_ANIMATION_INTERVAL = 1.0/60.0;
#else
const double BATTLEMINTS_ANIMATION_INTERVAL = 1.0/15.0;
#endif

@interface BattleMintsView ()

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

const CGFloat BATTLEMINTS_PAUSE_AREA = 40.0f;

static BOOL _is_pause_touch(BattleMintsView *view, UITouch *touch)
{
    CGPoint location = [touch locationInView:view];
    CGSize bounds = [view bounds].size;
    
    return view->animationTimer
        && (battlemints_pause_flags() & BATTLEMINTS_GAME_ACTIVE)
        && (bounds.width  - location.x <= BATTLEMINTS_PAUSE_AREA)
        && (bounds.height - location.y <= BATTLEMINTS_PAUSE_AREA);
}

static void _report_touch(UIView *view, UITouch *touch)
{
    CGPoint location = [touch locationInView:view];
    CGSize bounds = [view bounds].size;

    battlemints_input(
        (location.x - bounds.width/2)/(bounds.width/4),
        -(location.y - bounds.height/2)/(bounds.height/4),
        [touch tapCount]
    );
}

@implementation BattleMintsView

@synthesize pauseView, popupView;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithCoder:(NSCoder*)coder
{
    if ((self = [super initWithCoder:coder])) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        animationTimer = nil;

        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
            kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
            nil
        ];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
        battlemints_start();
    }
    return self;
}

- (void)tick
{
    battlemints_tick();
    [self drawView];
}

- (void)drawView
{
    [EAGLContext setCurrentContext:context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);

    battlemints_draw();
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)layoutSubviews
{
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}

- (BOOL)createFramebuffer
{
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    //glGenRenderbuffersOES(1, &depthRenderbuffer);
    //glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
    //glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
    //glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}


- (void)destroyFramebuffer
{
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
//    if(depthRenderbuffer) {
//        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
//        depthRenderbuffer = 0;
//    }
}

- (void)unpause
{
    if (!animationTimer) {
        animationTimer = [[NSTimer
            scheduledTimerWithTimeInterval:BATTLEMINTS_ANIMATION_INTERVAL
            target:self
            selector:@selector(tick)
            userInfo:nil
            repeats:YES
        ] retain];
    }
}

- (void)pause
{
    if (animationTimer) {
        [animationTimer invalidate];
        [animationTimer release];
        animationTimer = nil;
    }
}

- (void)pauseMenu
{
    [[NSBundle mainBundle] loadNibNamed:@"BattleMintsPauseView"
                           owner:self
                           options:[NSDictionary dictionary]];
    [self addSubview:self.pauseView];
}

- (void)popUp:(NSString*)text
{
    [[NSBundle mainBundle] loadNibNamed:@"BattleMintsPopupView"
                           owner:self
                           options:[NSDictionary dictionary]];
    self.popupView.popupText.text = text;
    [self addSubview:self.popupView];
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch *touch = [touches anyObject];
    touchStartedAsPause = _is_pause_touch(self, touch);
    if (!touchStartedAsPause)
        _report_touch(self, touch);
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch *touch = [touches anyObject];
    if (!touchStartedAsPause)
        _report_touch(self, touch);
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch *touch = [touches anyObject];

    BOOL touchEndedAsPause = _is_pause_touch(self, touch);
    if (touchStartedAsPause && touchEndedAsPause)
        [self pauseMenu];
    else
        battlemints_input(0.0, 0.0, 0);
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    battlemints_input(0.0, 0.0, 0);
}

- (void)dealloc
{
    [self pause];
    
    [EAGLContext setCurrentContext:context];
    battlemints_finish();
    [EAGLContext setCurrentContext:nil];
    
    [context release];  
    [super dealloc];
}

@end
