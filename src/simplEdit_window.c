#include "simplEdit_window.h"
#include "simplEdit_pref_dialog.h"
#include "simplEdit_search_dialog.h"

struct _SimpleditAppWindow {
	GtkApplicationWindow parent;

	SimpleditContent * pEditData;
	
	GtkNotebook * bookEditors;

	GtkStatusbar  * statusBar;
	
	GtkMenuItem * menuFileSave;
	GtkMenuItem * menuFileSaveAs;
	GtkMenuItem * menuFileReturnToSaved;
	GtkMenuItem * menuFileClose;
	

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

void simpledit_app_window_update_status (SimpleditAppWindow *pWindow);
void smpldt_clbk_notebook_switch_page (GtkNotebook * bookEditors, GtkWidget * pChild, guint page_num, gpointer user_data);

typedef struct _sMenuLangItem {
	SimpleditAppWindow *pWin;
	GtkSourceLanguage * pSelLang;
} sMenuLangItem;

static void simpledit_app_window_init (SimpleditAppWindow *pWindow) {
	gtk_widget_init_template(GTK_WIDGET(pWindow));
	
	GtkMenu * pMnuLanguages = simpledit_app_window_get_language_menu(pWindow);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pWindow->menuLanguage), GTK_WIDGET(pMnuLanguages));
	
	pWindow->pSearchDlg = NULL;
	
	pWindow->iSttsIdPosition = gtk_statusbar_get_context_id (pWindow->statusBar, "Cursor position");
}

static void simpledit_app_window_class_init (SimpleditAppWindowClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditAppWindow.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, bookEditors);

	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, statusBar);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSave);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSaveAs);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileReturnToSaved);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileClose);
	
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
	
	pWindow->pEditData = NULL;
	
	return pWindow;
}

void simpledit_app_window_open (SimpleditAppWindow *pWindow, GFile *pFile) {
	gchar * pcFileName = NULL;
	
	
	pcFileName = g_file_get_path(pFile);
	
	pWindow->pEditData = simpledit_content_new(GTK_WINDOW(pWindow));
	
	if (simpledit_content_set_filename(pWindow->pEditData, pcFileName)) {
		simpledit_content_add_to_stack(pWindow->pEditData, pWindow->bookEditors);
		if (simpledit_content_load(pWindow->pEditData)) {
			simpledit_app_window_update_status(pWindow);
			simpledit_content_update_highlight(pWindow->pEditData, NULL);
			gtk_notebook_set_current_page(pWindow->bookEditors, -1);
		}
	} else {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pWindow), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, 
								GTK_BUTTONS_CLOSE, _("Error openning file '%s' : (%i) %s"), pcFileName);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	}
	
	g_free(pcFileName);
}

gboolean simpledit_app_window_close_all (SimpleditAppWindow *pWindow) {
	SimpleditContent * pEditData;
	GtkWidget * pPageChild = NULL;
	gint iNbPages = -1;
	
	iNbPages = gtk_notebook_get_n_pages(pWindow->bookEditors);
//g_print("simpledit_app_window_close_all - iNbPages : %d\n", iNbPages);
	for (int iPage = iNbPages - 1; iPage >= 0; iPage --) {
//g_print("simpledit_app_window_close_all - Closing page : %d - 01\n", iPage);
		pPageChild = gtk_notebook_get_nth_page(pWindow->bookEditors, iPage);
//g_print("simpledit_app_window_close_all - Closing page : %d - 02 - %d\n", iPage, (gint)pPageChild);
		pEditData = g_object_get_data(G_OBJECT(pPageChild), "content_data");
//g_print("simpledit_app_window_close_all - Closing page : %d - 03\n", iPage);
		
		if(!simpledit_content_close(pEditData)) {
//g_print("simpledit_app_window_close_all - Closing page : %d - 04 - return FALSE\n", iPage);
			return FALSE;
		}
//g_print("simpledit_app_window_close_all - Closing page : %d - 05\n", iPage);
	}
	
//g_print("simpledit_app_window_close_all - return TRUE\n", iNbPages);
	return TRUE;
}

