/*
 Copyright (c) 2015, Stephane Sudre
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 - Neither the name of the WhiteBox nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "SWIRLSettings.h"

NSString * const SWIRLUserDefaultsNumberOfKnotsKey=@"Node";

NSString * const SWIRLUserDefaultsColorCyclingSpeedKey=@"ColorCyclingSpeed";

NSString * const SWIRLUserDefaultsTwoPlaneSupportKey=@"Plane";

@implementation SWIRLSettings

- (id)initWithDictionaryRepresentation:(NSDictionary *)inDictionary
{
	self=[super init];
	
	if (self!=nil)
	{
		[self resetSettings];
	
		NSNumber * tNumber=inDictionary[SWIRLUserDefaultsNumberOfKnotsKey];
		if (tNumber!=nil)
			_numberOKnots=[tNumber unsignedIntegerValue];
		
		tNumber=inDictionary[SWIRLUserDefaultsColorCyclingSpeedKey];
		if (tNumber!=nil)
			_colorCyclingSpeed=[tNumber unsignedIntegerValue];
		
		tNumber=inDictionary[SWIRLUserDefaultsTwoPlaneSupportKey];
		if (tNumber!=nil)
			_twoPlaneSupport=[tNumber boolValue];
	}
	
	return self;
}

- (NSDictionary *)dictionaryRepresentation
{
	NSMutableDictionary * tMutableDictionary=[NSMutableDictionary dictionary];
	
	if (tMutableDictionary!=nil)
	{
		tMutableDictionary[SWIRLUserDefaultsNumberOfKnotsKey]=@(_numberOKnots);
		tMutableDictionary[SWIRLUserDefaultsColorCyclingSpeedKey]=@(_colorCyclingSpeed);
		tMutableDictionary[SWIRLUserDefaultsTwoPlaneSupportKey]=@(_twoPlaneSupport);
	}
	
	return [tMutableDictionary copy];
}

- (void)resetSettings
{
	_numberOKnots=5;
	
	_colorCyclingSpeed=2;
	
	_twoPlaneSupport=NO;
}

@end
