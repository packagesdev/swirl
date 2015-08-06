
#import "SWIRLConfigurationWindowController.h"

#import <ScreenSaver/ScreenSaver.h>

#import "SWIRLAboutBoxWindowController.h"

#import "SWIRLUserDefaults+Constants.h"

#import "SWIRLSettings.h"

#import "SWIRLWindow.h"

@interface SWIRLConfigurationWindowController ()
{
	IBOutlet NSSlider *_numberOfKnotsSlider;
	
	IBOutlet NSButton * _twoPlaneSupportButton;
	
	
	IBOutlet NSButton * _mainScreenCheckBox;
	
	IBOutlet NSButton * _cancelButton;
	
	NSRect _savedCancelButtonFrame;
	
	SWIRLSettings * _swirlSettings;
	
	BOOL _mainScreenSetting;
}

- (IBAction)setNumberOfKnots:(id)sender;

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
	
	[_twoPlaneSupportButton setState:(_swirlSettings.twoPlaneSupport==YES) ? NSOnState : NSOffState];
	
	[_mainScreenCheckBox setState:(_mainScreenSetting==YES) ? NSOnState : NSOffState];
}

- (IBAction)setNumberOfKnots:(id)sender
{
	_swirlSettings.numberOKnots=[sender integerValue];
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

- (void)window:(NSWindow *)inWindow modifierFlagsDidChange:(NSUInteger) inModifierFlags
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
