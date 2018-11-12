#pragma once

#include "stdafx.h"

#include <functional>
#include <vector>

// utility methods
int WaitAndExit(int exitcode);
void WaitAndExecute(std::function<void()> callback, bool clear = false);
const char * ErrorToString(CorsairError error);

// SDK methods
const CorsairError Handshake(CorsairProtocolDetails &details);
const CorsairError RequestControl();
const CorsairError ReleaseControl();
const CorsairError GetLeds(std::vector<CorsairLedColor> &leds);
const CorsairError SetLedsColor(std::vector<CorsairLedColor> leds, CueTestColor color);
