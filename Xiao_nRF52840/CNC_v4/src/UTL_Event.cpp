/*
  2025 Copyright (c) National Taipei University of Technology. All right reserved.
  2025 Copyright (c) Cory Long Jun, Chiou.  All Rights Reserved.

  Author: Long Jun, Chiou
  Contributor: Cory Long Jun, Chiou
  Website: https://github.com/Long-Jun
  2025-08-19  
  This Library contains Arduino libraries commonly used in NTUT UTL & SPIE Lab.
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "UTL_Event.h"

UTL_Event::UTL_Event(void)
{
	eventType = EVENT_NONE;
}

void UTL_Event::update(void)
{
    unsigned long now = millis();
    update(now);
}

void UTL_Event::update(unsigned long now)
{
	if (now - lastEventTime >= period)
	{
		switch (eventType)
		{
			case EVENT_EVERY:
				(*callback)();
				break;

			case EVENT_OSCILLATE:
				pinState = ! pinState;
				digitalWrite(pin, pinState);
				break;
		}
		lastEventTime = now;
		count++;
	}
	if (repeatCount > -1 && count >= repeatCount)
	{
		eventType = EVENT_NONE;
	}
}
