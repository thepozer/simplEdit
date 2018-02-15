#include "simplEdit_window.h"

struct _SimpleditAppWindow
{
	GtkApplicationWindow parent;

	SimpleditContent * pEditData;

	GtkSourceView * pSrcView;
	
	GtkMenuItem * menuFileSave;
	GtkMenuItem * menuFileSaveAs;
	GtkMenuItem * menuFileReturnToSaved;
	
	GtkMenuItem * menuEditUndo;
	GtkMenuItem * menuEditRedo;
	GtkMenuItem * menuEditCut;
	GtkMenuItem * menuEditCopy;
};

G_DEFINE_TYPE(SimpleditAppWindow, simpledit_app_window, GTK_TYPE_APPLICATION_WINDOW);

static void simpledit_app_window_init (SimpleditAppWindow *pWin) {
	gtk_widget_init_template(GTK_WIDGET(pWin));
}

static void simpledit_app_window_class_init (SimpleditAppWindowClass *pClass) {
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/simpledit/simplEdit.glade");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, pSrcView);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSave);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileSaveAs);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuFileReturnToSaved);
	
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditUndo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditRedo);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCut);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(pClass), SimpleditAppWindow, menuEditCopy);
}

SimpleditAppWindow * simpledit_app_window_new (SimpleditApp *pApp) {
	SimpleditAppWindow * pWindow = g_object_new (SIMPLEDIT_APP_WINDOW_TYPE, "application", pApp, NULL);
  
	pWindow->pEditData = simpledit_content_new(GTK_WINDOW(pWindow), pWindow->pSrcView);
	
	simpledit_content_update_title(pWindow->pEditData);
	
	return pWindow;
}

void simpledit_app_window_open (SimpleditAppWindow *pWin, GFile *pFile) {
	gchar * pcFileName = NULL;
	
	pcFileName = g_file_get_path(pFile);
	simpledit_content_set_filename(pWin->pEditData, pcFileName);
	simpledit_content_load(pWin->pEditData);
	
	g_free(pcFileName);
}


void smpldt_clbk_menu_file (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	GtkWidget * pMenuItem = NULL;
	gboolean bModified, bHaveFilename;
	
	bModified = simpledit_content_is_modified(pWin->pEditData);
	bHaveFilename = simpledit_content_have_filename(pWin->pEditData);
	
	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuFileSave), bModified && bHaveFilename);
	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuFileSaveAs), bModified);
	gtk_widget_set_sensitive(GTK_WIDGET(pWin->menuFileReturnToSaved), bModified && bHaveFilename);
}

void smpldt_clbk_menu_file_new (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);

	simpledit_content_reset(pWin->pEditData);
	simpledit_content_update_title(pWin->pEditData);
}

void smpldt_clbk_menu_file_open (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_select_name(pWin->pEditData, GTK_FILE_CHOOSER_ACTION_OPEN)) {
		if (simpledit_content_load(pWin->pEditData)) {
			simpledit_content_update_title(pWin->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_save (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_have_filename(pWin->pEditData)) {
		simpledit_content_save(pWin->pEditData);
	}
}

void smpldt_clbk_menu_file_saveas (GtkMenuItem *menuitem, gpointer user_data) {
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_select_name(pWin->pEditData, GTK_FILE_CHOOSER_ACTION_SAVE)) {
		if (simpledit_content_save(pWin->pEditData)) {
			simpledit_content_update_title(pWin->pEditData);
		}
	}
}

void smpldt_clbk_menu_file_returntosaved (GtkMenuItem *menuitem, gpointer user_data) { 
	SimpleditAppWindow * pWin = SIMPLEDIT_APP_WINDOW(user_data);
	
	if (simpledit_content_have_filename(pWin->pEditData)) {
		simpledit_content_load(pWin->pEditData);
	}
}

void smpldt_clbk_menu_file_quit (GtkMenuItem *menuitem, gpointer user_data) {
	GtkWindow * pWin = GTK_WINDOW(user_data);

	gtk_window_close(pWin);
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


void smpldt_clbk_menu_search_find (GtkMenuItem *menuitem, gpointer user_data) {
}

void smpldt_clbk_menu_search_replace (GtkMenuItem *menuitem, gpointer user_data) {
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
