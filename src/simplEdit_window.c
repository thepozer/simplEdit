#include "simplEdit_window.h"
#include "simplEdit_pref_dialog.h"
#include "simplEdit_search_dialog.h"

struct _SimpleditAppWindow {
	GtkApplicationWindow parent;

	SimpleditContent * pEditData;
	
	GtkNotebook * bookEditors;

	GtkLabel    * sttsbrLabel;
	GtkButton   * sttsbrBtnLanguage;
	GtkButton   * sttsbrBtnInsOwr;
	
	GtkMenuItem * menuFileSave;
	GtkMenuItem * menuFileSaveAs;
	GtkMenuItem * menuFileReturnToSaved;
	GtkMenuItem * menuFileClose;
	GtkMenuItem * menuFileCloseAll;
	GtkMenuItem * menuFilePrint;
	
	GtkMenuItem * menuEditUndo;
	GtkMenuItem * menuEditRedo;
	GtkMenuItem * menuEditCut;
	GtkMenuItem * menuEditCopy;
	GtkMenuItem * menuEditPaste;

	GtkMenuItem * menuLanguage;
	
	GtkMenuItem * menuSearchFind;
	GtkMenuItem * menuSearchReplace;
	
	GtkMenu    * menuListLanguages;
	GtkWidget  * menuSelectedLanguage;
	GHashTable * pHashLanguage;

	SimpleditSearchDialog * pSearchDlg;
};

G_DEFINE_TYPE(SimpleditAppWindow, simpledit_app_window, GTK_TYPE_APPLICATION_WINDOW);

void simpledit_app_window_update_status (SimpleditAppWindow *pWindow);
void smpldt_clbk_notebook_switch_page (GtkNotebook * bookEditors, GtkWidget * pChild, guint page_num, gpointer user_data);

typedef struct _sMenuLangItem {
	SimpleditAppWindow *pWin;
	GtkSourceLanguage * pSelLang;
	GtkWidget * pMenuItem;
} sMenuLangItem;

static void simpledit_app_window_init (SimpleditAppWindow *pWindow) {
	gtk_widget_init_template(GTK_WIDGET(pWindow));
	
	pWindow->menuListLanguages = simpledit_app_window_get_language_menu(pWindow);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pWindow->menuLanguage), GTK_WIDGET(pWindow->menuListLanguages));
	pWindow->menuSelectedLanguage = NULL;
	
	pWindow->pSearchDlg = NULL;
}

static void simpledit_app_window_class_init (SimpleditAppWindowClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.SimpleditAppWindow.glade");
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, bookEditors);

	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, sttsbrLabel);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, sttsbrBtnLanguage);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, sttsbrBtnInsOwr);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSave);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSaveAs);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileReturnToSaved);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileClose);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileCloseAll);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFilePrint);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditUndo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditRedo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCut);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCopy);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditPaste);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuLanguage);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuSearchFind);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuSearchReplace);
}

gboolean smpldt_clbk_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(widget);
	
	return !simpledit_app_window_close_all(pWindow);
}

SimpleditAppWindow * simpledit_app_window_new (SimpleditApp *pApp) {
	SimpleditAppWindow * pWindow = g_object_new (SIMPLEDIT_APP_WINDOW_TYPE, "application", pApp, NULL);
	
	pWindow->pEditData = NULL;
	
	g_signal_connect(pWindow, "delete-event", G_CALLBACK(smpldt_clbk_delete_event), pWindow);
	
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSave),   FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSaveAs), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileReturnToSaved),FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileClose),    FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileCloseAll), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFilePrint),    FALSE);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditUndo),  FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditRedo),  FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCut),   FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCopy),  FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditPaste), FALSE);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuLanguage),  FALSE);

	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchFind),    FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchReplace), FALSE);
	
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
	for (int iPage = iNbPages - 1; iPage >= 0; iPage --) {
		pPageChild = gtk_notebook_get_nth_page(pWindow->bookEditors, iPage);
		pEditData = g_object_get_data(G_OBJECT(pPageChild), "content_data");
		
		if(!simpledit_content_close(pEditData)) {
			return FALSE;
		}
	}
	
	return TRUE;
}

SimpleditContent * simpledit_app_window_get_content (SimpleditAppWindow *pWindow) {
	return pWindow->pEditData;
}

void simpledit_app_window_clear_search_dialog (SimpleditAppWindow *pWindow) {
	pWindow->pSearchDlg = NULL;
}

