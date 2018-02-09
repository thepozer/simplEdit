#include "simplEdit_window.h"
#include "simplEdit_content.h"

GtkWidget * pWndEdit = NULL;

SEditorData gblData = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, TRUE} ;
SimpleditContent * pEditData = NULL;

GtkWidget * simplEdit_window_extraWidget(SEditorData * pEditData);

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
	GtkWidget * pDlgFile, * pWndEdit;
	gchar * pcFilename;
	gint iResult;

	g_object_get(pEditData, "window", &pWndEdit, "filename", &pcFilename, NULL);

	pDlgFile = gtk_file_chooser_dialog_new (
		"Open File", GTK_WINDOW(pWndEdit),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Open", GTK_RESPONSE_ACCEPT,
		NULL);

	if (pcFilename) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), pcFilename);
	}
	
	iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
//g_print("smpldt_clbk_menu_file_open - iResult : %i\n", iResult);
	if (iResult == GTK_RESPONSE_ACCEPT) {
		simpledit_content_reset(pEditData);
		
		GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
		gchar * pcNewFilename = gtk_file_chooser_get_filename (pChooser);
//g_print("smpldt_clbk_menu_file_open - pcNewFilename : '%s'\n", pcNewFilename);
		
		if (simpledit_content_load(pEditData, pcNewFilename)) {
			simpledit_content_update_title(pEditData);
		}
		
		g_free(pcNewFilename);
	}
	gtk_widget_destroy (pDlgFile);
	
	g_object_unref(pWndEdit);
	g_free(pcFilename);
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	if (simpledit_content_have_filename(pEditData)) {
		simpledit_content_save(pEditData, NULL);
	}
}

GtkWidget * simplEdit_window_extraWidget(SEditorData * pEditData) {
	GtkWidget * pHBox, * pLabel, * pLstEncoding, * pLstEndOfLines, * pLstCompress;
	GtkCellRenderer * pCellRndr = NULL;
	GtkListStore * pLstModelEOL;
	GtkTreeIter iter, * pIterSelEOL;
	GtkSourceNewlineType typeEOL;
	
	pHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_widget_show(pHBox);
	
	pLabel = gtk_label_new("Encoding :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

	pLstEncoding = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pLstEncoding), "utf-8", "UTF-8");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pLstEncoding), "iso8859-1", "ISO8859-1 - latin1");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pLstEncoding), "iso8859-15", "ISO8859-15 - Latin9");
	gtk_widget_show(pLstEncoding);
	gtk_box_pack_start(GTK_BOX(pHBox), pLstEncoding, TRUE, TRUE, 1);

	pLabel = gtk_label_new("End of line :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);
	
	typeEOL = gtk_source_file_get_newline_type(pEditData->pSrcFile);
	pLstModelEOL = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_append (pLstModelEOL, &iter);
	gtk_list_store_set (pLstModelEOL, &iter, 0, GTK_SOURCE_NEWLINE_TYPE_LF,   1, "Unix (\\n)", -1);
	if (typeEOL == GTK_SOURCE_NEWLINE_TYPE_LF) {
		pIterSelEOL = gtk_tree_iter_copy(&iter);
	}
	gtk_list_store_append (pLstModelEOL, &iter);
	gtk_list_store_set (pLstModelEOL, &iter, 0, GTK_SOURCE_NEWLINE_TYPE_CR_LF, 1, "Windows (\\r\\n)", -1);
	if (typeEOL == GTK_SOURCE_NEWLINE_TYPE_LF) {
		pIterSelEOL = gtk_tree_iter_copy(&iter);
	}
	gtk_list_store_append (pLstModelEOL, &iter);
	gtk_list_store_set (pLstModelEOL, &iter, 0, GTK_SOURCE_NEWLINE_TYPE_CR,   1, "Mac (\\r)", -1);
	if (typeEOL == GTK_SOURCE_NEWLINE_TYPE_LF) {
		pIterSelEOL = gtk_tree_iter_copy(&iter);
	}
	pLstEndOfLines = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelEOL));
 	pCellRndr = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (pLstEndOfLines), pCellRndr, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (pLstEndOfLines), pCellRndr, "text", 1, NULL);
	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(pLstModelEOL), pIterSelEOL);
	gtk_widget_show(pLstEndOfLines);
	gtk_box_pack_start(GTK_BOX(pHBox), pLstEndOfLines, TRUE, TRUE, 1);

	pLabel = gtk_label_new("Compression :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

	pLstCompress = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pLstCompress), "none", "None");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pLstCompress), "gzip", "Gzip (.gz)");
	gtk_widget_show(pLstCompress);
	gtk_box_pack_start(GTK_BOX(pHBox), pLstCompress, TRUE, TRUE, 1);
	
	return pHBox;
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget * pDlgFile, * pWndEdit;
	gchar * pcFilename;
	gint iResult;

	g_object_get(pEditData, "window", &pWndEdit, "filename", &pcFilename, NULL);

	pDlgFile = gtk_file_chooser_dialog_new (
			"Open File", GTK_WINDOW(pWndEdit),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			"_Cancel", GTK_RESPONSE_CANCEL,
			"_Save", GTK_RESPONSE_ACCEPT,
			NULL);
	
	if (gblData.pcFilename) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), pcFilename);
	} else {
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(pDlgFile), "New file");
	}
	
	//GtkWidget * pHBox = simplEdit_window_extraWidget(&gblData);
	
	//gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(pDlgFile), pHBox);
	
	iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
//g_print("smpldt_clbk_menu_file_saveas - iResult : %i\n", iResult);
	if (iResult == GTK_RESPONSE_ACCEPT) {
		GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
		gchar * pcNewFilename = NULL;
		
		pcNewFilename = gtk_file_chooser_get_filename (pChooser);
		
		if (simpledit_content_save(pEditData, pcNewFilename)) {
			simpledit_content_update_title(pEditData);
		}
		
		g_free(pcNewFilename);
	}

	gtk_widget_destroy (pDlgFile);
	
	g_object_unref(pWndEdit);
	g_free(pcFilename);
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	if (simpledit_content_have_filename(pEditData)) {
		simpledit_content_load(pEditData, NULL);
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
