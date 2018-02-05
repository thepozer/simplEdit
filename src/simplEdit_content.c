#include "simplEdit_content.h"

void simplEdit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);

gboolean simplEdit_content_init(SEditorData * pEditData, GtkBuilder * pBuilder) {
	pEditData->pBuilder = pBuilder;
	
	pEditData->pWndEdit = GTK_WIDGET(gtk_builder_get_object(pEditData->pBuilder, "wndSimplEdit"));
	pEditData->pSrcView = GTK_SOURCE_VIEW(gtk_builder_get_object(pEditData->pBuilder, "srcView"));
	pEditData->pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pEditData->pSrcView));

	pEditData->pSrcFile = gtk_source_file_new();
	gtk_source_file_set_location(pEditData->pSrcFile, NULL);
	
	gtk_text_buffer_set_text(pEditData->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

	g_free(pEditData->pcFilename);
	pEditData->pcFilename = NULL;

	g_free(pEditData->pcFiletitle);
	pEditData->pcFiletitle = NULL;
	
	return TRUE;
}

gboolean simplEdit_content_reset(SEditorData * pEditData) {
	gtk_source_file_set_location(pEditData->pSrcFile, NULL);
	
	gtk_text_buffer_set_text(pEditData->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

	g_free(pEditData->pcFilename);
	pEditData->pcFilename = NULL;

	g_free(pEditData->pcFiletitle);
	pEditData->pcFiletitle = NULL;
	
	return TRUE;
}

void simplEdit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	SEditorData * pEditData = (SEditorData *)user_data;
	GtkSourceFileLoader * pSrcFileLoader = GTK_SOURCE_FILE_LOADER(source_object);
	GError * pErr = NULL;
	gboolean success = FALSE;

	success = gtk_source_file_loader_load_finish(pSrcFileLoader, res, &pErr);

	if (!success) {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWndEdit), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
										 "Error reading '%s' : (%i) %s",
										 pEditData->pcFilename, pErr->code, pErr->message);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	}
}


gboolean simplEdit_content_load(SEditorData * pEditData, const gchar * pcFilename) {
	GFile * pFile = NULL;
	GError * pErr = NULL;
	gchar * pcContent = NULL;
	
	if (pcFilename) {
		pFile = g_file_new_for_path(pcFilename);
		if (!pFile) {
			return FALSE;
		}
		
		gtk_source_file_set_location(pEditData->pSrcFile, pFile);
		
		g_free(pEditData->pcFilename);
		pEditData->pcFilename = g_strdup(pcFilename);
		
		g_free(pEditData->pcFiletitle);
		pEditData->pcFiletitle = g_file_get_basename(pFile);
		
		g_object_unref(pFile);
	}
	
	GtkSourceFileLoader * pSrcFileLoader = NULL;
	pSrcFileLoader = gtk_source_file_loader_new(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile);
	
	gtk_source_file_loader_load_async(pSrcFileLoader, G_PRIORITY_DEFAULT, NULL, NULL, NULL, NULL, simplEdit_content_load_cb_async, (gpointer)pEditData);

	return TRUE;
}

gboolean simplEdit_content_save(SEditorData * pEditData, const gchar * pcFilename) {
	GtkTextIter sIterStart, sIterEnd;
	GFile * pFile = NULL;
	GError * pErr = NULL;
	gchar * pcContent = NULL;
	gsize sContent;
	gboolean bReturnValue = FALSE;

	if (pcFilename) {
		pFile = g_file_new_for_path(pcFilename);
		
		g_free(pEditData->pcFilename);
		pEditData->pcFilename = g_strdup(pcFilename);
		
		g_free(pEditData->pcFiletitle);
		pEditData->pcFiletitle = g_file_get_basename(pFile);
	} else {
		pFile = g_file_new_for_path(pEditData->pcFilename);
	}
	
	if (pFile) {
		gtk_text_buffer_get_start_iter(pEditData->pTxtBuff, &sIterStart);
		gtk_text_buffer_get_end_iter(pEditData->pTxtBuff, &sIterEnd);
		pcContent = gtk_text_buffer_get_text(pEditData->pTxtBuff, &sIterStart, &sIterEnd, TRUE);
		sContent = strlen(pcContent);

//g_print("simplEdit_content_save - pcContent : \n%s\n", pcContent);
		if (!g_file_replace_contents(pFile, pcContent, sContent, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &pErr)) {
			if (pErr) {
				g_printerr("simplEdit_content_save - Error (%i) : '%s'\n", pErr->code, pErr->message);
				g_error_free (pErr);
			} else {
				bReturnValue = TRUE;
			}
		}

		g_free(pcContent);
		g_object_unref(pFile);
	}
	
	return bReturnValue;
}