SimpleditContent * simpledit_app_window_get_content (SimpleditAppWindow *pWindow) {
	return pWindow->pEditData;
}

void simpledit_app_window_clear_search_dialog (SimpleditAppWindow *pWindow) {
	pWindow->pSearchDlg = NULL;
}

void simpledit_app_window_update_status (SimpleditAppWindow *pWindow) {
	gchar * pcStatus = NULL;
	
	if (pWindow->pEditData) {
		simpledit_content_update_title(pWindow->pEditData);
		pcStatus = simpledit_content_get_status(pWindow->pEditData);
		gtk_statusbar_push(pWindow->statusBar, pWindow->iSttsIdPosition, pcStatus);
		g_free(pcStatus);
	} else {
		gtk_statusbar_push(pWindow->statusBar, pWindow->iSttsIdPosition, "");
	}
}

void smpldt_clbk_notebook_switch_page (GtkNotebook * bookEditors, GtkWidget * pChild, guint page_num, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditContent * pTmpEditData = NULL;
	
	pTmpEditData = g_object_get_data(G_OBJECT(pChild), "content_data");
	if (pTmpEditData != NULL) {
		pWindow->pEditData = pTmpEditData;
	}
	
}

void smpldt_clbk_cursor_position_changed (GtkTextBuffer *textbuffer, GParamSpec *pSpec, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	simpledit_app_window_update_status(pWindow);
}

void smpldt_clbk_text_changed (GtkTextBuffer *textbuffer, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	simpledit_app_window_update_status(pWindow);
}

void smpldt_clbk_mark_set (GtkTextBuffer * textbuffer, GtkTextIter * location, GtkTextMark * mark, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	simpledit_app_window_update_status(pWindow);
}

void smpldt_clbk_menu_file (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkWidget * pMenuItem = NULL;
	gboolean bModified = FALSE, bHaveFilename = FALSE;
	
	if (pWindow->pEditData) {
		bModified = simpledit_content_is_modified(pWindow->pEditData);
		bHaveFilename = simpledit_content_have_filename(pWindow->pEditData);
	}
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSave), bModified && bHaveFilename);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSaveAs), bHaveFilename);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileReturnToSaved), bModified && bHaveFilename);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileClose), pWindow->pEditData != NULL);
}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);

	pWindow->pEditData = simpledit_content_new(GTK_WINDOW(pWindow));
	simpledit_content_add_to_stack(pWindow->pEditData, pWindow->bookEditors);
	simpledit_content_update_highlight(pWindow->pEditData, NULL);
	simpledit_content_update_title(pWindow->pEditData);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditContent * pTmpEditData;
	
	pTmpEditData = simpledit_content_new(GTK_WINDOW(pWindow));
	if (simpledit_content_select_name(pTmpEditData, GTK_FILE_CHOOSER_ACTION_OPEN)) {
		pWindow->pEditData = pTmpEditData;
		simpledit_content_add_to_stack(pWindow->pEditData, pWindow->bookEditors);
		if (simpledit_content_load(pWindow->pEditData)) {
			simpledit_app_window_update_status(pWindow);
			simpledit_content_update_highlight(pWindow->pEditData, NULL);
			gtk_notebook_set_current_page(pWindow->bookEditors, -1);
		}
	}
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		if (simpledit_content_have_filename(pWindow->pEditData)) {
			simpledit_content_save(pWindow->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		if (simpledit_content_select_name(pWindow->pEditData, GTK_FILE_CHOOSER_ACTION_SAVE)) {
			if (simpledit_content_save(pWindow->pEditData)) {
				simpledit_content_update_title(pWindow->pEditData);
			}
		}
	}
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		if (simpledit_content_have_filename(pWindow->pEditData)) {
			simpledit_content_load(pWindow->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_close (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditContent * pTmpEditData = pWindow->pEditData;
	
	pWindow->pEditData = NULL;
	if (!simpledit_content_close(pTmpEditData)) {
		pWindow->pEditData = pTmpEditData;
	}
}

void smpldt_clbk_menu_file_close_all (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	simpledit_app_window_close_all(pWindow);
}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWindow = GTK_WINDOW(user_data);
	SimpleditApp * pApp = NULL;
	
	pApp = SIMPLEDIT_APP(gtk_window_get_application(GTK_WINDOW(pWindow)));
	simpledit_app_quit(pApp); 
}


void smpldt_clbk_menu_edit (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkTextBuffer * pTxtBuff;
	GtkWidget * pMenuItem = NULL;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, NULL);

		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditUndo), gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(pTxtBuff)));

		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditRedo), gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(pTxtBuff)));
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCut), gtk_text_buffer_get_has_selection(pTxtBuff));
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCopy), gtk_text_buffer_get_has_selection(pTxtBuff));
		
		g_object_unref(pTxtBuff);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditUndo), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditRedo), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCut), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCopy), FALSE);
	}
}

