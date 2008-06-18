#include <math.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define EV_STANDALONE 1
#include <ev.c>

#define LOG_MESSAGE(...)  printf(__VA_ARGS__)
#define LOG_ERROR(...)  printf(__VA_ARGS__)
#define LOG_WARNING(...)  printf(__VA_ARGS__)

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 200

static char *text;

enum {
  XATOM_WM_STATE,
  XATOM_NET_DESKTOP_NAMES,
  XATOM_NET_WM_STATE,
  XATOM_NET_ACTIVE_WINDOW,
  XATOM_NET_WM_NAME,
  XATOM_NET_WORKAREA,
  XATOM_NET_WM_ICON,
  XATOM_NET_WM_VISIBLE_NAME,
  XATOM_NET_WM_STATE_SKIP_TASKBAR,
  XATOM_NET_WM_STATE_SHADED,
  XATOM_NET_WM_STATE_HIDDEN,
  XATOM_NET_WM_DESKTOP,
  XATOM_NET_MOVERESIZE_WINDOW,
  XATOM_NET_WM_WINDOW_TYPE,
  XATOM_NET_WM_WINDOW_TYPE_DOCK,
  XATOM_NET_WM_WINDOW_TYPE_DESKTOP,
  XATOM_NET_WM_STRUT,
  XATOM_NET_CLIENT_LIST,
  XATOM_NET_NUMBER_OF_DESKTOPS,
  XATOM_NET_CURRENT_DESKTOP,
  XATOM_NET_SYSTEM_TRAY_OPCODE,
  XATOM_UTF8_STRING,
  XATOM_MOTIF_WM_HINTS,
  XATOM_COUNT
};

static char *atom_names[] = {
	"WM_STATE",
	"_NET_DESKTOP_NAMES",
	"_NET_WM_STATE",
	"_NET_ACTIVE_WINDOW",
	"_NET_WM_NAME",
	"_NET_WORKAREA",
	"_NET_WM_ICON",
	"_NET_WM_VISIBLE_NAME",
	"_NET_WM_STATE_SKIP_TASKBAR",
	"_NET_WM_STATE_SHADED",
	"_NET_WM_STATE_HIDDEN",
	"_NET_WM_DESKTOP",
	"_NET_MOVERESIZE_WINDOW",
	"_NET_WM_WINDOW_TYPE",
	"_NET_WM_WINDOW_TYPE_DOCK",
	"_NET_WM_WINDOW_TYPE_DESKTOP",
	"_NET_WM_STRUT",
	"_NET_CLIENT_LIST",
	"_NET_NUMBER_OF_DESKTOPS",
	"_NET_CURRENT_DESKTOP",
	"_NET_SYSTEM_TRAY_OPCODE",
	"UTF8_STRING",
	"_MOTIF_WM_HINTS"
};

static struct {
  Display *display;
  int screen;
  int screen_width;
  int screen_height;
  
  int wa_x;
  int wa_y;
  int wa_w;
  int wa_h;

  Visual *visual;
  Colormap colmap;
  XSetWindowAttributes attrs;
  uint32_t amask;
  int depth;

  Window root;
  Atom atoms[XATOM_COUNT];
  Atom trayselatom;
} X;

static void cleanup()
{
  LOG_MESSAGE("cleanup\n");
  exit(1);
}

/**************************************************************************
  X error handlers
**************************************************************************/

static int X_error_handler(Display *dpy, XErrorEvent *error)
{
  char buf[1024];
  if (error->error_code == BadWindow)
    return 0;
  XGetErrorText(dpy, error->error_code, buf, sizeof(buf));
  LOG_WARNING("X error: %s (resource id: %d)\n", buf, error->resourceid);
  return 0;
}

static int X_io_error_handler(Display *dpy)
{
  LOG_WARNING("fatal error, connection to X server lost? cleaning up\n");
  cleanup();
  return 0;
}

/**************************************************************************
  window properties
**************************************************************************/

static void *get_prop_data(Window win, Atom prop, Atom type, int *items)
{
	Atom type_ret;
	int format_ret;
	unsigned long items_ret;
	unsigned long after_ret;
	unsigned char *prop_data;

	prop_data = 0;

	XGetWindowProperty(X.display, win, prop, 0, 0x7fffffff, False,
			type, &type_ret, &format_ret, &items_ret,
			&after_ret, &prop_data);
	if (items)
		*items = items_ret;

	return prop_data;
}

/**************************************************************************
  initialization
**************************************************************************/

