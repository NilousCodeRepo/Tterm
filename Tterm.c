#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <pty.h>
#include <X11/Xlib.h>

#define KEY_ESCAPE 9

int term_window()
{
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
}


int main(void)
{
	term_window();

//open slave device
	int master_fd = posix_openpt(O_RDWR);//open pseudoterminal master device		
	int key = unlockpt(master_fd);//needed before creating a slave
	int slave_fd = grantpt(master_fd);//the real pseudoterm
	
	char buf = *(char*)malloc(sizeof(char)*64);
	buf = 0;
	int slave_name = ptsname_r(slave_fd, &buf,sizeof(buf));

	int open_fd = forkpty(&master_fd, NULL, NULL, NULL);
	
	return 0;
}


