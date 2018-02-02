#include "simplEdit_window.h"

GtkWidget * pWndEdit = NULL;
typedef struct {
	GtkBuilder    * pBuilder;
	GtkWidget     * pWndEdit;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;
} SEditorData;

SEditorData gblData = {NULL, NULL, NULL, NULL, TRUE} ;

void simplEdit_window_init(GtkBuilder * pBuilder) {
	gblData.pBuilder = pBuilder;
	
	gblData.pWndEdit = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "wndSimplEdit"));
	gblData.pSrcView = GTK_SOURCE_VIEW(gtk_builder_get_object(gblData.pBuilder, "srcView"));
	gblData.pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gblData.pSrcView));
	
	gtk_builder_connect_signals(gblData.pBuilder, &gblData);

	gtk_widget_show_all(gblData.pWndEdit);

}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_text_buffer_set_text(gblData.pTxtBuff, "", 0);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_main_quit();
}


void smpldt_clbk_menu_edit (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pMenuItem = NULL;

	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuEditUndo"));
	gtk_widget_set_sensitive(pMenuItem, gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(gblData.pTxtBuff)));

	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuEditRedo"));
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(gblData.pTxtBuff)));
	
	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuEditCut"));
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_text_buffer_get_has_selection(gblData.pTxtBuff));
	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuEditCopy"));
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_text_buffer_get_has_selection(gblData.pTxtBuff));
}

void smpldt_clbk_menu_edit_undo (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_source_buffer_undo(GTK_SOURCE_BUFFER(gblData.pTxtBuff));
}

void smpldt_clbk_menu_edit_redo (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_source_buffer_redo(GTK_SOURCE_BUFFER(gblData.pTxtBuff));
}

void smpldt_clbk_menu_edit_cut (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_cut_clipboard(gblData.pTxtBuff, pClipboard, gblData.bWritable);
}

void smpldt_clbk_menu_edit_copy (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_copy_clipboard(gblData.pTxtBuff, pClipboard);
}

void smpldt_clbk_menu_edit_paste (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_paste_clipboard(gblData.pTxtBuff, pClipboard, NULL, gblData.bWritable);
}


void smpldt_clbk_menu_search_find (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_search_replace (GtkMenuItem *menuitem, gpointer user_data) {
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
