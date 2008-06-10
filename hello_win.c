#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIN_TITLE "Test-Programm"
#define WIN_NAME "TEST"
#define WIN_CLASS "test"

int main(int argc, char *argv[])
{
    Display *dpy;
    Window win;
    XClassHint class_hint;
    if ((dpy = XOpenDisplay(NULL)) == NULL)
    {
        printf("Unable to open display\n");
        return 1;
    }
    win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, \
    400, 300, BlackPixel(dpy, DefaultScreen(dpy)), 35, \
    WhitePixel(dpy, DefaultScreen(dpy)));
    XStoreName(dpy, win, WIN_TITLE);
    class_hint.res_name = WIN_NAME;
    class_hint.res_class = WIN_CLASS;
    XSetClassHint(dpy, win, &class_hint);
    XMapWindow(dpy, win);
    XFlush(dpy);
    sleep(10);
    return 0;
}
