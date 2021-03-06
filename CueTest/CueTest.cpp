// CueTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CueTestFunctions.h"

#include <iostream>
#include <vector>

void EndTest();

void TestRed();
void TestGreen();
void TestBlue();

CueTestColor CTC_Red{ "red", 255, 0, 0 };
CueTestColor CTC_Green{ "green", 0, 255, 0 };
CueTestColor CTC_Blue{ "blue", 0, 0, 255 };

std::vector<CorsairLedColor> leds;

int main()
{
	CorsairProtocolDetails details;

	if (const auto err = Handshake(details))
	{
		std::cout << "Handshake error: " << ErrorToString(err) << std::endl;
		return WaitAndExit(err);
	}

	if (details.serverVersion == NULL)
	{
		std::cout << "CUE server not found.\nPlease make sure that iCUE is up and running before running this program!" << std::endl;
		return WaitAndExit(2);
	}

	if (const auto err = GetLeds(leds))
	{
		std::cout << "Error " << err << " retrieving LEDs: " << ErrorToString(err) << "\n";
		return WaitAndExit(err);
	}

	if (const auto err = RequestControl())
	{
		std::cout << "Unable to request exclusive control of LEDs: " << ErrorToString(err) << std::endl;
		return WaitAndExit(err);
	}

	std::cout
		<< "CUE server version " << details.serverVersion << ", protocol " << details.serverProtocolVersion
		<< "\nCUE SDK version " << details.sdkVersion << ", protocol " << details.sdkProtocolVersion
		<< "\n" << leds.size() << " LEDs on keyboard, all currently blanked"
		<< "\n\nReady to test.\n" << std::flush;

	WaitAndExecute(&TestRed);

	if (const auto err = ReleaseControl())
	{
		std::cout << "Unable to release exclusive control of LEDs: " << ErrorToString(err)
			<< "\n(Don't worry, control will be released implicitly when this program crashes, "
			<< "which will be right... about.... n" << std::flush;
		return WaitAndExit(err);
	}

	std::cout << "Test complete." << std::endl;
	return WaitAndExit(0);
}

void EndTest()
{
	std::cout << "\nTest complete." << std::endl;
	WaitAndExit(0);
}



void TestColor(CueTestColor color, std::function<void()> callback)
{
	if (const auto err = SetLedsColor(leds, color))
	{
		std::cout << "\nUnable to test " << color.name << " LEDs: " << ErrorToString(err) << std::endl;
		WaitAndExit(err);
		return;
	}

	std::cout << "\nYour keyboard should now be lit up " << color.name << ". Verify." << std::endl;
	WaitAndExecute(callback);
}

void TestRed() { TestColor(CTC_Red, &TestGreen); }
void TestGreen() { TestColor(CTC_Green, &TestBlue); }
void TestBlue() { TestColor(CTC_Blue, &EndTest); }
