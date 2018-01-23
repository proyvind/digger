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

bool init_joystick(void) {
  if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
    fprintf(stderr, "Unable to init joystick: %s\n", SDL_GetError());
  } else {
    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks == 0)
    {
      printf("No joysticks were found\n");
    }
    else
    {
      printf("Found %d joystick(s)\n\n", num_joysticks);
      for(int joy_idx = 0; joy_idx < num_joysticks; ++joy_idx)
      {
	SDL_Joystick* joy = SDL_JoystickOpen(joy_idx);
	if (!joy)
	{
	  fprintf(stderr, "Unable to open joystick %d\n", joy_idx);
	}
	else
	{
	  SDL_GameController* gamepad = SDL_GameControllerOpen(joy_idx);
	  print_joystick_info(joy_idx, joy, gamepad);
	  if (gamepad)
	  {
	    SDL_GameControllerClose(gamepad);
	  }
	  SDL_JoystickClose(joy);
	}
      }
      return true;
    }
  }
  return false;
}


