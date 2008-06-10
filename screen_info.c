#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

Display *display;
int screen_num;
unsigned int display_width;
unsigned int display_height;

int main(int argc, char** argv) {

/* connect to the XServer */
if ((display = XOpenDisplay(NULL)) == NULL) {
    printf("ERROR - Failed to call XOpenDisplay()\n");
    exit(-1);
}

/* Query the display structure for information */
screen_num = DefaultScreen(display);
display_width = DisplayWidth(display, screen_num);
display_height = DisplayHeight(display, screen_num);

/* Print the resolution into the shell */
printf("resolution width: %d pixels\n", display_width);
printf("resolution height: %d pixels\n", display_height);

/* Close the Window and exit application */
XCloseDisplay(display);

return 0;
}

