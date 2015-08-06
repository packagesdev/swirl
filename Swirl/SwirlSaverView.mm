
#import "SwirlSaverView.h"

#import "SWIRLConfigurationWindowController.h"

#import "SWIRLUserDefaults+Constants.h"

#import "SWIRLSettings.h"

#include "xswirl.h"

#import <OpenGL/gl.h>

@interface SwirlSaverView ()
{
	BOOL _preview;
	BOOL _mainScreen;
	
	BOOL _OpenGLIncompatibilityDetected;
	
	NSOpenGLView *_openGLView;
	
	swirl *_swirl;
	
	// Preferences
	
	SWIRLConfigurationWindowController *_configurationWindowController;
}

+ (void)_initializeSwirl:(swirl *)inSwirl withSettings:(SWIRLSettings *)inSettings;

@end

@implementation SwirlSaverView

+ (void)_initializeSwirl:(swirl *)inSwirl withSettings:(SWIRLSettings *)inSettings
{
	inSwirl->numberOfKnots=(int)inSettings.numberOKnots;
	
	inSwirl->supportTwoPlanes=(inSettings.twoPlaneSupport==YES);
}

- (instancetype)initWithFrame:(NSRect)frameRect isPreview:(BOOL)isPreview
{
	self=[super initWithFrame:frameRect isPreview:isPreview];
	
	if (self!=nil)
	{
		_preview=isPreview;
		
		if (_preview==YES)
			_mainScreen=YES;
		else
			_mainScreen= (NSMinX(frameRect)==0 && NSMinY(frameRect)==0);
		
		[self setAnimationTimeInterval:1.0/60.0];
	}

	return self;
}
	
#pragma mark -

- (void) setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	
	if (_openGLView!=nil)
		[_openGLView setFrameSize:newSize];
}

#pragma mark -

- (void) drawRect:(NSRect) inFrame
{
	[[NSColor blackColor] set];
	
	NSRectFill(inFrame);
}

#pragma mark -

- (void)startAnimation
{
	_OpenGLIncompatibilityDetected=NO;
	
	[super startAnimation];
	
	NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
	ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
	
	BOOL tBool=[tDefaults boolForKey:SWIRLUserDefaultsMainDisplayOnly];
	
	if (tBool==YES && _mainScreen==NO)
		return;
	
	// Add OpenGLView
	
	NSOpenGLPixelFormatAttribute attribs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize,(NSOpenGLPixelFormatAttribute)8,
		NSOpenGLPFAMinimumPolicy,
		(NSOpenGLPixelFormatAttribute)0
	};
	
	NSOpenGLPixelFormat *tFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	
	if (tFormat==nil)
	{
		_OpenGLIncompatibilityDetected=YES;
		return;
	}
	_openGLView = [[NSOpenGLView alloc] initWithFrame:[self bounds] pixelFormat:tFormat];
	
	if (_openGLView!=nil)
	{
		[_openGLView setWantsBestResolutionOpenGLSurface:YES];
		
		[self addSubview:_openGLView];
	}
	else
	{
		_OpenGLIncompatibilityDetected=YES;
		return;
	}
	
	[[_openGLView openGLContext] makeCurrentContext];
	
	NSRect tPixelBounds=[_openGLView convertRectToBacking:[_openGLView bounds]];
	NSSize tSize=tPixelBounds.size;
	
	_swirl=new swirl();
	
	if (_swirl!=NULL)
	{
		[SwirlSaverView _initializeSwirl:_swirl
						 withSettings:[[SWIRLSettings alloc] initWithDictionaryRepresentation:[tDefaults dictionaryRepresentation]]];
		
		_swirl->resize((int)tSize.width, (int)tSize.height);
		_swirl->create();
	}
	
	const GLint tSwapInterval=1;
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval,&tSwapInterval);
}

- (void)stopAnimation
{
	[super stopAnimation];
	
	if (_swirl!=NULL)
	{
		delete _swirl;
		_swirl=NULL;
	}
}

- (void)animateOneFrame
{
	if (_openGLView!=nil)
	{
		[[_openGLView openGLContext] makeCurrentContext];
		
		if (_swirl!=NULL)
		{
			_swirl->draw();
			
			[[_openGLView openGLContext] flushBuffer];
			
			if (_swirl->needsRefresh==true)
			{
				NSRect tPixelBounds=[_openGLView convertRectToBacking:[_openGLView bounds]];
				NSSize tSize=tPixelBounds.size;
				
				delete _swirl;
				_swirl=NULL;
				
				_swirl=new swirl();
				
				if (_swirl!=NULL)
				{
					NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
					ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
					
					[SwirlSaverView _initializeSwirl:_swirl
										withSettings:[[SWIRLSettings alloc] initWithDictionaryRepresentation:[tDefaults dictionaryRepresentation]]];
					
					_swirl->resize((int)tSize.width, (int)tSize.height);
					_swirl->create();
				}
			}
		}
	}
}

#pragma mark - Configuration

- (BOOL)hasConfigureSheet
{
	return YES;
}

- (NSWindow*)configureSheet
{
	if (_configurationWindowController==nil)
		_configurationWindowController=[[SWIRLConfigurationWindowController alloc] init];
	
	NSWindow * tWindow=_configurationWindowController.window;
	
	[_configurationWindowController restoreUI];
	
	return tWindow;
}

@end