void simpledit_app_window_clean_status (SimpleditAppWindow *pWindow) {
	if (gtk_notebook_get_n_pages(pWindow->bookEditors) < 1) {
		pWindow->pEditData = NULL;
	}
	
	simpledit_app_window_update_status(pWindow);
}

void simpledit_app_window_update_status (SimpleditAppWindow *pWindow) {
	gchar * pcStatus = NULL, * pcTypeFile = NULL;
	gboolean bModified = FALSE, bHaveFilename = FALSE, bOverwriteMode = FALSE;
	GtkTextBuffer * pTxtBuff;
	
	if (pWindow->pEditData) {
		simpledit_content_update_title(pWindow->pEditData);
		pcStatus = simpledit_content_get_status(pWindow->pEditData);
		gtk_label_set_label(pWindow->sttsbrLabel, pcStatus);
		g_free(pcStatus);
		
		bModified = simpledit_content_is_modified(pWindow->pEditData);
		bHaveFilename = simpledit_content_have_filename(pWindow->pEditData);
		bOverwriteMode = simpledit_content_get_overwrite(pWindow->pEditData);
		g_object_get(pWindow->pEditData, "textbuffer", &pTxtBuff, NULL);
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSave),   bModified && bHaveFilename);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSaveAs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileReturnToSaved), bModified && bHaveFilename);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileClose),    TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileCloseAll), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFilePrint),    TRUE);

		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditUndo),  gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(pTxtBuff)));
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditRedo),  gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(pTxtBuff)));
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCut),   gtk_text_buffer_get_has_selection(pTxtBuff));
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCopy),  gtk_text_buffer_get_has_selection(pTxtBuff));
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditPaste), TRUE);
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuLanguage),  TRUE);

		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchFind),    TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchReplace), TRUE);
		
		pcTypeFile = g_strdup_printf(_("File type : %s"), simpledit_content_get_language(pWindow->pEditData));
		gtk_button_set_label(pWindow->sttsbrBtnLanguage, pcTypeFile);
		g_free(pcTypeFile);
		
		gtk_button_set_label(pWindow->sttsbrBtnInsOwr, ((bOverwriteMode) ? _("OWR") : _("INS")));
		
		g_object_unref(pTxtBuff);
	} else {
		gtk_window_set_title(GTK_WINDOW(pWindow), "simplEdit");
		gtk_label_set_label(pWindow->sttsbrLabel, "-");
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSave),   FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileSaveAs), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileReturnToSaved), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileClose),    FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFileCloseAll), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuFilePrint),    FALSE);
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditUndo),  FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditRedo),  FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCut),   FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditCopy),  FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuEditPaste), FALSE);
		
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuLanguage),  FALSE);

		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchFind),    FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(pWindow->menuSearchReplace), FALSE);
	}
}

