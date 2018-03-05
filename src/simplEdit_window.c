#include "simplEdit_window.h"
#include "simplEdit_pref_dialog.h"
#include "simplEdit_search_dialog.h"

struct _SimpleditAppWindow {
	GtkApplicationWindow parent;

	SimpleditContent * pEditData;

	GtkSourceView * pSrcView;
	GtkStatusbar  * statusBar;
	
	GtkMenuItem * menuFileSave;
	GtkMenuItem * menuFileSaveAs;
	GtkMenuItem * menuFileReturnToSaved;
	
	GtkMenuItem * menuEditUndo;
	GtkMenuItem * menuEditRedo;
	GtkMenuItem * menuEditCut;
	GtkMenuItem * menuEditCopy;

	GtkMenuItem * menuLanguage;
	
	GtkMenuItem * menuSearchFind;
	GtkMenuItem * menuSearchReplace;
	
	SimpleditSearchDialog * pSearchDlg;
	
	guint iSttsIdPosition;
};

G_DEFINE_TYPE(SimpleditAppWindow, simpledit_app_window, GTK_TYPE_APPLICATION_WINDOW);

typedef struct _sMenuLangItem {
	SimpleditAppWindow *pWin;
	GtkSourceLanguage * pSelLang;
} sMenuLangItem;

void smpldt_clbk_text_changed (GtkTextBuffer *textbuffer, gpointer user_data);
void smpldt_clbk_cursor_position_changed (GtkTextBuffer *textbuffer, GParamSpec *pspec, gpointer user_data);

static void simpledit_app_window_init (SimpleditAppWindow *pWindow) {
	gtk_widget_init_template(GTK_WIDGET(pWindow));
	
	GtkMenu * pMnuLanguages = simpledit_app_window_get_language_menu(pWindow);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pWindow->menuLanguage), GTK_WIDGET(pMnuLanguages));
	
	pWindow->pSearchDlg = NULL;
	
	pWindow->iSttsIdPosition = gtk_statusbar_get_context_id (pWindow->statusBar, "Cursor position");
}

static void simpledit_app_window_class_init (SimpleditAppWindowClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditAppWindow.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, pSrcView);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, statusBar);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSave);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSaveAs);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileReturnToSaved);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditUndo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditRedo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCut);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCopy);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuLanguage);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuSearchFind);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuSearchReplace);
}

SimpleditAppWindow * simpledit_app_window_new (SimpleditApp *pApp) {
	SimpleditAppWindow * pWindow = g_object_new (SIMPLEDIT_APP_WINDOW_TYPE, "application", pApp, NULL);
	
	GtkTextBuffer * pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pWindow->pSrcView));
	g_signal_connect (pTxtBuff, "changed", G_CALLBACK (smpldt_clbk_text_changed), pWindow);
	g_signal_connect (pTxtBuff, "notify::cursor-position", G_CALLBACK (smpldt_clbk_cursor_position_changed), pWindow);
	
	pWindow->pEditData = simpledit_content_new(GTK_WINDOW(pWindow), pWindow->pSrcView);
	
	simpledit_content_update_highlight(pWindow->pEditData, NULL);
	g_signal_emit_by_name(pTxtBuff, "changed", pWindow);
	
	return pWindow;
}

void simpledit_app_window_open (SimpleditAppWindow *pWindow, GFile *pFile) {
	gchar * pcFileName = NULL;
	
	pcFileName = g_file_get_path(pFile);
	if (simpledit_content_set_filename(pWindow->pEditData, pcFileName)) {
		simpledit_content_load(pWindow->pEditData);
	} else {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pWindow), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, 
								GTK_BUTTONS_CLOSE, "Error openning file '%s' : (%i) %s", pcFileName);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	}
	
	g_free(pcFileName);
}

SimpleditContent * simpledit_app_window_get_content (SimpleditAppWindow *pWindow) {
	return pWindow->pEditData;
}

