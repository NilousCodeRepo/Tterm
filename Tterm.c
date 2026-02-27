//TODO:try GLFW with #define GLFW_NO_API
//TODO: check for GPU and if there is use glx to render

//TODO: WRITE THINGS TO TERMINAL
#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>//manipulate fds
#include <stdlib.h>
#include <string.h>
#include <pty.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

#define UNUSED __attribute__((unused))

//i only check the env var to not cause confusion when actually opening a connection to the server, i hope it is enough, if not, i will resolve.
bool xorg_exists()
{
	char* disp_env_var = getenv("DISPLAY");
	if(disp_env_var == NULL)
		return false;

	return true;
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
	
	unsigned int width = 800;
	unsigned int height = 600;
	
	unsigned int border_width = 1;
	
	unsigned long border_color = 0;
	unsigned long BG = 0;

	//allocate and initialize memory for window without drawing it
	Window simple_window = XCreateSimpleWindow(display,
                                               ROOT_DEFAULT, 
                                               x, y, width, height,
                                               border_width, 
                                               border_color, 
                                               BG);

	
	XSelectInput(display, simple_window, KeyPressMask | ExposureMask);//specifies the input type that X should report, if no make -> no report on input

 	int window_name = XStoreName(display, simple_window, "Tterm");

	XMapWindow(display, simple_window);//map window to be drawn

    Drawable d = simple_window;
    GC graphical_ctx = XCreateGC(display, d, 0, NULL);//Segfaulted here, wtf is a Drawable? is a fucking window where you draw, what a shit lib
    
	XSync(display, false);

#if 0
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
    // the slave provides a processe with an interface that is identical to that of a real terminal.   
    char* slave_name = ptsname(master_fd); 
    int slave_fd = open(slave_name, O_RDWR|O_NOCTTY);//offline man is wrong, return value > 0 is good
    
    if(slave_fd < 0)
    {
        perror("ERROR: Could not open slave device\n");
        return -1;
    }
    else
    {
        char* slave_to_shell = getenv("SHELL");//execute shell in a separete process
        char* arg = "";
    
        execlp(slave_to_shell, arg, NULL);
    }
#endif

    int master_fd;  // primary file descriptor for the pseudo-terminal
    pid_t pid;      // Process ID of the child process

    // create a pseudo-terminal (pty)
    pid = forkpty(&master_fd, NULL, NULL, NULL);
    
    if (pid == -1) {
        perror("Error creating pseudo-terminal");
        return 1;
    }

    // Execute the shell in the child process
    if (pid == 0) {
        // at this point, the child process is bound to the secondary end of the pty
        // This means that stdin/stdout/stderr of the child process will be connected to the pty
        // and the following code writes to stdout of the pty.
        printf("Initializing the shell...\n");

        // this replaces the child process completely with the shell set in the environment
        execlp(getenv("SHELL"), getenv("SHELL"), NULL);

        // If execlp fails, handle the error. Notice that if execlp succeeds, then this line will not be executed.
        // because the current process image was replaced by the shell.
        perror("execlp");
        exit(1);
    }

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    Status set_atom_protocols = XSetWMProtocols(display, simple_window, &wm_delete_window, 1);

    XEvent event;
    int exit_flag = 1;

    unsigned long white = WhitePixel(display, screen);
    unsigned long black = BlackPixel(display, screen);
    
    XSetBackground(display, graphical_ctx, black);
    XSetForeground(display, graphical_ctx, white);
   
    while(exit_flag != 0)
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
                        exit_flag = 0;
                    }
                }
            break;
            
            case Expose:
                {
                    char* msg = "STOP THIS NIGHTMARE";
                    XDrawString(display, simple_window, graphical_ctx, 50, 50, msg, strlen(msg));
                }
            break;
        }
    }

    XCloseDisplay(display);
    return 0;

}

