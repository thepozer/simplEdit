#include "simplEdit_content.h"

gboolean simplEdit_content_reset(SEditorData * pEditData) {
	gtk_text_buffer_set_text(pEditData->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

	g_free(pEditData->pcFilename);
	pEditData->pcFilename = NULL;

	g_free(pEditData->pcFiletitle);
	pEditData->pcFiletitle = NULL;
	
	return TRUE;
}

gboolean simplEdit_content_load(SEditorData * pEditData, const gchar * pcFilename) {
	GFile * pFile = NULL;
	GError * pErr = NULL;
	gchar * pcContent = NULL;
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
//g_print("simplEdit_content_load - pcFiletitle : '%s'\n", pEditData->pcFiletitle);
		if (g_file_load_contents (pFile, NULL, &pcContent, NULL, NULL, &pErr)) {
			if (pErr) {
				g_printerr("simplEdit_content_load - Error (%i) : '%s'\n", pErr->code, pErr->message);
				g_error_free (pErr);
			} else {
//g_print("simplEdit_content_load - pcContent : \n%s\n", pcContent);
				gtk_text_buffer_set_text(pEditData->pTxtBuff, pcContent, -1);
				gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

				g_free(pcContent);
				bReturnValue = TRUE;
			}
		}
		
		g_object_unref(pFile);
	}
	
	return bReturnValue;
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
