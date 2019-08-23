#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "font.h"

extern SDL_Surface *screen;
extern SDL_Surface *display;
SDL_Surface *background_tmp, *backbuffer;

extern char gamepak_filename[512];
static int32_t choice_menu = 0;

#define COLOR_BG           	SDL_MapRGB(screen->format,5,3,2)
#define COLOR_OK			SDL_MapRGB(screen->format,0,0,255)
#define COLOR_KO			SDL_MapRGB(screen->format,255,0,0)
#define COLOR_INFO			SDL_MapRGB(screen->format,0,255,0)
#define COLOR_LIGHT			SDL_MapRGB(screen->format,255,255,0)
#define COLOR_ACTIVE_ITEM   SDL_MapRGB(screen->format,255, 255, 255)
#define COLOR_INACTIVE_ITEM SDL_MapRGB(screen->format,255,255,255)

/* Shows menu items and pointing arrow	*/
#define SPRX (16)
#define OFF_X 0

/* Re-adujusting Menu Y position */
#define OFF_Y (-6)

uint16_t GBA_screen[(240*160)*2];

static int32_t savestate_slot = 0;
extern SDL_Joystick *joy;
extern int load_state(const char *savestate_filename);
extern int save_state(const char *savestate_filename, uint16_t *screen_capture);

static void screen_showchar(SDL_Surface *s, int32_t x, int32_t y, uint8_t a, const int32_t fg_color, const int32_t bg_color) 
{
	uint16_t *dst;
	uint16_t w, h;
	SDL_LockSurface(s);
	for(h = 8; h; h--) 
	{
		dst = (uint16_t *)s->pixels + ((y+8-h)*s->w + x);
		for(w = 8; w; w--) 
		{
			uint16_t color = *dst; /* background */
			if((fontdata8x8[a*8 + (8-h)] >> w) & 1) color = fg_color;
			*dst++ = color;
		}
	}
	SDL_UnlockSurface(s);
}

static void print_string_menu(const int8_t *s, const  uint16_t fg_color, const uint16_t bg_color, int32_t x, int32_t y) 
{
	int32_t i, j = strlen(s);
	for(i = 0; i < j; i++, x += 6)
	{
		screen_showchar(backbuffer, x, y, s[i], fg_color, bg_color);
	}
}

uint32_t MyGUI()
{
	char current_savestate_filename[256];
	SDL_Event event;
	uint8_t done = 0;
	
	savestate_slot = 0;
	choice_menu = 0;
	
	if (background_tmp == NULL)
	{
		background_tmp = SDL_LoadBMP("background.bmp");
	}
	
	if (backbuffer == NULL)
	{
		backbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,400,240,16, display->format->Rmask, display->format->Gmask, display->format->Bmask, display->format->Amask);
	}
	
	while(!done)
	{
		SDL_BlitSurface(background_tmp, NULL, backbuffer, NULL);
		print_string_menu("GPSP for RS-97, Arcade Mini", COLOR_ACTIVE_ITEM, 0, 16, 16); 
		
		print_string_menu("Go back to Game", COLOR_ACTIVE_ITEM, 0, 32, 48); 
		
		print_string_menu("Load State", COLOR_ACTIVE_ITEM, 0, 32, 64); 
		
		print_string_menu("Save State", COLOR_ACTIVE_ITEM, 0, 32, 80); 
		
		print_string_menu("Exit GPSP", COLOR_ACTIVE_ITEM, 0, 32, 96); 
		
		print_string_menu("=>", COLOR_LIGHT, 0, 16, 48 + (choice_menu * 16)); 
		SDL_SoftStretch(backbuffer, NULL, display, NULL);
		SDL_Flip(display);
		
		SDL_JoystickUpdate();
		
		if (SDL_JoystickGetAxis(joy, 1) < -500) 
		{
			choice_menu -= 1;
			if (choice_menu < 0) choice_menu = 0;
			SDL_Delay(80);
		}
		else if (SDL_JoystickGetAxis(joy, 1) > 500) 
		{
			choice_menu += 1;
			if (choice_menu > 3) choice_menu = 3;
			SDL_Delay(80);
		}
		

		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
				switch( event.key.keysym.sym )
                {
                    case SDLK_LEFT:
					break;
                    case SDLK_RIGHT:
					break;
                    case SDLK_UP:
                        choice_menu -= 1;
                        if (choice_menu < 0) choice_menu = 0;
					break;
                    case SDLK_DOWN:
                        choice_menu += 1;
                        if (choice_menu > 3) choice_menu = 3;
					break;
                    case SDLK_LCTRL:
                        switch(choice_menu)
                        {
							case 0:
								done = 1;
							break;
							/* Load state */
							case 1:
								strcpy(current_savestate_filename, gamepak_filename);
								strcat(current_savestate_filename, ".svs");
								load_state((const char*)current_savestate_filename);
								done = 1;
							break;
							/* Save State */
							case 2:
								memset(GBA_screen, 0, sizeof(GBA_screen));
								strcpy(current_savestate_filename, gamepak_filename);
								strcat(current_savestate_filename, ".svs");
								save_state((const char*)current_savestate_filename, GBA_screen);
							break;
							case 3:
								done = 2;
							break;
						}
					break;
					default:
					break;
                }
				break;
				case SDL_JOYBUTTONDOWN:
				{
					switch(event.jbutton.button)
					{
						case 2:
                        switch(choice_menu)
                        {
							case 0:
								done = 1;
							break;
							/* Load state */
							case 1:
								strcpy(current_savestate_filename, gamepak_filename);
								strcat(current_savestate_filename, ".svs");
								load_state((const char*)current_savestate_filename);
								done = 1;
							break;
							/* Save State */
							case 2:
								memset(GBA_screen, 0, sizeof(GBA_screen));
								strcpy(current_savestate_filename, gamepak_filename);
								strcat(current_savestate_filename, ".svs");
								save_state((const char*)current_savestate_filename, GBA_screen);
							break;
							case 3:
								done = 2;
							break;
						}
						break;
					}
					break;
				}
				default:
				break;
			}
		}
	}
	SDL_FillRect(backbuffer, NULL, 0);
	SDL_FillRect(screen, NULL, 0);
	SDL_FillRect(display, NULL, 0);
	SDL_Flip(display);
	
	if (done == 2) return 1;
	else return 0;
}

