/*  THIS WORKS AS AN INITIALIZATION FILE(obv)
 *  NO RAELLY IMPORTANT CODE IS HERE
*/
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#define ENV_DISPLAY getenv("DISPLAY")
#include "init.h"

//Specifies the connection to the DISPLAY that is the one in the user's DISPLAY enviromental variable
bool xorg_exists()
{
	char* disp_env_var = ENV_DISPLAY;
	
    if(disp_env_var == NULL)
    {
        printf("[FATAL ERROR]: No Xserver active at the moment\n");
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}

Display* default_display()
{
    Display* pd = XOpenDisplay(ENV_DISPLAY);

    if(pd == NULL)
    {
        printf("[FATAL ERROR]: Could not initialize display\n");
        exit(EXIT_FAILURE);
    }
    return pd;
}

//check number of screens and return default root window for setup in init_simple_window()
Window root_screen()
{
    Display* d = default_display();
	int root = XDefaultScreen(d);

    int monitor_count = XScreenCount(d);
    
    if(monitor_count < 0)
    {
        printf("[FATAL ERROR]: No available screens(physical monitors) found\n");
        exit(EXIT_FAILURE);
    }

    Window s = XRootWindow(d, root);
    
    if(!s)
    {
        printf("[FATAL ERROR]: Could not setup the window\n");
        exit(EXIT_FAILURE);
    }

    return s;
}

//Window initialitation. TODO: make it more elastic, check screen size and so on
Window init_simple_window(Display* display)
{
    int screen = root_screen();
    
    int x = 100;
    int y = 100;

    int border_color = 0;
    int border_width = 1;
	
    unsigned long BG = 0;
    
    Window w = XCreateSimpleWindow(display,
                                   screen,
                                   x,y,
                                   WIDTH,HEIGHT,
                                   border_color,
                                   border_width,
                                   BG
                                  );
    if(!w)
    {
        printf("[FATAL ERROR]: Could not initialize window\n");
        exit(EXIT_FAILURE);
    }

    return w;
}

