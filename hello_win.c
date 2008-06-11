#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIN_TITLE "Test-Programm"
#define WIN_NAME "TEST"
#define WIN_CLASS "test"

int main(int argc, char *argv[])
{

    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        printf("Unable to open display\n");
        return 1;
    }

    XClassHint class_hint;

    int black_color = BlackPixel(display, DefaultScreen(display));
    int white_color = WhitePixel(display, DefaultScreen(display));

    Window window = XCreateSimpleWindow( display
                                       , DefaultRootWindow(display)
                                       , 0
                                       , 0
                                       , 400
                                       , 300
                                       , black_color 
                                       , 35
                                       , white_color
                                       );
    XStoreName(display, window, WIN_TITLE);
    class_hint.res_name = WIN_NAME;
    class_hint.res_class = WIN_CLASS;
    XSetClassHint(display, window, &class_hint);
    XMapWindow(display, window);
    XFlush(display);
    sleep(10);
    return 0;
}
