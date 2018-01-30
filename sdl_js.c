#include "sdl_js.h"

static void print_joystick_info(int joy_idx, SDL_Joystick* joy, SDL_GameController* gamepad)
{
  SDL_JoystickGUID guid = SDL_JoystickGetGUID(joy);
  char guid_str[1024];
  SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));

  printf("Joystick Name:     '%s'\n", SDL_JoystickName(joy));
  printf("Joystick GUID:     %s\n", guid_str);
  printf("Joystick Number:   %2d\n", joy_idx);
  printf("Number of Axes:    %2d\n", SDL_JoystickNumAxes(joy));
  printf("Number of Buttons: %2d\n", SDL_JoystickNumButtons(joy));
  printf("Number of Hats:    %2d\n", SDL_JoystickNumHats(joy));
  printf("Number of Balls:   %2d\n", SDL_JoystickNumBalls(joy));
  printf("GameController:\n");
  if (!gamepad)
  {
    printf("  not a gamepad\n");
  }
  else
  {
    printf("  Name:    '%s'\n", SDL_GameControllerName(gamepad));
    printf("  Mapping: '%s'\n", SDL_GameControllerMappingForGUID(guid));
  }
  printf("\n");
}


static SDL_Joystick *joy[2] = {NULL,NULL};
static SDL_Haptic *haptic[2] = {NULL,NULL};

bool init_joystick(void) {
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
    fprintf(stderr, "Unable to init joystick: %s\n", SDL_GetError());
  else {
    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks == 0)
      printf("No joysticks were found\n");
    else
    {
      printf("Found %d joystick(s)\n\n", num_joysticks);
      if (num_joysticks > sizeof(joy)/sizeof(*joy))
	num_joysticks = sizeof(joy)/sizeof(*joy);
      for(int joy_idx = 0; joy_idx < num_joysticks; ++joy_idx) {
	if (joy[joy_idx])
	  SDL_JoystickClose(joy[joy_idx]);
	joy[joy_idx] = SDL_JoystickOpen(joy_idx);

	if (!joy[joy_idx])
	  fprintf(stderr, "Unable to open joystick %d\n", 0);
	else {
	  if (haptic[joy_idx])
	    SDL_HapticClose(haptic[joy_idx]);
	  haptic[joy_idx] = SDL_HapticOpenFromJoystick(joy[joy_idx]);
	  if (!SDL_HapticRumbleSupported(haptic[joy_idx]) &&
		SDL_HapticRumbleInit(haptic[joy_idx]) !=0) {
	    SDL_HapticClose(haptic[joy_idx]);
	    haptic[joy_idx] = NULL;
	  }
	}
      }
    }
  }
  return (joy[0] != NULL);
}

int16_t GetJSAxis(int joy_idx, int axis) {
  return SDL_JoystickGetAxis(joy[joy_idx], axis);
}

uint8_t GetJSHat(int joy_idx, int hat) {
  return SDL_JoystickGetHat(joy[joy_idx], hat);
}

uint8_t GetJSButton(int joy_idx, int button) {
  return SDL_JoystickGetButton(joy[joy_idx], button);
}

void HapticRumble(int joy_idx, float strength, uint32_t length) {
  if (haptic[joy_idx])
    SDL_HapticRumblePlay(haptic[joy_idx], strength, length);
}