void simpledit_app_window_clear_search_dialog (SimpleditAppWindow *pWindow) {
	pWindow->pSearchDlg = NULL;
}

void smpldt_clbk_cursor_position_changed (GtkTextBuffer *textbuffer, GParamSpec *pspec, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	gchar * pcStatus = NULL;
	
	pcStatus = simpledit_content_get_status(pWindow->pEditData);
	gtk_statusbar_push(pWindow->statusBar, pWindow->iSttsIdPosition, pcStatus);
	g_free(pcStatus);
}

void smpldt_clbk_text_changed (GtkTextBuffer *textbuffer, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	simpledit_content_update_title(pWindow->pEditData);
}

void smpldt_clbk_menu_file (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkWidget * pMenuItem = NULL;
	gboolean bModified, bHaveFilename;
	
	bModified = simpledit_content_is_modified(pWindow->pEditData);
	bHaveFilename = simpledit_content_have_filename(pWindow->pEditData);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSave), bModified && bHaveFilename);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSaveAs), bModified);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileReturnToSaved), bModified && bHaveFilename);
}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);

	simpledit_content_reset(pWindow->pEditData);
	simpledit_content_update_title(pWindow->pEditData);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_select_name(pWindow->pEditData, GTK_FILE_CHOOSER_ACTION_OPEN)) {
		if (simpledit_content_load(pWindow->pEditData)) {
			simpledit_content_update_title(pWindow->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_have_filename(pWindow->pEditData)) {
		simpledit_content_save(pWindow->pEditData);
	}
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_select_name(pWindow->pEditData, GTK_FILE_CHOOSER_ACTION_SAVE)) {
		if (simpledit_content_save(pWindow->pEditData)) {
			simpledit_content_update_title(pWindow->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_have_filename(pWindow->pEditData)) {
		simpledit_content_load(pWindow->pEditData);
	}
}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWindow = GTK_WINDOW(user_data);

	gtk_window_close(pWindow);
}


void smpldt_clbk_menu_edit (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkTextBuffer * pTxtBuff;
	GtkWidget * pMenuItem = NULL;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, NULL);

	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuEditUndo), gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(pTxtBuff)));

	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuEditRedo), gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(pTxtBuff)));
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuEditCut), gtk_text_buffer_get_has_selection(pTxtBuff));
	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuEditCopy), gtk_text_buffer_get_has_selection(pTxtBuff));
	
	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_undo (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkSourceBuffer * pTxtBuff;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, NULL);
	gtk_source_buffer_undo(pTxtBuff);
	
	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_redo (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkSourceBuffer * pTxtBuff;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, NULL);
	gtk_source_buffer_redo(pTxtBuff);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_cut (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_cut_clipboard(pTxtBuff, pClipboard, bWritable);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_copy (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_copy_clipboard(pTxtBuff, pClipboard);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_paste (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	g_object_get(pWin->pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
	
	pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
	gtk_text_buffer_paste_clipboard(pTxtBuff, pClipboard, NULL, bWritable);

	g_object_unref(pTxtBuff);
}

void smpldt_clbk_menu_edit_preference (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditApp * pApp = NULL;
	SimpleditPrefDialog * pDialog = NULL;
	
	pApp = SIMPLEDIT_APP(gtk_window_get_application(GTK_WINDOW(pWindow)));
	
	pDialog = simpledit_pref_dialog_new(pWindow, simpledit_app_get_settings(pApp));
	gtk_dialog_run(GTK_DIALOG(pDialog));
}

void smpldt_clbk_menu_language_item (GtkMenuItem *menuitem, gpointer user_data) {
	sMenuLangItem * pMnuItemData = (sMenuLangItem *)user_data;
	
	simpledit_content_update_highlight(pMnuItemData->pWin->pEditData, pMnuItemData->pSelLang);
}

void smpldt_clbk_menu_search (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchFind), (pWindow->pSearchDlg == NULL));
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchReplace), (pWindow->pSearchDlg == NULL));
}

