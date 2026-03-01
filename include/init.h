#define HEIGHT 600
#define WIDTH  800

bool xorg_exists();

Display* default_display();

Window root_screen();

Window init_simple_window(Display* display);


