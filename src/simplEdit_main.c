#define __SIMPLEDIT_MAIN_PART__
#include "simplEdit_main.h"

#include "simplEdit_window.h"

GtkApplication * GpApp = NULL;

int main (int argc, char *argv []) {
	GtkBuilder * pBuilder = NULL;
	GError * pErr = NULL;
	gchar * pcFilename = NULL;
	guint iRet = 0;

	gtk_init(&argc, &argv);
	GpApp = gtk_application_new("net.thepozer.simpledit", 0);
	
	pBuilder = gtk_builder_new();
	
	g_resources_register(simplEdit_get_resource());
	
	iRet = gtk_builder_add_from_resource(pBuilder, "/net/thepozer/simpledit/simplEdit.glade", &pErr);

	if (pErr) {
		gint iCode = pErr->code;
		g_printerr("%s\n", pErr->message);
		g_error_free (pErr);
		return iCode;
	}

	simplEdit_window_init(pBuilder);
	
	gtk_main();

	return 0;
}
