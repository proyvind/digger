/* Digger Remastered
   Copyright (c) Andrew Jenner 1998-2004 */

#include "def.h"
#include "input.h"
#include "main.h"
#include "sound.h"
#include "hardware.h"
#include "record.h"
#include "digger.h"
#ifdef _SDL
#include "sdl_kbd.h" 
#include "sdl_js.h"
#elif defined(_VGL)
#include "fbsd_kbd.h"
#endif

/* global variables first */
bool escape=false,pausef=false,mode_change=false;
bool krdf[NKEYS]={false,false,false,false,false,false,false,false,false,false,
               false,false,false,false,false,false,false,false};

static bool aleftpressed[2]={false,false},arightpressed[2]={false,false},
       auppressed[2]={false,false},adownpressed[2]={false,false},
       af1pressed[2]={false,false},firepflag[2]={false,false};
static bool start=false;
int16_t akeypressed;

static int16_t dynamicdir[2]={-1,-1},staticdir[2]={-1,-1},
       keydir[2]={false,false};

static bool joyflag=false;

bool getfirepflag(int n)
{
  return firepflag[n];
}

/* The standard ASCII keyboard is also checked so that very short keypresses
   are not overlooked. The functions kbhit() (returns bool denoting whether or
   not there is a key in the buffer) and getkey() (wait until a key is in the
   buffer, then return it) are used. These functions are emulated on platforms
   which only provide an inkey() function (return the key in the buffer, unless
   there is none, in which case return -1. It is done this way around for
   historical reasons, there is no fundamental reason why it shouldn't be the
   other way around. */
void checkkeyb(void)
{
  int i,j,k=0;
  bool *aflagp[10]={&arightpressed[0],&auppressed[0],&aleftpressed[0],&adownpressed[0],
                    &af1pressed[0],&arightpressed[1],&auppressed[1],&aleftpressed[1],
                    &adownpressed[1],&af1pressed[1]};
  if (leftpressed(0))
    aleftpressed[0]=true;
  if (rightpressed(0))
    arightpressed[0]=true;
  if (uppressed(0))
    auppressed[0]=true;
  if (downpressed(0))
    adownpressed[0]=true;
  if (f1pressed(0))
    af1pressed[0]=true;
  if (leftpressed(1))
    aleftpressed[1]=true;
  if (rightpressed(1))
    arightpressed[1]=true;
  if (uppressed(1))
    auppressed[1]=true;
  if (downpressed(1))
    adownpressed[1]=true;
  if (f1pressed(1))
    af1pressed[1]=true;

  while (kbhit()) {
    akeypressed=getkey(true);
    for (i=0;i<10;i++)
      for (j=2;j<5;j++)
        if (akeypressed==keycodes[i][j])
          *aflagp[i]=true;
    for (i=10;i<NKEYS;i++)
      for (j=0;j<5;j++)
        if (akeypressed==keycodes[i][j])
          k=i;
    switch (k) {
      case DKEY_CHT: /* Cheat! */
        if (!gauntlet) {
          playing=false;
          drfvalid=false;
        }
        break;
      case DKEY_SUP: /* Increase speed */
        if (ftime>10000l)
          ftime-=10000l;
        break;
      case DKEY_SDN: /* Decrease speed */
        ftime+=10000l;
        break;
      case DKEY_MTG: /* Toggle music */
        musicflag=!musicflag;
        break;
      case DKEY_STG: /* Toggle sound */
        soundflag=!soundflag;
        break;
      case DKEY_EXT: /* Exit */
        escape=true;
        break;
      case DKEY_PUS: /* Pause */
        pausef=true;
        break;
      case DKEY_MCH: /* Mode change */
        mode_change=true;
        break;
      case DKEY_SDR: /* Save DRF */
        savedrf=true;
        break;
    }
    if (!mode_change)
      start=true;                                /* Change number of players */
  }
}

/* Joystick not yet implemented. It will be, though, using gethrt on platform
   DOSPC. */
