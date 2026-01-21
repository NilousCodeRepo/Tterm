//TODO:try GLFW with #define GLFW_NO_API
//TODO: check for GPU and if there is use glx to render
#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <pty.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>


#define UNUSED __attribute__((unused))

//i only check the env var to not cause confusion when actually opening a connection to the server, i hope it is enough, if not, i will resolve.
bool xorg_exists()
{
	char* disp_env_var = getenv("DISPLAY");
	if(disp_env_var == NULL)
		return false;

	return true;
}

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
	
	unsigned int border_width = 1;
	
	unsigned long border_color = 0;
	unsigned long BG = 0;

	//allocate and initialize memory for window(?) without drawing it
	Window simple_window = XCreateSimpleWindow(display,ROOT_DEFAULT, x, y, width, height,
                                               border_width, border_color, BG);

	XEvent event = {0};
	
	XSelectInput(display,simple_window, KeyPressMask);//specifies the input that i want the window to receive

	UNUSED int window_name = XStoreName(display, simple_window, "Tterm");

	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	
	UNUSED Status set_atom_protocols = XSetWMProtocols(display, simple_window, &wm_delete_window, 1);

	XMapWindow(display, simple_window);//map window to be drawn

	XSync(display, false);

	int loop = 1;
	while(loop != 0)
	{
		XNextEvent(display, &event);
		switch(event.type)
		{
			case KeyPress:
			{
				printf("Key Pressed!\n");
			}
			break;

			case ClientMessage:
			{
				if((Atom)event.xclient.data.l[0] == wm_delete_window)
				{
					loop = 0;
				}
			}
			break;
			}
		}

	XCloseDisplay(display);	
}


/*
 * A me serve creare un nuovo processo(?) con execv o qualcosa di simile
 * quindi mi serve il PID dello slave(?)
 * poi devo fare in modo renderizzare i caratteri su term_window(), usando gli eventi
 * successivamente devo mandare quei comandi allo shell(bash) e stampare l'output
 * quindi devi connettermi a stdin e stdout e stderr per eventuali cazzi
*/
int main(void)
{
    //term_window();
    
    //open an unused master file descriptor for master pseduto-terminal 
    int master_fd = posix_openpt(O_RDWR);//from /dev/pts/ptmx pseudo-term multiplexter 
    
    if(master_fd == -1)
    {
        perror("ERROR: Could not open master device\n");
        return 1; 
    }

    //it changes the ID of the slave to the real UID of the calling process, so Tterm in this case 
    grantpt(master_fd);
    unlockpt(master_fd); 

    // Once both the pseudoterminal master and slave are open
    // the slave provides processes  with  an  interface that is identical to that of a real terminal.
   
    char* slave_name = ptsname(master_fd); 
    int slave_fd = open(slave_name, O_RDWR|O_NOCTTY);//offline man is wrong, return value > 0 is good
    
    if(slave_fd < 0)
    {
        perror("ERROR: Could not open slave device\n");
        return -1;
    }

    char* slave_to_shell = getenv("SHELL");
   
   //TODO: with fork() spawn new process, so if i call it from a term, it opens a new window
    while(true)
        execv(slave_to_shell,NULL);


    return 0;
}


