#include "simplEdit_window.h"
#include "simplEdit_search_dialog.h"

struct _SimpleditSearchDialog {
	GtkDialog parent;
	
	SimpleditAppWindow * pWindow;
	
	GtkSourceSearchSettings * pSearchSettings;
	GtkSourceSearchContext  * pSearchContext;
	GtkTextIter * pStartIter;
	GtkTextIter sStartIter;
	
	GtkWidget * txtSearchText;
	GtkWidget * txtReplaceText;
	
	GtkWidget * checkWordBoundaries;
	GtkWidget * checkCaseSensitive;
	GtkWidget * checkRegexEnabled;
	GtkWidget * checkWrapAround;

	GtkWidget * checkHighlightSearch;
};

G_DEFINE_TYPE(SimpleditSearchDialog, simpledit_search_dialog, GTK_TYPE_DIALOG);

static void simpledit_search_dialog_init (SimpleditSearchDialog *pDialog) {
	gtk_widget_init_template(GTK_WIDGET(pDialog));
	
	pDialog->pSearchSettings = NULL;
	pDialog->pSearchContext = NULL;
	pDialog->pStartIter = g_new0(GtkTextIter, 1);
}

static void simpledit_search_dialog_class_init (SimpleditSearchDialogClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditSearchDialog.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, txtSearchText);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, txtReplaceText);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, checkWordBoundaries);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, checkCaseSensitive);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, checkRegexEnabled);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditSearchDialog, checkWrapAround);
}

SimpleditSearchDialog * simpledit_search_dialog_new (SimpleditAppWindow *pWindow) {
	SimpleditSearchDialog * pDialog = g_object_new(SIMPLEDIT_SEARCH_DIALOG_TYPE, "transient-for", pWindow, NULL);
	
	pDialog->pWindow = pWindow;
	
	return pDialog;
}

void smpldt_searchdlg_clbk_close (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWindow = GTK_WINDOW(user_data);

	gtk_window_close(pWindow);
}

void smpldt_searchdlg_clbk_search (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditSearchDialog *pDialog = SIMPLEDIT_SEARCH_DIALOG(user_data);
	SimpleditContent * pContent = NULL;
	GtkSourceView * pSrcView = NULL;
	GtkSourceBuffer * pSrcBuff = NULL;
	GtkTextMark * pMark = NULL;
	GtkTextIter * pEndIter = g_new0(GtkTextIter, 1);
	GtkTextIter sEndIter;
	
	pContent = simpledit_app_window_get_content(pDialog->pWindow);
	g_object_get(pContent, "textbuffer", &pSrcBuff, "sourceview", &pSrcView, NULL);
	
	if (!pDialog->pSearchSettings) {
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(pSrcBuff), pDialog->pStartIter);
		
		pDialog->pSearchSettings = gtk_source_search_settings_new();
		pDialog->pSearchContext  = gtk_source_search_context_new(pSrcBuff, pDialog->pSearchSettings);
	}
	
	gtk_source_search_settings_set_at_word_boundaries (pDialog->pSearchSettings, 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pDialog->checkWordBoundaries)));
	gtk_source_search_settings_set_case_sensitive(pDialog->pSearchSettings, 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pDialog->checkCaseSensitive)));
	gtk_source_search_settings_set_regex_enabled (pDialog->pSearchSettings, 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pDialog->checkRegexEnabled)));
	gtk_source_search_settings_set_wrap_around(pDialog->pSearchSettings, 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pDialog->checkWrapAround)));
	gtk_source_search_settings_set_search_text(pDialog->pSearchSettings,
			gtk_entry_get_text(GTK_ENTRY(pDialog->txtSearchText)));
			
	if (gtk_source_search_context_forward2(pDialog->pSearchContext, pDialog->pStartIter, pDialog->pStartIter, pEndIter, NULL)) {
		gtk_text_buffer_move_mark_by_name(GTK_TEXT_BUFFER(pSrcBuff), "selection_bound", pDialog->pStartIter);
		gtk_text_buffer_move_mark_by_name(GTK_TEXT_BUFFER(pSrcBuff), "insert", pEndIter);
		gtk_text_iter_assign(pDialog->pStartIter, pEndIter);
		
		pMark = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(pSrcBuff), "insert");
		gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(pSrcView), pMark);
	} else {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pDialog->pWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "No more item found !");
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
		
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(pSrcBuff), pDialog->pStartIter);
	}
	
	g_free(pEndIter);
}

void smpldt_searchdlg_clbk_replace (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditSearchDialog *pDialog = SIMPLEDIT_SEARCH_DIALOG(user_data);

}

