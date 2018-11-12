#include "CueTestFunctions.h"

#include "stdafx.h"

#include <functional>
#include <iostream>
#include <vector>


/***** Utility methods *****/

void WaitForInput(bool clear = true, const char *message = "Press Enter to continue. ")
{
	std::cout << message << std::flush;
	getchar();

	if (clear) system("cls");
}

// should never clear
int WaitAndExit(int code)
{
	WaitForInput(false, "Press Enter to exit. ");
	exit(code);
	return code;
}

// may want to clear
void WaitAndExecute(std::function<void()> cb, bool clear = false)
{
	WaitForInput(clear);
	cb();
}

const char * ErrorToString(CorsairError e)
{
	switch (e)
	{
	case CE_Success:
		return "success";
	case CE_ServerNotFound:
		return "server not found";
	case CE_NoControl:
		return "lost exclusive control";
	case CE_ProtocolHandshakeMissing:
		return "protocol handshake missing";
	case CE_IncompatibleProtocol:
		return "incompatible protocol";
	case CE_InvalidArguments:
		return "invalid arguments";
	default:
		return "unknown error";
	}
}


/***** SDK methods *****/


const CorsairError Handshake(CorsairProtocolDetails &result)
{
	result = CorsairPerformProtocolHandshake();
	return CorsairGetLastError();
}

const CorsairError RequestControl()
{
	return CorsairRequestControl(CAM_ExclusiveLightingControl) ? CE_Success : CorsairGetLastError();
}

const CorsairError ReleaseControl()
{
	return CorsairReleaseControl(CAM_ExclusiveLightingControl) ? CE_Success : CorsairGetLastError();
}

const CorsairError GetLeds(std::vector<CorsairLedColor> &leds)
{
	int num_devices = CorsairGetDeviceCount();
	if (const auto err = CorsairGetLastError()) return err;

	for (auto i_dev = 0; i_dev < num_devices; i_dev++)
	{
		if (CorsairDeviceInfo *deviceInfo = CorsairGetDeviceInfo(i_dev))
		{
			switch (deviceInfo->type)
			{
			case CDT_Keyboard:
			{
				CorsairLedPositions *positions = CorsairGetLedPositions();
				if (positions)
				{
					for (int i_pos = 0; i_pos < positions->numberOfLed; i_pos++)
					{
						CorsairLedId ledId = positions->pLedPosition[i_pos].ledId;
						leds.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
					}
				}

				if (const auto err = CorsairGetLastError()) return err;
				break;
			}

			default: break;
			}
		}
	}

	return CorsairGetLastError();
}

const CorsairError SetLedsColor(std::vector<CorsairLedColor> leds, CueTestColor color)
{
	for (auto p = leds.begin(); p != leds.end(); p++)
	{
		p->r = color.r;
		p->g = color.g;
		p->b = color.b;
	}

	CorsairSetLedsColors((int)leds.size(), leds.data());
	return CorsairGetLastError();
}
