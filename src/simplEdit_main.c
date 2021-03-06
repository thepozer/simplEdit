#define __SIMPLEDIT_MAIN_PART__
#include "simplEdit_main.h"

#include "simplEdit_app.h"

int main (int argc, char *argv []) {
	/* Set up internationalization */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);

	g_resources_register(simplEdit_get_resource());
	
	return g_application_run(G_APPLICATION(simpledit_app_new()), argc, argv);
}
