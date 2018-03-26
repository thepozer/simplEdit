
#include "simplEdit_app.h"
#include "simplEdit_window.h"

struct _SimpleditApp {
	GtkApplication parent;
  
	GSettings * pSettings;
};

G_DEFINE_TYPE(SimpleditApp, simpledit_app, GTK_TYPE_APPLICATION);

static void simpledit_app_init (SimpleditApp * pApp) {}

static void simpledit_app_activate (GApplication * pApp) {
	SimpleditAppWindow * pWindow;

	pWindow = simpledit_app_window_new(SIMPLEDIT_APP(pApp));
	gtk_window_present (GTK_WINDOW(pWindow));
}

static void simpledit_app_open (GApplication * pApp, GFile **files, gint n_files, const gchar * hint) {
	GList * pLstWindows;
	SimpleditAppWindow *pWindow;
	int i;

	pLstWindows = gtk_application_get_windows(GTK_APPLICATION(pApp));
	if (pLstWindows) {
		pWindow = SIMPLEDIT_APP_WINDOW(pLstWindows->data);
	} else {
		pWindow = simpledit_app_window_new(SIMPLEDIT_APP(pApp));
	}
	
	if (n_files > 0) {
		for (i = 0; i < n_files; i++) {
			simpledit_app_window_open(pWindow, files[i]);
		}
	}

	gtk_window_present(GTK_WINDOW(pWindow));
}

static void simpledit_app_class_init(SimpleditAppClass *class)
{
	G_APPLICATION_CLASS (class)->activate = simpledit_app_activate;
	G_APPLICATION_CLASS (class)->open = simpledit_app_open;
}

SimpleditApp * simpledit_app_new (void) {
	SimpleditApp * pApp = NULL;
	
	pApp = g_object_new (SIMPLEDIT_APP_TYPE, "application-id", "net.thepozer.simpledit", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
			
	pApp->pSettings = g_settings_new ("net.thepozer.simpledit");
	
	return pApp;
}

GSettings * simpledit_app_get_settings (SimpleditApp * pApp) {
	return pApp->pSettings;
}

void simpledit_app_quit (SimpleditApp * pApp) {
	GList * pLstWindows;
	SimpleditAppWindow * pWindow;
	int i;
	
	pLstWindows = gtk_application_get_windows(GTK_APPLICATION(pApp));
	while(pLstWindows != NULL) {
		pWindow = SIMPLEDIT_APP_WINDOW(pLstWindows->data);
		
		if (!simpledit_app_window_close_all(pWindow)) {
			return ;
		}
		
		pLstWindows = pLstWindows->next;
	}
	
	g_application_quit(G_APPLICATION(pApp));
}
