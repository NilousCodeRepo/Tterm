#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "init.h"

#define UNUSED __attribute__((unused))

void create_window(Display* display, int screen, Window Window, GC graphical_ctx);
bool event_handler(Display* display, Window window, GC graphical_ctx);

int main(UNUSED int ac, UNUSED char* av[])
{
    printf("========== THIS IS TTERM_V2 ===========\n");

    Display* display = default_display();
    Window window = init_simple_window(display);
    int screen = XDefaultScreen(display); 
    
    GC graphical_ctx = XCreateGC(display, window, 0, NULL);
    
    create_window(display, screen, window, graphical_ctx);
    
    while( event_handler(display, window, graphical_ctx) );
    
    return EXIT_SUCCESS;
}

//TODO(maybe): pass an even mask with the events you want, make struct and so on
void create_window(Display* display, int screen, Window window, GC graphical_ctx)
{
    XSelectInput(display, window, KeyPressMask );//specifies the input type that X should report, if no make -> no report on input
    
    UNUSED int window_name = XStoreName(display, window, "Tterm");
    
    XMapWindow(display, window);//map window to be drawn
    
    unsigned long white = WhitePixel(display, screen);
    unsigned long black = BlackPixel(display, screen);
    
    XSetBackground(display, graphical_ctx, black);
    XSetForeground(display, graphical_ctx, white);
	
    XSync(display, false);
    
}

bool event_handler(Display* display, Window window, GC graphical_ctx)
{
    int text_x = 20;
    int text_y = 20;

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    UNUSED Status set_atom_protocols = XSetWMProtocols(display, window, &wm_delete_window, 1);

    XEvent event = {0};
    bool running = true;
    
    while(running)//idk why event handler does not work
    {
        XNextEvent(display, &event);
        switch(event.type)
        {
            case KeyPress:
            {
                KeyCode key = event.xkey.keycode;
                char* character = XKeysymToString( XKeycodeToKeysym(display, key, 0) );
            
                    XDrawString(display, window, graphical_ctx, text_x, text_y, character, strlen(character));
                    text_x += 10;
                    if( text_y == WIDTH - 1)
                    {
                       text_y += 1;
                       text_x = 20;
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
    return running;
}
