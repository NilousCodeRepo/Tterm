//TODO: why FG black and not WHITE
#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <pty.h>
#include <X11/Xlib.h>

int term_window();

int main(void)
{
	term_window();

//open slave device
	int master_fd = posix_openpt(O_RDWR);//open pseudoterminal master device		
	int key = unlockpt(master_fd);//needed before creating a slave
	int slave_fd = grantpt(master_fd);//the real pseudoterm
	
	char buf = *(char*)malloc(sizeof(char)*64);
	int slave_name = ptsname_r(slave_fd, &buf,sizeof(buf));

	int open_fd = forkpty(&master_fd, NULL, NULL, NULL);
	
	return 0;
}

int term_window()
{
//create window in which to visualize the term using the bash shell

	void* default_value = NULL;
	Display* display = XOpenDisplay(default_value);
 	
	if (!display)
  {
    perror("ERROR: Can't open connection to display server.");
    return 1;
  }


	int screen = DefaultScreen(display);

  XSetWindowAttributes attributes = {
    .background_pixel = BlackPixel(display, screen),
    .event_mask = ExposureMask | KeyPressMask
  };

	int x = 100;
	int y = 100;
	int w = 100;
	int h = 100;
	int border_w = 1;

	Window window = XCreateWindow(display,
    														XRootWindow(display, screen),//parent
    														x, y, w, h, border_w,
    														DefaultDepth(display, screen),//number of planes
    														InputOutput,//type of window
    														DefaultVisual(display, screen),
    														CWBackPixel | CWEventMask, // same as attributes
    														&attributes);
	int win_name = XStoreName(display, window, "Tterm");// set window title

	int value_mask = 0;
	void* values_struct = NULL;
	GC graphic_context = XCreateGC(display, window, value_mask, values_struct);
	
	unsigned long WHITE = WhitePixel(display, screen);
  int FG = XSetForeground(display, graphic_context, WHITE);

	// create event handler for closing window in window manager
	//https://tronche.com/gui/x/xlib/window-information/properties-and-atoms.html
	bool only_if_exist = false;
  Atom close_window_ev = XInternAtom(display, 
																		 "WM_DELETE_WINDOW", 
																			 only_if_exist);
	//set property 
	XSetWMProtocols(display, window, &close_window_ev, 1);
	/*
	A property is a collection of named, typed data. 
	The window system has a set of predefined properties
	(for example, the name of a window, size hints, and so on).
	Users can define any other arbitrary information and associate it with windows.
	Each property has a name, which is an ISO Latin-1 string. 
	For each named property, a unique identifier(atom) is associated with it. 
	A property also has a type, for example, string or integer. 
	These types are also indicated using atoms, so arbitrary new types can be defined.
	Data of only one type may be associated with a single property name. 
	Clients can store and retrieve properties associated with windows. 
	For efficiency reasons, an atom is used rather than a character string. 
	XInternAtom can be used to obtain the atom for property names. 
	*/

	//make window elagable for displaying. Window becomes viewable
  XMapWindow(display, window);

	XEvent event = { 0 };
  
	for (int loop = 1; loop; )
  {
    XNextEvent(display, &event);//TODO:understand this

    switch (event.type)
    {
      case Expose:
      {
        XDrawRectangle(display, window, graphic_context, // draw rect
          210, 140, 300, 200);              // x, y, width, height//TODO: not
																																	//as intended
        break;
      }

      case KeyPress:
      {
				char KEY_Q = 'q';
        if (event.xkey.keycode == KEY_Q)
          loop = 0; 
        break;
      }

      case ClientMessage:                                       // handle event
      {                                                         // of closing window
        if ((Atom) event.xclient.data.l[0] == close_window_ev) // in window manager
          loop = 0;
        break;
      }
    }

  }

  XFreeGC(display, graphic_context);
  XUnmapWindow(display, window);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

	return 0;
}
