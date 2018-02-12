#include "simplEdit_window.h"
#include "simplEdit_content.h"

GtkWidget * pWndEdit = NULL;

SimpleditContent * pEditData = NULL;

GtkWidget * simplEdit_window_extraWidget(SimpleditContent * pEditData);

void simplEdit_window_init(GtkBuilder * pBuilder) {
	pEditData = simpledit_content_new(pBuilder);
	
	simpledit_content_update_title(pEditData);
}

void smpldt_clbk_menu_file (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pMenuItem = NULL;
	gboolean bModified, bHaveFilename;
	
	bModified = simpledit_content_is_modified(pEditData);
	bHaveFilename = simpledit_content_have_filename(pEditData);
	
	pMenuItem = simpledit_content_get_widget(pEditData, "menuFileSave");
	gtk_widget_set_sensitive(pMenuItem, bModified && bHaveFilename);

	pMenuItem = simpledit_content_get_widget(pEditData, "menuFileSaveAs");
	gtk_widget_set_sensitive(pMenuItem, bModified);
	
	pMenuItem = simpledit_content_get_widget(pEditData, "menuFileReturnToSaved");
	gtk_widget_set_sensitive(pMenuItem, bModified && bHaveFilename);
}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	simpledit_content_reset(pEditData);
	simpledit_content_update_title(pEditData);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) { 
	if (simpledit_content_select_name(pEditData, GTK_FILE_CHOOSER_ACTION_OPEN)) {
		if (simpledit_content_load(pEditData)) {
			simpledit_content_update_title(pEditData);
		}
	}
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	if (simpledit_content_have_filename(pEditData)) {
		simpledit_content_save(pEditData);
	}
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	if (simpledit_content_select_name(pEditData, GTK_FILE_CHOOSER_ACTION_SAVE)) {
		if (simpledit_content_save(pEditData)) {
			simpledit_content_update_title(pEditData);
		}
	}
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	if (simpledit_content_have_filename(pEditData)) {
		simpledit_content_load(pEditData);
	}
}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_main_quit();
}


void smpldt_clbk_menu_edit (GtkMenuItem *menuitem, gpointer user_data) {
	GtkTextBuffer * pTxtBuff;
	GtkWidget * pMenuItem = NULL;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, NULL);

	pMenuItem = simpledit_content_get_widget(pEditData, "menuEditUndo");
	gtk_widget_set_sensitive(pMenuItem, gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(pTxtBuff)));

	pMenuItem = simpledit_content_get_widget(pEditData, "menuEditRedo");
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(pTxtBuff)));
	
	pMenuItem = simpledit_content_get_widget(pEditData, "menuEditCut");
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_text_buffer_get_has_selection(pTxtBuff));
	pMenuItem = simpledit_content_get_widget(pEditData, "menuEditCopy");
	gtk_widget_set_sensitive(GTK_WIDGET(pMenuItem), gtk_text_buffer_get_has_selection(pTxtBuff));
	
	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_undo (GtkMenuItem *menuitem, gpointer user_data) {
	GtkSourceBuffer * pTxtBuff;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, NULL);
	gtk_source_buffer_undo(pTxtBuff);
	
	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_redo (GtkMenuItem *menuitem, gpointer user_data) {
	GtkSourceBuffer * pTxtBuff;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, NULL);
	gtk_source_buffer_redo(pTxtBuff);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_cut (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_cut_clipboard(pTxtBuff, pClipboard, bWritable);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_copy (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_copy_clipboard(pTxtBuff, pClipboard);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_paste (GtkMenuItem *menuitem, gpointer user_data) {
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	g_object_get(pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_paste_clipboard(pTxtBuff, pClipboard, NULL, bWritable);

	g_object_unref(pTxtBuff);
}


void smpldt_clbk_menu_search_find (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_search_replace (GtkMenuItem *menuitem, gpointer user_data) {
}


void smpldt_clbk_menu_about (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pWndEdit;
	const gchar * pcAuthors[] = {
		PACKAGE_BUGREPORT,
		NULL
	};
	
	g_object_get(pEditData, "window", &pWndEdit, NULL);
	
	gtk_show_about_dialog (GTK_WINDOW(pWndEdit),
		"authors", pcAuthors,
		"comments", "Another small/simple editor based on GtkSourceView.\nIt try to make accessible every features of GtkSourceView.",
		"copyright", "(c) 2018 Didier Prolhac",
		"license", "MIT",
		"license-type", GTK_LICENSE_MIT_X11,
		"program-name", PACKAGE_NAME,
		"version", PACKAGE_VERSION,
		"website", "https://github.com/thepozer/simplEdit",
		"website-label", "github simplEdit",
		NULL);

	g_object_unref(pWndEdit);
}
