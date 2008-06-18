#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>

#define WIN_TITLE "test_programme"
#define WIN_NAME "TEST"
#define WIN_CLASS "test"

//char* font_name = "*-helvetica-*-12-*";
char* font_name = "9x15";

XFontStruct* font_info;
XGCValues     values;
GC            gc;
XEvent        report;


int main(int argc, char *argv[])
{

    Display *display = XOpenDisplay(NULL);
    printf("dddd\n");
    if (display == NULL) {
        printf("Unable to open display\n");
        return 1;
    }
    
    font_info = XLoadQueryFont(display, font_name);
    if (!font_info) {
        fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", font_name);
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
    
    gc = XCreateGC(display, window, 0, &values);

    XSetFont(display, gc, font_info->fid);
    XSetForeground(display, gc, black_color);

    class_hint.res_name = WIN_NAME;
    class_hint.res_class = WIN_CLASS;
    
    XSetClassHint(display, window, &class_hint);
    
        
    size_hints->flags = PPosition | PSize | PMinSize; 
    size_hints->min_width = 400;
    size_hints->min_height = 300;
    
    XSetWMNormalHints(display, window, size_hints);
   
    XSelectInput(display, window, ExposureMask);

    XMapWindow(display, window);
    XFlush(display);

    while(1) {
         static char *text = "Hello, Ryan";
	 static int   txt_length;
	 static int   font_height;
	 static int   txt_x_pos,txt_y_pos;
	 
	 XNextEvent(display, &report);
	 
	 switch ( report.type ) {

	 case Expose:
	     if ( report.xexpose.count != 0 )
	     break;

	     txt_length = XTextWidth(font_info, text, strlen(text));

	     XDrawString(display, window, gc, 100, 100, "hello ryan",
	                 strlen("hello ryan"));
            break;
        }
    }
    return 0;
}