void smpldt_clbk_menu_search_find (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditSearchDialog * pDialog = NULL;
	
	if (!pWindow->pSearchDlg) {
		pWindow->pSearchDlg = simpledit_search_dialog_new(pWindow, FALSE);
		gtk_window_present(GTK_WINDOW(pWindow->pSearchDlg));
	}
}

void smpldt_clbk_menu_search_replace (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditSearchDialog * pDialog = NULL;
	
	if (!pWindow->pSearchDlg) {
		pWindow->pSearchDlg = simpledit_search_dialog_new(pWindow, TRUE);
		gtk_window_present(GTK_WINDOW(pWindow->pSearchDlg));
	}
}


void smpldt_clbk_menu_about (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	const gchar * pcAuthors[] = {
		PACKAGE_BUGREPORT,
		NULL
	};
	
	gtk_show_about_dialog (GTK_WINDOW(pWin),
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
}

GtkMenu * simpledit_app_window_get_language_menu(SimpleditAppWindow *pWindow) {
	GtkSourceLanguageManager * pLangMngr = NULL;
	GtkSourceLanguage * pLang = NULL;
	GtkWidget * pMnuMain    = NULL;
	GtkWidget * pMnuSection = NULL;
	GtkWidget * pMnuItem    = NULL;
	sMenuLangItem * pMnuItemData = NULL;
	GHashTable * pHash = NULL;
	const gchar * const * arpcLangIds = NULL;
	const gchar * const * arpcPtrIds  = NULL;
	const gchar * pcCurrentId = NULL;
	
	pLangMngr = gtk_source_language_manager_get_default();
	arpcLangIds = gtk_source_language_manager_get_language_ids(pLangMngr);
	
	pHash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);		
	arpcPtrIds = arpcLangIds;
	for (pcCurrentId = *arpcPtrIds; pcCurrentId; pcCurrentId = *++arpcPtrIds) {
		const gchar * pcSection, * pcName;
		pLang = gtk_source_language_manager_get_language(pLangMngr, pcCurrentId);
		
		pcSection = gtk_source_language_get_section(pLang);
		pcName    = gtk_source_language_get_name(pLang);
		
		if (!gtk_source_language_get_hidden(pLang)) {
			pMnuSection = GTK_WIDGET(g_hash_table_lookup(pHash, pcSection));
			
			if (pMnuSection == NULL) {
				pMnuSection = gtk_menu_new();
				gtk_widget_show (pMnuSection);
				
				g_hash_table_insert(pHash, (gchar *)pcSection, pMnuSection);
			}
			
			pMnuItem = gtk_menu_item_new_with_label(pcName);
			gtk_widget_show (pMnuItem);
			gtk_menu_shell_append (GTK_MENU_SHELL(pMnuSection), pMnuItem);
			
			pMnuItemData = g_new0(sMenuLangItem, 1);
			pMnuItemData->pWin = pWindow;
			pMnuItemData->pSelLang = pLang;
			g_signal_connect (pMnuItem, "activate", G_CALLBACK (smpldt_clbk_menu_language_item), pMnuItemData);
		}
		
	}
	
	pMnuMain = gtk_menu_new();
	gtk_widget_show(pMnuMain);
	
	GList * pListSection = g_hash_table_get_keys(pHash);
	int g_str_cmp (gconstpointer a, gconstpointer b) { return g_strcmp0(a, b); }
	pListSection = g_list_sort(pListSection, g_str_cmp);
	
	void addSectionMenu(gpointer data, gpointer user_data) {
		pMnuSection = GTK_WIDGET(g_hash_table_lookup(pHash, data));
		
		pMnuItem = gtk_menu_item_new_with_label((gchar *)data);
		gtk_widget_show (pMnuItem);
		gtk_menu_shell_append (GTK_MENU_SHELL(pMnuMain), pMnuItem);
		
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMnuItem), pMnuSection);
	}
	g_list_foreach(pListSection, addSectionMenu, NULL);
	
	
	g_hash_table_unref(pHash);
	return GTK_MENU(pMnuMain);
}
