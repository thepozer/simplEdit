#include "simplEdit_window.h"
#include "simplEdit_content.h"

GtkWidget * pWndEdit = NULL;

SEditorData gblData = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, TRUE} ;

void simplEdit_window_updateTitle(SEditorData * pEditData);

void simplEdit_window_init(GtkBuilder * pBuilder) {
	simplEdit_content_init(&gblData, pBuilder);
	
	gtk_builder_connect_signals(gblData.pBuilder, &gblData);

	gtk_widget_show_all(gblData.pWndEdit);
	
	simplEdit_window_updateTitle(&gblData);
}

void simplEdit_window_updateTitle(SEditorData * pEditData) {
	if (pEditData->pcFiletitle != NULL) {
		gchar * pcTitle = g_strdup_printf("simplEdit - %s", pEditData->pcFiletitle);
		gtk_window_set_title(GTK_WINDOW(pEditData->pWndEdit), pcTitle);
		g_free(pcTitle);
	} else {
		gtk_window_set_title(GTK_WINDOW(pEditData->pWndEdit), "simplEdit");
	}
}


void smpldt_clbk_menu_file (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pMenuItem = NULL;
	gboolean bModified = FALSE;
	
	bModified = gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(gblData.pTxtBuff));
	
	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuFileSave"));
	gtk_widget_set_sensitive(pMenuItem, bModified && (gblData.pcFilename != NULL));

	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuFileSaveAs"));
	gtk_widget_set_sensitive(pMenuItem, bModified);
	
	pMenuItem = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "menuFileReturnToSaved"));
	gtk_widget_set_sensitive(pMenuItem, bModified && (gblData.pcFilename != NULL));

}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	simplEdit_content_reset(&gblData);
	simplEdit_window_updateTitle(&gblData);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) { 
	GtkWidget * pDlgFile;
	gint iResult;

	pDlgFile = gtk_file_chooser_dialog_new (
		"Open File", GTK_WINDOW(gblData.pWndEdit),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Open", GTK_RESPONSE_ACCEPT,
		NULL);

	if (gblData.pcFilename) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), gblData.pcFilename);
	}
	
	iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
//g_print("smpldt_clbk_menu_file_open - iResult : %i\n", iResult);
	if (iResult == GTK_RESPONSE_ACCEPT) {
		simplEdit_content_reset(&gblData);
		
		GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
		gchar * pcFilename = gtk_file_chooser_get_filename (pChooser);
//g_print("smpldt_clbk_menu_file_open - pcFilename : '%s'\n", pcFilename);
		
		if (simplEdit_content_load(&gblData, pcFilename)) {
			simplEdit_window_updateTitle(&gblData);
		}
	}
	gtk_widget_destroy (pDlgFile);
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	if (gblData.pcFilename != NULL) {
		simplEdit_content_save(&gblData, NULL);
	}
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pDlgFile;
	gint iResult;

	pDlgFile = gtk_file_chooser_dialog_new (
			"Open File", GTK_WINDOW(gblData.pWndEdit),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			"_Cancel", GTK_RESPONSE_CANCEL,
			"_Save", GTK_RESPONSE_ACCEPT,
			NULL);
	
	if (gblData.pcFilename) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), gblData.pcFilename);
	} else {
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(pDlgFile), "New file");
	}
	
	iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
//g_print("smpldt_clbk_menu_file_saveas - iResult : %i\n", iResult);
	if (iResult == GTK_RESPONSE_ACCEPT) {
		GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
		gchar * pcFilename = NULL;
		
		pcFilename = gtk_file_chooser_get_filename (pChooser);
		
		if (simplEdit_content_save(&gblData, pcFilename)) {
			simplEdit_window_updateTitle(&gblData);
		}
		
		g_free(pcFilename);
	}

	gtk_widget_destroy (pDlgFile);
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	if (gblData.pcFilename != NULL) {
		simplEdit_content_load(&gblData, NULL);
	}
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
