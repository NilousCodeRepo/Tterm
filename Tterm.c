//TODO:try GLFW with #define GLFW_NO_API

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <pty.h>
#include <X11/Xlib.h>
#include <assert.h>

//i only check the env var to not cause confusion when actually opening a connection to the server, i hope it is enough, if not, i will resolve.
bool xorg_exists()
{
	char* disp_env_var = getenv("DISPLAY");
	if(disp_env_var == NULL)
		return false;

	return true;
}

//use: XSelectInput() if using XCreateWindow(), with the simple version is automatically set to InputOutput
void term_window()
{
	if(!xorg_exists())//TODO: check to see if it works
	{
		assert("FATAL ERROR: X server is NOT running\nor the 'DISPLAY' enviromental variable is not set properly");
	}

	void* DEFAULT = NULL;
	//for more info check: tronche.com/gui/x/xlib/display/opening.html
	Display* display = XOpenDisplay(DEFAULT);//returns opaque struct
	int screen = XDefaultScreen(display);
	
	int ROOT_DEFAULT = XRootWindow(display, screen);
	
	int x = 0;
	int y = 0;
	
	unsigned int width = 100;
	unsigned int height = 100;
	
	unsigned long border_width = 1;
	unsigned long border = 0;
	unsigned long BG = 0;

	//allocate and initialize memory for window(?) without drawing it
	Window simple_window = XCreateSimpleWindow(display,ROOT_DEFAULT, x, y, width, height,
																						 border_width, border, BG);

	int window_name = XStoreName(display, simple_window, "Tterm");

	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	
	Status set_atom_protocols = XSetWMProtocols(display, simple_window, &wm_delete_window, 1);

	XMapWindow(display, simple_window);//map window to be drawn

	XEvent event;

	int loop = 1;
	while(loop != 0)
	{

		XNextEvent(display, &event);
		switch(event.type)
		{
			case KeyPress:
				if(event.xkey.keycode)
				{
					loop = 0;
				}
			break;

			case ClientMessage:
			{

				if((Atom)event.xclient.data.l[0] == wm_delete_window)
					{
						loop = 0;
					}
			break;
			}
		}
	}
	
	XCloseDisplay(display);	
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