void smpldt_clbk_notebook_switch_page (GtkNotebook * bookEditors, GtkWidget * pChild, guint page_num, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	SimpleditContent * pTmpEditData = NULL;
	const gchar * pcLanguage = NULL;
	
	pTmpEditData = g_object_get_data(G_OBJECT(pChild), "content_data");
	if (pTmpEditData != NULL) {
		pWindow->pEditData = pTmpEditData;
		
		pcLanguage = simpledit_content_get_language(pWindow->pEditData);
		simpledit_app_window_select_language_in_menu(SIMPLEDIT_APP_WINDOW(pWindow), pcLanguage);
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

void smpldt_clbk_btn_sttsbr_btn_language (GtkButton * button, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		gtk_menu_popup_at_widget(pWindow->menuListLanguages, GTK_WIDGET(pWindow->sttsbrBtnLanguage),
			GDK_GRAVITY_NORTH_WEST, GDK_GRAVITY_SOUTH_EAST, NULL);
	}
}

void smpldt_clbk_btn_sttsbr_btn_insowr (GtkButton * button, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		simpledit_content_toggle_overwrite(pWindow->pEditData);
	}
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

void smpldt_clbk_menu_file_open_recent (GtkRecentChooser *pChooser, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	GFile * pFile = NULL;
	
	pFile = g_file_new_for_uri(gtk_recent_chooser_get_current_uri(pChooser));
	simpledit_app_window_open(pWindow, pFile);
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

void smpldt_clbk_menu_file_print (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (pWindow->pEditData) {
		simpledit_content_print(pWindow->pEditData);
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
		if (pMnuItemData->pSelLang != NULL) {
			simpledit_content_update_highlight(pMnuItemData->pWin->pEditData, pMnuItemData->pSelLang);
		} else {
			simpledit_content_text_highlight(pMnuItemData->pWin->pEditData);
		}
		pMnuItemData->pWin->menuSelectedLanguage = pMnuItemData->pMenuItem;
	}
}

void smpldt_clbk_menu_language_guess_item (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWindow = SIMPLEDIT_APP_WINDOW(user_data);
	const gchar * pcLanguage = NULL;
	
	if (pWindow->pEditData) {
		simpledit_content_update_highlight(pWindow->pEditData, NULL);

		pcLanguage = simpledit_content_get_language(pWindow->pEditData);
		simpledit_app_window_select_language_in_menu(SIMPLEDIT_APP_WINDOW(pWindow), pcLanguage);
	}
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

void simpledit_app_window_select_language_in_menu(SimpleditAppWindow *pWindow, const gchar * pcLanguageName) {
	sMenuLangItem * pMnuItemData = NULL;
	
	if (pcLanguageName != NULL) {
		pMnuItemData = (sMenuLangItem *)g_hash_table_lookup(pWindow->pHashLanguage, pcLanguageName);
		if (pMnuItemData != NULL) {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(pMnuItemData->pMenuItem), TRUE);
		}
	}
}

GtkMenu * simpledit_app_window_get_language_menu(SimpleditAppWindow *pWindow) {
	GtkSourceLanguageManager * pLangMngr = NULL;
	GtkSourceLanguage * pLang = NULL;
	GSList * pGrpMenu = NULL;
	GtkWidget * pMnuMain    = NULL;
	GtkWidget * pMnuSection = NULL;
	GtkWidget * pMnuItem    = NULL;
	sMenuLangItem * pMnuItemData = NULL;
	GHashTable * pHash = NULL;
	const gchar * const * arpcLangIds = NULL;
	const gchar * const * arpcPtrIds  = NULL;
	const gchar * pcCurrentId = NULL;
	const gchar * pcSection, * pcName;

	pLangMngr = gtk_source_language_manager_get_default();
	arpcLangIds = gtk_source_language_manager_get_language_ids(pLangMngr);
	
	pHash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
	pWindow->pHashLanguage = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
	arpcPtrIds = arpcLangIds;
	for (pcCurrentId = *arpcPtrIds; pcCurrentId; pcCurrentId = *++arpcPtrIds) {
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
			
			pMnuItem = gtk_radio_menu_item_new_with_label(pGrpMenu, pcName);
			pGrpMenu = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(pMnuItem));
			gtk_widget_show (pMnuItem);
			gtk_menu_shell_append (GTK_MENU_SHELL(pMnuSection), pMnuItem);
			
			pMnuItemData = g_new0(sMenuLangItem, 1);
			pMnuItemData->pWin = pWindow;
			pMnuItemData->pSelLang = pLang;
			pMnuItemData->pMenuItem = pMnuItem;
			g_signal_connect (pMnuItem, "activate", G_CALLBACK (smpldt_clbk_menu_language_item), pMnuItemData);
			g_hash_table_insert(pWindow->pHashLanguage, (gchar *)pcName, pMnuItemData);
		}
		
	}
	
	pMnuMain = gtk_menu_new();
	gtk_widget_show(pMnuMain);
	
	/* Add default/empty language */
	pMnuItem = gtk_radio_menu_item_new_with_label(pGrpMenu, _("Plain text"));
	pGrpMenu = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(pMnuItem));
	gtk_widget_show (pMnuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL(pMnuMain), pMnuItem);
	
	pMnuItemData = g_new0(sMenuLangItem, 1);
	pMnuItemData->pWin = pWindow;
	pMnuItemData->pSelLang = NULL;
	pMnuItemData->pMenuItem = pMnuItem;
	g_signal_connect (pMnuItem, "activate", G_CALLBACK (smpldt_clbk_menu_language_item), pMnuItemData);
	g_hash_table_insert(pWindow->pHashLanguage, "Text", pMnuItemData);
	
	/* Add auto select language */
	pMnuItem = gtk_menu_item_new_with_label(_("Autodetect"));
	gtk_widget_show (pMnuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL(pMnuMain), pMnuItem);
	
	g_signal_connect (pMnuItem, "activate", G_CALLBACK (smpldt_clbk_menu_language_guess_item), pWindow);
	
	/* Add separator before sections */
	pMnuItem = gtk_separator_menu_item_new();
	gtk_widget_show (pMnuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL(pMnuMain), pMnuItem);
	
	
	/* Generate sections menu */
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
