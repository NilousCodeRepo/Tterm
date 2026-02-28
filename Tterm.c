//TODO: move all the clutter in a init.h file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define UNUSED __attribute__((unused))
#define ENV_DISPLAY getenv("DISPLAY")

bool xorg_exists();

//Specifies the connection to the DISPLAY that is the one in the user's DISPLAY enviromental variable
Display* T_default_display();

//check number of screens and return default root window
int root_screen();

//Window initialitation. TODO: make it more elastic, check screen size and so on
Window init_simple_window(Display* display);

typedef struct grid
{
    int x;
    int y;
}grid;


int main(UNUSED int ac, UNUSED char* av[])
{
    printf("========= QUESTO E' TTERM V2 ===========\n");
    printf("XORG ACTIVE: %d\n", xorg_exists());

    Display* display = T_default_display();
    if(display == NULL)
        printf("[FATAL ERROR]: Could not initialize display\n");
    
    Window window = init_simple_window(display); 
    if(!window)
        printf("[FATAL ERROR]: Could not initialize window\n");
    
    printf("AVAILABLE MONITORS COUNT: %d\n", XScreenCount(display));

	XSelectInput(display, window, KeyPressMask | ExposureMask);//specifies the input type that X should report, if no make -> no report on input
    
    UNUSED int window_name = XStoreName(display, window, "Tterm");
	
    XMapWindow(display, window);//map window to be drawn
    
    GC graphical_ctx = XCreateGC(display, window, 0, NULL);
    
	XSync(display, false);

	int screen = XDefaultScreen(display);
    unsigned long white = WhitePixel(display, screen);
    unsigned long black = BlackPixel(display, screen);
    
    XSetBackground(display, graphical_ctx, black);
    XSetForeground(display, graphical_ctx, white);

    XEvent event = {0};
    
    bool running = true;
    
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    UNUSED Status set_atom_protocols = XSetWMProtocols(display, window, &wm_delete_window, 1);
    
    while(running)//idk why event handler does not work
    {
        XNextEvent(display, &event);
        switch(event.type)
        {
            case KeyPress:
            {
                KeyCode key = event.xkey.keycode;
                char* character = XKeysymToString( XKeycodeToKeysym(display, key, 0) );
            
                case Expose:
                {
                    XDrawString(display, window, graphical_ctx, 50, 50, character, strlen(character));
                }
            }
            break;

            case ClientMessage:
            {
                if( (Atom)event.xclient.data.l[0] == wm_delete_window )
                {
                    running = false;
                }
            }
            break;
        }
    }

    return EXIT_SUCCESS;
}

bool xorg_exists()
{
	char* disp_env_var = ENV_DISPLAY;
	if(disp_env_var == NULL)
    {
        printf("[FATAL ERROR]: No Xserver active at the moment\n");
		return false;
    }
    
    return true;
}

Display* T_default_display()
{
    Display* pd = XOpenDisplay(ENV_DISPLAY);
    return pd;
}

int root_screen()
{
    Display* d = T_default_display();
    int monitor_count = XScreenCount(d);
   
	int root = XDefaultScreen(d);

    if(monitor_count < 0)
        printf("[FATAL ERROR]: No available screens(physical monitors) found\n"); 

    Window s = XRootWindow(d, root);
    return s;
}

Window init_simple_window(Display* display)
{
    int screen = root_screen();
    
    int x = 100;
    int y = 100;
    int height = 600; 
    int width = 800;

    int border_color = 0;
    int border_width = 1;
	
    unsigned long BG = 0;
    
    Window w = XCreateSimpleWindow(display,
                                   screen,
                                   x,y,
                                   width,height,
                                   border_color,
                                   border_width,
                                   BG
                                  );
    return w;
}