static uint8_t readjoy(int n)
{
  uint8_t hat = SDL_HAT_CENTERED;

  if (joyflag) {
    if ((hat = GetJSHat(n,0)) == SDL_HAT_CENTERED) {
      int16_t axisx = GetJSAxis(n,0), axisy = GetJSAxis(n,1);
      if (axisx || axisy) {
	static const uint16_t threshold = 16384;

	if (axisx < 0 && abs(axisx) > threshold)
	  hat|=SDL_HAT_LEFT;
	else if (axisx > threshold)
	  hat|=SDL_HAT_RIGHT;
	if (axisy < 0 && abs(axisy) > threshold)
	  hat|=SDL_HAT_UP;
	else if (axisy > threshold)
	  hat|=SDL_HAT_DOWN;
      }
    }
  }

  return hat;
}

void detectjoy(void)
{
  joyflag = init_joystick();
}

/* Contrary to some beliefs, you don't need a separate OS call to flush the
   keyboard buffer. */
void flushkeybuf(void)
{
  while (kbhit())
    getkey(true);
  aleftpressed[0]=arightpressed[0]=auppressed[0]=adownpressed[0]=af1pressed[0]=false;
  aleftpressed[1]=arightpressed[1]=auppressed[1]=adownpressed[1]=af1pressed[1]=false;
}

void clearfire(int n)
{
  af1pressed[n]=false;
}

bool oupressed[2]={false,false},odpressed[2]={false,false},
     olpressed[2]={false,false},orpressed[2]={false,false};

void readdirect(int n)
{
  bool u=false,d=false,l=false,r=false;
  uint8_t hat = readjoy(n);

    if ((hat & SDL_HAT_UP) || auppressed[n] || uppressed(n)) { u=true; auppressed[n]=false; }
    if ((hat & SDL_HAT_DOWN) || adownpressed[n] || downpressed(n)) { d=true; adownpressed[n]=false; }
    if ((hat & SDL_HAT_LEFT) || aleftpressed[n] || leftpressed(n)) { l=true; aleftpressed[n]=false; }
    if ((hat & SDL_HAT_RIGHT) || arightpressed[n] || rightpressed(n)) { r=true; arightpressed[n]=false; }
    if (GetJSButton(n,0) || f1pressed(n) || af1pressed[n]) {
      firepflag[n]=true;
      af1pressed[n]=false;
    }
    else
      firepflag[n]=false;
    if (u && !oupressed[n])
      staticdir[n]=dynamicdir[n]=DIR_UP;
    if (d && !odpressed[n])
      staticdir[n]=dynamicdir[n]=DIR_DOWN;
    if (l && !olpressed[n])
      staticdir[n]=dynamicdir[n]=DIR_LEFT;
    if (r && !orpressed[n])
      staticdir[n]=dynamicdir[n]=DIR_RIGHT;
    if ((oupressed[n] && !u && dynamicdir[n]==DIR_UP) ||
        (odpressed[n] && !d && dynamicdir[n]==DIR_DOWN) ||
        (olpressed[n] && !l && dynamicdir[n]==DIR_LEFT) ||
        (orpressed[n] && !r && dynamicdir[n]==DIR_RIGHT)) {
      dynamicdir[n]=DIR_NONE;
      if (u) dynamicdir[n]=staticdir[n]=2;
      if (d) dynamicdir[n]=staticdir[n]=6;
      if (l) dynamicdir[n]=staticdir[n]=4;
      if (r) dynamicdir[n]=staticdir[n]=0;
    }
    oupressed[n]=u;
    odpressed[n]=d;
    olpressed[n]=l;
    orpressed[n]=r;
    keydir[n]=staticdir[n];
    if (dynamicdir[n]!=DIR_NONE)
      keydir[n]=dynamicdir[n];
    staticdir[n]=DIR_NONE;
}

bool teststart(void)
{
  bool startf=false;
  if (joyflag) {
    if (GetJSButton(0,0))
      startf=true;
  }
  if (start) {
    start=false;
    startf=true;
  }
  if (!startf)
    return false;

  return true;
}

int16_t getdirect(int n)
{
  int16_t dir=keydir[n];
    if (playing)
      playgetdir(&dir,&firepflag[n]);
    recputdir(dir,firepflag[n]);
  return dir;
}
