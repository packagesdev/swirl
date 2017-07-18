/*
 Copyright (c) 2015-2017, Stephane Sudre
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 - Neither the name of the WhiteBox nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
	
	inSwirl->colorRotationSpeed=(int)inSettings.colorCyclingSpeed;
	
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
	
	if (_OpenGLIncompatibilityDetected==YES)
	{
		BOOL tShowErrorMessage=_mainScreen;
		
		if (tShowErrorMessage==NO)
		{
			NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
			ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
			
			tShowErrorMessage=![tDefaults boolForKey:SWIRLUserDefaultsMainDisplayOnly];
		}
		
		if (tShowErrorMessage==YES)
		{
			NSRect tFrame=[self frame];
			
			NSMutableParagraphStyle * tMutableParagraphStyle=[[NSParagraphStyle defaultParagraphStyle] mutableCopy];
			[tMutableParagraphStyle setAlignment:NSCenterTextAlignment];
			
			NSDictionary * tAttributes = @{NSFontAttributeName:[NSFont systemFontOfSize:[NSFont systemFontSize]],
										   NSForegroundColorAttributeName:[NSColor whiteColor],
										   NSParagraphStyleAttributeName:tMutableParagraphStyle};
			
			
			NSString * tString=NSLocalizedStringFromTableInBundle(@"Minimum OpenGL requirements\rfor this Screen Effect\rnot available\ron your graphic card.",@"Localizable",[NSBundle bundleForClass:[self class]],@"No comment");
			
			NSRect tStringFrame;
			
			tStringFrame.origin=NSZeroPoint;
			tStringFrame.size=[tString sizeWithAttributes:tAttributes];
			
			tStringFrame=SSCenteredRectInRect(tStringFrame,tFrame);
			
			[tString drawInRect:tStringFrame withAttributes:tAttributes];
		}
	}
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
	
	if (_openGLView!=nil)
	{
		[_openGLView removeFromSuperview];
		_openGLView=nil;
	}
	
	_openGLView = [[NSOpenGLView alloc] initWithFrame:self.bounds pixelFormat:tFormat];
	
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
	
	NSRect tPixelBounds=[_openGLView convertRectToBacking:_openGLView.bounds];
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
				NSRect tPixelBounds=[_openGLView convertRectToBacking:_openGLView.bounds];
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
