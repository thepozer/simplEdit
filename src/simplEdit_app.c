
#include "simplEdit_app.h"
#include "simplEdit_window.h"

struct _SimpleditApp
{
  GtkApplication parent;
};

G_DEFINE_TYPE(SimpleditApp, simpledit_app, GTK_TYPE_APPLICATION);

static void simpledit_app_init (SimpleditApp *app) {}

static void simpledit_app_activate (GApplication *app) {
	SimpleditAppWindow *win;

	win = simpledit_app_window_new (SIMPLEDIT_APP (app));
	gtk_window_present (GTK_WINDOW (win));
}

static void simpledit_app_open (GApplication *app, GFile **files, gint n_files, const gchar * hint) {
	GList *windows;
	SimpleditAppWindow *win;
	int i;

	windows = gtk_application_get_windows(GTK_APPLICATION(app));
	if (windows) {
		win = SIMPLEDIT_APP_WINDOW(windows->data);
	} else {
		win = simpledit_app_window_new(SIMPLEDIT_APP(app));
	}
	
	if (n_files > 0) {
		simpledit_app_window_open(win, files[0]);
	}

	gtk_window_present(GTK_WINDOW(win));
}

static void simpledit_app_class_init(SimpleditAppClass *class)
{
	G_APPLICATION_CLASS (class)->activate = simpledit_app_activate;
	G_APPLICATION_CLASS (class)->open = simpledit_app_open;
}

SimpleditApp * simpledit_app_new (void) {
	return g_object_new (SIMPLEDIT_APP_TYPE, 
			"application-id", "net.thepozer.simpledit", 
			"flags", G_APPLICATION_HANDLES_OPEN, 
			NULL);
}
