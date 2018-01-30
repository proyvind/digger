#ifndef __SDL_JS_H
#define __SDL_JS_H

#include <SDL.h>

#include "def.h"

bool init_joystick(void);

int16_t GetJSAxis(int joy_idx, int axis);

uint8_t GetJSHat(int joy_idx, int hat);

uint8_t GetJSButton(int joy_idx, int button);

void HapticRumble(int joy_idx, float strength, uint32_t length);

#endif
