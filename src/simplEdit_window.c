#include "simplEdit_window.h"

GtkWidget * pWndEdit = NULL;
SGlobalData gblData = {NULL, NULL} ;

void simplEdit_window_init(GtkBuilder * pBuilder) {
	gblData.builder = pBuilder;
	
	pWndEdit = GTK_WIDGET(gtk_builder_get_object(pBuilder, "wndSimplEdit"));
	
	gtk_builder_connect_signals (pBuilder, &gblData);

	gtk_widget_show_all(pWndEdit);

}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_main_quit();
}

void smpldt_clbk_menu_about (GtkMenuItem *menuitem, gpointer user_data) {
	const gchar * pcAuthors[] = {
		"Didier prolhac <dev@thepozer.net>",
		NULL
	};

	
	gtk_show_about_dialog (GTK_WINDOW(pWndEdit),
		"authors", pcAuthors,
		"comments", "Another small/simple editor based on GtkSourceView.\nIt try to make accessible every features of GtkSourceView.",
		"copyright", "(c) 2018 Didier Prolhac",
		"license", "MIT",
		"license-type", GTK_LICENSE_MIT_X11,
		"program-name", "simplEdit",
		"version", "0.0.1",
		"website", "https://github.com/thepozer/simplEdit",
		"website-label", "github simplEdit",
		NULL);
}
