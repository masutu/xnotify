#include <stdio.h>
/*#include <string.h>
#include <stdlib.h>*/
#include <unistd.h> /* for sleep() */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/*#include <X11/Xos.h>
#include <X11/Xatom.h>*/

#define WIN_TITLE "test_programme"
#define WIN_NAME "TEST"
#define WIN_CLASS "test"


int main(int argc, char *argv[])
{

    Display *display = XOpenDisplay(NULL);
    printf("dddd\n");
    if (display == NULL) {
        printf("Unable to open display\n");
        return 1;
    }

    XClassHint class_hint;
    XSizeHints *size_hints;
    size_hints = XAllocSizeHints();
    XWMHints *wm_hints;
    wm_hints = XAllocWMHints();




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
    
        
    size_hints->flags = PPosition | PSize | PMinSize; 
    size_hints->min_width = 400;
    size_hints->min_height = 300;
    
    XSetWMNormalHints(display, window, size_hints);
    
    XMapWindow(display, window);
    XFlush(display);

    while(1) {
        sleep(1);
	printf("nextstepi\n");
    }
    return 0;
}
