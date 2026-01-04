//TODO:try GLFW with #define GLFW_NO_API

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <pty.h>
#include <X11/Xlib.h>
#include <assert.h>

#define KEY_ESCAPE 9

bool xorg_exists()
{
	char* disp_env_var = getenv("DISPLAY");
	Display* display = XOpenDisplay(display);

	if(disp_env_var == NULL || display == NULL)
		return false;

	return true;
}

int term_window()
{
	if(!xorg_exists())
	{
		assert("FATAL ERROR: X server is NOT running\nor the 'DISPLAY' enviromental variable is not set properly");
	}


#if 0
	int screen = DefaultScreen(display);
	
	XSetWindowAttributes attributes = {
		.background_pixel = BlackPixel(display, screen),
		.event_mask = KeyPressMask //DO NOT SET ExposureMask, it spawns TWO windows
	};
	//TODO: use XSelectInput()

	int x = 100;
	int y = 100;
	int w = 800;
	int h = 600;
	int border_w = 0;

	Window window = XCreateWindow(display,
																RootWindow(display, screen),
																x, y, w, h, border_w,
																DefaultDepth(display, screen),
																InputOutput,
																DefaultVisual(display, screen),
																CWBackPixel | CWEventMask, // same as attributes
																&attributes);

	XStoreName(display, window, "Tterm");

	GC gc = XCreateGC(display, window, 0, NULL);              // create graphic context
	XSetForeground(display, gc, WhitePixel(display, screen));

	//closing window event handler, theese are the events that i want to receive
	//from the window manager(which is higher level than X so it has to handle
	//the closing events)
	Atom wm_delete_window = XInternAtom(display,"WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	XMapWindow(display, window);

	XEvent event;

	for (int loop = 1; loop; )
	{
		XNextEvent(display, &event);

		switch (event.type)
		{	
			case KeyPress:
				{
					if (event.xkey.keycode == KEY_ESCAPE)
						loop = 0;
					break;
				}

			case ClientMessage:                                       
				{ //event for closing window 
					if ((Atom) event.xclient.data.l[0] == wm_delete_window) 
						loop = 0;
					break;
				}
			}
		}

	XCloseDisplay(display);//this is enougth, it handles everything
#endif
}


int main(void)
{
	term_window();

//open slave device
	int master_fd = posix_openpt(O_RDWR);//open pseudoterminal master device		
	int key = unlockpt(master_fd);//unlocks master/slave pair
	int slave_fd = grantpt(master_fd);//the real pseudoterm
	
	char buf = *(char*)malloc(sizeof(char)*64);
	buf = 0;
	int slave_name = ptsname_r(slave_fd, &buf,sizeof(buf));

	int open_fd = forkpty(&master_fd, NULL, NULL, NULL);
	
	return 0;
}