void smpldt_clbk_menu_edit_undo (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkSourceBuffer * pTxtBuff;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, NULL);
		gtk_source_buffer_undo(pTxtBuff);
		
		g_object_unref(pTxtBuff);
	}
}

void smpldt_clbk_menu_edit_redo (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkSourceBuffer * pTxtBuff;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, NULL);
		gtk_source_buffer_redo(pTxtBuff);

		g_object_unref(pTxtBuff);
	}
}

void smpldt_clbk_menu_edit_cut (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
		
		pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
		gtk_text_buffer_cut_clipboard(pTxtBuff, pClipboard, bWritable);

		g_object_unref(pTxtBuff);
	}
}

void smpldt_clbk_menu_edit_copy (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, NULL);
		
		pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
		gtk_text_buffer_copy_clipboard(pTxtBuff, pClipboard);

		g_object_unref(pTxtBuff);
	}
}

void smpldt_clbk_menu_edit_paste (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GtkClipboard * pClipboard = NULL;
	GtkTextBuffer * pTxtBuff;
	gboolean bWritable;

	if (pWindow->pEditData) {
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, "writable", &bWritable, NULL);
		
		pClipboard = gtk_clipboard_get(gdk_atom_intern ("CLIPBOARD", FALSE));
		gtk_text_buffer_paste_clipboard(pTxtBuff, pClipboard, NULL, bWritable);

		g_object_unref(pTxtBuff);
	}
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
	
	if (pMnuItemData->pWin->pEditData) {
		simpledit_content_update_highlight(pMnuItemData->pWin->pEditData, pMnuItemData->pSelLang);
	}
}

void smpldt_clbk_menu_search (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (!pWindow->pEditData) {
		pWindow->pSearchDlg = NULL;
	}
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchFind), (pWindow->pSearchDlg == NULL));
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchReplace), (pWindow->pSearchDlg == NULL));
}

void smpldt_clbk_menu_search_find (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditSearchDialog * pDialog = NULL;
	
	if (pWindow->pEditData && !pWindow->pSearchDlg) {
		pWindow->pSearchDlg = simpledit_search_dialog_new(pWindow, FALSE);
		gtk_window_present(GTK_WINDOW(pWindow->pSearchDlg));
	}
}

void smpldt_clbk_menu_search_replace (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditSearchDialog * pDialog = NULL;
	
	if (pWindow->pEditData && !pWindow->pSearchDlg) {
		pWindow->pSearchDlg = simpledit_search_dialog_new(pWindow, TRUE);
		gtk_window_present(GTK_WINDOW(pWindow->pSearchDlg));
	}
}


void smpldt_clbk_menu_about (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	const gchar * pcAuthors[] = {
		PACKAGE_BUGREPORT,
		NULL
	};
	
	gtk_show_about_dialog (GTK_WINDOW(pWindow),
		"authors", pcAuthors,
		"comments", _("Another small/simple editor based on GtkSourceView.\nIt try to make accessible every features of GtkSourceView."),
		"copyright", _("(c) 2018 Didier Prolhac"),
		"license", _("MIT"),
		"license-type", GTK_LICENSE_MIT_X11,
		"program-name", PACKAGE_NAME,
		"version", PACKAGE_VERSION,
		"website", "https://github.com/thepozer/simplEdit",
		"website-label", _("github simplEdit"),
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
