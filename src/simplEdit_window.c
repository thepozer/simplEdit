#include "simplEdit_window.h"

GtkWidget * pWndEdit = NULL;
typedef struct {
	GtkBuilder    * pBuilder;
	GtkWidget     * pWndEdit;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
        gchar * pcFilename;
        gchar * pcFiletitle;
	gboolean bWritable;
} SEditorData;

SEditorData gblData = {NULL, NULL, NULL, NULL, NULL, NULL, TRUE} ;

void simplEdit_window_init(GtkBuilder * pBuilder) {
	gblData.pBuilder = pBuilder;
	
	gblData.pWndEdit = GTK_WIDGET(gtk_builder_get_object(gblData.pBuilder, "wndSimplEdit"));
	gblData.pSrcView = GTK_SOURCE_VIEW(gtk_builder_get_object(gblData.pBuilder, "srcView"));
	gblData.pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gblData.pSrcView));
	
	gtk_builder_connect_signals(gblData.pBuilder, &gblData);

	gtk_widget_show_all(gblData.pWndEdit);

}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
        gtk_window_set_title(GTK_WINDOW(gblData.pWndEdit), "simplEdit");
	gtk_text_buffer_set_text(gblData.pTxtBuff, "", 0);
        g_free(gblData.pcFilename);
        g_free(gblData.pcFiletitle);
        gblData.pcFilename = NULL;
        gblData.pcFiletitle = NULL;
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

        iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
//g_print("smpldt_clbk_menu_file_open - iResult : %i\n", iResult);
        if (iResult == GTK_RESPONSE_ACCEPT) {
                smpldt_clbk_menu_file_new(menuitem, user_data);
                
                GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
                gblData.pcFilename = gtk_file_chooser_get_filename (pChooser);
//g_print("smpldt_clbk_menu_file_open - pcFilename : '%s'\n", gblData.pcFilename);
                
                GFile * pFile = g_file_new_for_path(gblData.pcFilename);
                GError * pErr = NULL;
                gchar * pcContent = NULL;
                gchar * pcTitle = NULL;
                
                if (pFile) {
                        gblData.pcFiletitle = g_file_get_basename(pFile);
//g_print("smpldt_clbk_menu_file_open - pcFiletitle : '%s'\n", gblData.pcFiletitle);
                        if (g_file_load_contents (pFile, NULL, &pcContent, NULL, NULL, &pErr)) {
                                if (pErr) {
                                        g_printerr("smpldt_clbk_menu_file_open - Error (%i) : '%s'\n", pErr->code, pErr->message);
                                        g_error_free (pErr);
                                        g_free(gblData.pcFilename);
                                        gblData.pcFilename = NULL;
                                        return;
                                }
//g_print("smpldt_clbk_menu_file_open - pcContent : \n%s\n", pcContent);
                                
                                gtk_text_buffer_set_text(gblData.pTxtBuff, pcContent, -1);                                
                                g_free(pcContent);
                                
                                pcTitle = g_strdup_printf("simplEdit - %s", gblData.pcFiletitle);
                                gtk_window_set_title(GTK_WINDOW(gblData.pWndEdit), pcTitle);
                                g_free(pcTitle);
                        }
                }
        }

        gtk_widget_destroy (pDlgFile);
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