static void initX()
{
	/* open connection to X server */
	X.display = XOpenDisplay(0);
	if (!X.display)
		LOG_ERROR("failed connect to X server\n");
	XSetErrorHandler(X_error_handler);
	XSetIOErrorHandler(X_io_error_handler);
	
	memset(&X.attrs, 0, sizeof(X.attrs));

	/* useful variables */
	X.screen 	= DefaultScreen(X.display);
	X.screen_width 	= DisplayWidth(X.display, X.screen);
	X.screen_height	= DisplayHeight(X.display, X.screen);
	X.visual 	= DefaultVisual(X.display, X.screen);
	X.colmap	= CopyFromParent;
	X.root 		= RootWindow(X.display, X.screen);
	X.amask		= 0;
	X.depth 	= DefaultDepth(X.display, X.screen);
	X.wa_x 		= 0;
	X.wa_y 		= 0;
	X.wa_w 		= X.screen_width;
	X.wa_h 		= X.screen_height;
	
	/* get internal atoms */
	XInternAtoms(X.display, atom_names, XATOM_COUNT, False, X.atoms);
	XSelectInput( X.display
              , X.root
              , PropertyChangeMask 
              | ButtonPressMask 
              | KeyPressMask 
              | ExposureMask
              | FocusChangeMask 
              | StructureNotifyMask
              );

	/* append_font_path_to_imlib(); */

	/* get workarea */
	int32_t *workarea = get_prop_data(X.root, X.atoms[XATOM_NET_WORKAREA], XA_CARDINAL, 0);
	if (workarea) {
		X.wa_x = workarea[0];
		X.wa_y = workarea[1];
		X.wa_w = workarea[2];
		X.wa_h = workarea[3];
		XFree(workarea);	
	}
}

/**************************************************************************
  libev callbacks
**************************************************************************/

static int counter = 0;

static void xconnection_cb(EV_P_ struct ev_io *w, int revents)
{
  XEvent e;
  while (XPending(X.display)) {

    printf("hello world %d\n", counter++);

    XNextEvent(X.display, &e);
    switch (e.type) {
    case SelectionClear:
      /*
      handle_selection_clear(&e.xselectionclear);
      */
      break;
    case Expose:
      /* commence_panel_redraw = 1; */
      break;
    case ButtonPress:
    case KeyPress:
      printf("button press\n");
      /*
      handle_button(e.xbutton.x, e.xbutton.y, e.xbutton.button);
      */
      break;
    case ConfigureNotify:
      /*
      handle_configure_notify(e.xconfigure.window);
      */
      break;
    case PropertyNotify:
      /*
      handle_property_notify(e.xproperty.window, e.xproperty.atom);  
      */
      break;
    case FocusIn:
      /*
      handle_focusin(e.xfocus.window);
      render_update_panel_positions(&P);
      commence_taskbar_redraw = 1;
      */
      break;
    case ClientMessage:
      /*
      handle_client_message(&e.xclient);
      */
      break;
    case ReparentNotify:
      /*
      handle_reparent_notify(e.xreparent.window, e.xreparent.parent);
      */
      break;
    case DestroyNotify:
      /*
      del_tray_icon(e.xdestroywindow.window);
      render_update_panel_positions(&P);
      commence_panel_redraw = 1; 
      */
      break;
    default:
      break;
    }
    XSync(X.display, 0);
    /*
    if (commence_panel_redraw) {
      render_panel(&P);
    } else if (commence_switcher_redraw || commence_taskbar_redraw) {
      if (commence_switcher_redraw) {
        render_switcher(P.desktops);
      }
      if (commence_taskbar_redraw) {
        render_taskbar(P.tasks, P.desktops);
      }
      render_present();
    }
    commence_panel_redraw = 0;
    commence_switcher_redraw = 0;
    commence_taskbar_redraw = 0;
    */
  }
}

/**************************************************************************
  signal handlers
**************************************************************************/

static void sighup_handler(int xxx)
{
  LOG_MESSAGE("sighup signal received\n");
  cleanup();
  /* xmemleaks(); */
  exit(0);
}

static void sigint_handler(int xxx)
{
  LOG_MESSAGE("sigint signal received\n");
  cleanup();
  /* xmemleaks(); */
  exit(0);
}

/**************************************************************************
  libev loop and main
**************************************************************************/

static void init_and_start_ev_loop(int xfd)
{
  struct ev_loop *el = ev_default_loop(0);
  ev_io xconnection;

  /* macros?! whuut?! */
  xconnection.active = xconnection.pending = xconnection.priority = 0;
  xconnection.cb = xconnection_cb;
  xconnection.fd = xfd; 
  xconnection.events = EV_READ | EV_IOFDSET;

  ev_io_start(el, &xconnection);
  ev_loop(el, 0);
}

void display_window() 
{
  XClassHint class_hint;

  int black_color = BlackPixel(X.display, X.screen);
  int white_color = WhitePixel(X.display, X.screen);

  Window window = XCreateSimpleWindow( X.display
                                     , X.root
                                     , 0
                                     , 0
                                     , WINDOW_WIDTH
                                     , WINDOW_HEIGHT
                                     , black_color 
                                     , 35
                                     , white_color
                                     );
  XStoreName(X.display, window, "title");
  class_hint.res_name = "test";
  class_hint.res_class = "test";
  XSetClassHint(X.display, window, &class_hint);
  XMapWindow(X.display, window);
  XFlush(X.display);
}

int main(int argc, char **argv)
{

  if(argc == 2)
    text = argv[1];
  else {
    printf("give an argument.\n");
    exit(1);
  }

  LOG_MESSAGE("starting xnotify\n");

  initX();

  signal(SIGHUP, sighup_handler);
  signal(SIGINT, sigint_handler);
  
  display_window();

  init_and_start_ev_loop(ConnectionNumber(X.display));

  cleanup();

  return 0;
}
