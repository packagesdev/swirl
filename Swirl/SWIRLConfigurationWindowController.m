/*
 Copyright (c) 2015, Stephane Sudre
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 - Neither the name of the WhiteBox nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "SWIRLConfigurationWindowController.h"

#import <ScreenSaver/ScreenSaver.h>

#import "SWIRLAboutBoxWindowController.h"

#import "SWIRLUserDefaults+Constants.h"

#import "SWIRLSettings.h"

#import "SWIRLWindow.h"

@interface SWIRLConfigurationWindowController () <SWIRLWindowDelegate>
{
	IBOutlet NSSlider *_numberOfKnotsSlider;
	
	IBOutlet NSSlider *_colorCyclingSpeedSlider;
	
	IBOutlet NSButton * _twoPlaneSupportButton;
	
	
	IBOutlet NSButton * _mainScreenCheckBox;
	
	IBOutlet NSButton * _cancelButton;
	
	NSRect _savedCancelButtonFrame;
	
	SWIRLSettings * _swirlSettings;
	
	BOOL _mainScreenSetting;
}

- (IBAction)setNumberOfKnots:(id)sender;

- (IBAction)setColorCyclingSpeed:(id)sender;

- (IBAction)setTwoPlaneSupport:(id)sender;


- (IBAction)showAboutBox:(id)sender;

- (IBAction)resetDialogSettings:(id)sender;

- (IBAction)closeDialog:(id)sender;

@end

@implementation SWIRLConfigurationWindowController

- (id)init
{
	self=[super init];
	
	if (self!=nil)
	{
		NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
		ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
		
		_swirlSettings=[[SWIRLSettings alloc] initWithDictionaryRepresentation:[tDefaults dictionaryRepresentation]];
		
		_mainScreenSetting=[tDefaults boolForKey:SWIRLUserDefaultsMainDisplayOnly];
	}
	
	return self;
}

- (NSString *)windowNibName
{
	return NSStringFromClass([self class]);
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	_savedCancelButtonFrame=[_cancelButton frame];
}

#pragma mark -

- (void)restoreUI
{
	[_numberOfKnotsSlider setIntegerValue:_swirlSettings.numberOKnots];
	
	[_colorCyclingSpeedSlider setIntegerValue:_swirlSettings.colorCyclingSpeed];
	
	[_twoPlaneSupportButton setState:(_swirlSettings.twoPlaneSupport==YES) ? NSOnState : NSOffState];
	
	[_mainScreenCheckBox setState:(_mainScreenSetting==YES) ? NSOnState : NSOffState];
}

- (IBAction)setNumberOfKnots:(id)sender
{
	_swirlSettings.numberOKnots=[sender integerValue];
}

- (IBAction)setColorCyclingSpeed:(id)sender
{
	_swirlSettings.colorCyclingSpeed=[sender integerValue];
}

- (IBAction)setTwoPlaneSupport:(id)sender
{
	_swirlSettings.twoPlaneSupport=([sender state]==NSOnState);
}

#pragma mark -

- (IBAction)showAboutBox:(id)sender
{
	static SWIRLAboutBoxWindowController * sAboutBoxWindowController=nil;
	
	if (sAboutBoxWindowController==nil)
		sAboutBoxWindowController=[SWIRLAboutBoxWindowController new];
	
	if ([sAboutBoxWindowController.window isVisible]==NO)
		[sAboutBoxWindowController.window center];
	
	[sAboutBoxWindowController.window makeKeyAndOrderFront:nil];
}

- (IBAction)resetDialogSettings:(id)sender
{
	[_swirlSettings resetSettings];
	
	[self restoreUI];
}

- (IBAction)closeDialog:(id)sender
{
	NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
	ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
	
	if ([sender tag]==NSModalResponseOK)
	{
		// Scene Settings
		
		NSDictionary * tDictionary=[_swirlSettings dictionaryRepresentation];
		
		[tDictionary enumerateKeysAndObjectsUsingBlock:^(NSString *bKey,id bObject, BOOL * bOutStop){
			[tDefaults setObject:bObject forKey:bKey];
		}];
		
		// Main Screen Only
		
		_mainScreenSetting=([_mainScreenCheckBox state]==NSOnState);
		
		[tDefaults setBool:_mainScreenSetting forKey:SWIRLUserDefaultsMainDisplayOnly];
		
		[tDefaults synchronize];
	}
	else
	{
		_swirlSettings=[[SWIRLSettings alloc] initWithDictionaryRepresentation:[tDefaults dictionaryRepresentation]];
		
		_mainScreenSetting=[tDefaults boolForKey:SWIRLUserDefaultsMainDisplayOnly];
	}
	
	[NSApp endSheet:self.window];
}

#pragma mark -

- (void)window:(NSWindow *)inWindow modifierFlagsDidChange:(NSEventModifierFlags) inModifierFlags
{
	if ((inModifierFlags & NSAlternateKeyMask) == NSAlternateKeyMask)
	{
		NSRect tOriginalFrame=[_cancelButton frame];
		
		[_cancelButton setTitle:NSLocalizedStringFromTableInBundle(@"Reset",@"Localizable",[NSBundle bundleForClass:[self class]],@"")];
		[_cancelButton setAction:@selector(resetDialogSettings:)];
		
		[_cancelButton sizeToFit];
		
		NSRect tFrame=[_cancelButton frame];
		
		tFrame.size.width+=10.0;	// To compensate for sizeToFit stupidity
		
		if (NSWidth(tFrame)<84.0)
			tFrame.size.width=84.0;
		
		tFrame.origin.x=NSMaxX(tOriginalFrame)-NSWidth(tFrame);
		
		[_cancelButton setFrame:tFrame];
	}
	else
	{
		[_cancelButton setTitle:NSLocalizedStringFromTableInBundle(@"Cancel",@"Localizable",[NSBundle bundleForClass:[self class]],@"")];
		[_cancelButton setAction:@selector(closeDialog:)];
		
		[_cancelButton setFrame:_savedCancelButtonFrame];
	}
}

@end
