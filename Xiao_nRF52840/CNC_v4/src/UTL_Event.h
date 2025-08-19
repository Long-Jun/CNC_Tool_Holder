/*
  2025 Copyright (c) National Taipei University of Technology. All right reserved.
  2025 Copyright (c) Cory Long Jun, Chiou.  All Rights Reserved.

  Author: Long Jun, Chiou
  Contributor: Cory Long Jun, Chiou
  Website: https://github.com/Long-Jun
  2025-08-19  
  This Library contains Arduino libraries commonly used in NTUT UTL & SPIE Lab.
*/

#ifndef UTL_Event_h
#define UTL_Event_h

#include <inttypes.h>

#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2

class UTL_Event
{

public:
  UTL_Event(void);
  void update(void);
  void update(unsigned long now);
  int8_t eventType;
  unsigned long period;
  int repeatCount;
  uint8_t pin;
  uint8_t pinState;
  void (*callback)(void);
  unsigned long lastEventTime;
  int count;
};

#endif
