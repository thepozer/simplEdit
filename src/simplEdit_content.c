#include "simplEdit_content.h"

struct _SimpleditContent {
	GObject parent_instance;

	/* Read/Write Data */
	GtkWindow     * pWindow;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
	GtkSourceFile * pSrcFile;
	gchar * pcFilename;
	gchar * pcFiletitle;
	gboolean bWritable;
	
	GtkSourceLanguage * pSrcLang;
	const gchar * pcLanguage;
	
	/* Private Data */
    const GtkSourceEncoding * pEncod;
    const GtkSourceEncoding * pForcedEncod;
	GtkSourceNewlineType eTypeEOL;
    GtkSourceCompressionType eCompType;
};

G_DEFINE_TYPE (SimpleditContent, simpledit_content, G_TYPE_OBJECT) ;

enum
{
  PROP_WINDOW = 1,
  PROP_SOURCEVIEW,
  PROP_TEXTBUFFER,
  PROP_SOURCEFILE,
  PROP_FILENAME,
  PROP_FILETITLE,
  PROP_WRITABLE,
  N_PROPERTIES
};

static GParamSpec * arObjectProperties[N_PROPERTIES] = { NULL, };

gint simpledit_content_file_dialog(SimpleditContent * pEditData, GtkWidget * pDlgFile, gboolean bReadOnly);

void simpledit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);
void simpledit_content_save_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);

static void simpledit_content_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	SimpleditContent *self = SIMPLEDIT_CONTENT(object);

	switch (property_id) {
		case PROP_WINDOW:
			self->pWindow = GTK_WINDOW(g_value_get_object(value));
			break;
		case PROP_SOURCEVIEW:
			self->pSrcView = GTK_SOURCE_VIEW(g_value_get_object(value));
			break;
		case PROP_FILENAME:
			g_free(self->pcFilename);
			self->pcFilename = g_value_dup_string(value);
			break;
		case PROP_WRITABLE:
			self->bWritable = g_value_get_boolean(value);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void simpledit_content_get_property (GObject * object, guint property_id, GValue * value, GParamSpec *pspec) {
	SimpleditContent *self = SIMPLEDIT_CONTENT(object);

	switch (property_id) {
		case PROP_WINDOW:
			g_value_set_object(value, self->pWindow);
			break;
		case PROP_SOURCEVIEW:
			g_value_set_object(value, self->pSrcView);
			break;
		case PROP_TEXTBUFFER:
			g_value_set_object(value, self->pTxtBuff);
			break;
		case PROP_SOURCEFILE:
			g_value_set_object(value, self->pSrcFile);
			break;
		case PROP_FILENAME:
			g_value_set_string(value, self->pcFilename);
			break;
		case PROP_FILETITLE:
			g_value_set_string(value, self->pcFiletitle);
			break;
		case PROP_WRITABLE:
			g_value_set_boolean(value, self->bWritable);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void simpledit_content_class_init (SimpleditContentClass *klass) {
	GObjectClass * pObjectClass = G_OBJECT_CLASS (klass);

	pObjectClass->set_property = simpledit_content_set_property;
	pObjectClass->get_property = simpledit_content_get_property;

	arObjectProperties[PROP_WINDOW]     = g_param_spec_object("window", "window", "window of the application.", 
										GTK_TYPE_WINDOW, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
	arObjectProperties[PROP_SOURCEVIEW] = g_param_spec_object("sourceview", "sourceview", "sourceview of the application.", 
										GTK_SOURCE_TYPE_VIEW, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
	arObjectProperties[PROP_TEXTBUFFER] = g_param_spec_object("textbuffer", "textbuffer", "textbuffer of the sourceview.", 
										GTK_TYPE_TEXT_BUFFER, G_PARAM_READABLE);
	arObjectProperties[PROP_SOURCEFILE] = g_param_spec_object("sourcefile", "sourcefile", "sourcefile of the application.", 
										GTK_SOURCE_TYPE_FILE, G_PARAM_READABLE);
	
	arObjectProperties[PROP_FILENAME]   = g_param_spec_string("filename", "Filename", "File name of the application.", 
										NULL, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
	arObjectProperties[PROP_FILETITLE]  = g_param_spec_string("filetitle", "Filetitle", "File Title of the application.", 
										NULL, G_PARAM_READABLE);

	arObjectProperties[PROP_WRITABLE]   = g_param_spec_boolean ("writable", "Writable", "TextView is writable.", 
										TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	g_object_class_install_properties (pObjectClass, N_PROPERTIES, arObjectProperties);
	
}

static void simpledit_content_init (SimpleditContent * pEditData) {
  /* initialize all public and private members to reasonable default values.
   * They are all automatically initialized to 0 to begin with. */
	
	pEditData->pSrcFile = gtk_source_file_new();
	gtk_source_file_set_location(pEditData->pSrcFile, NULL);
	
	pEditData->pEncod = NULL;
	pEditData->pForcedEncod = NULL;
	pEditData->eTypeEOL = GTK_SOURCE_NEWLINE_TYPE_DEFAULT;
	pEditData->eCompType = GTK_SOURCE_COMPRESSION_TYPE_NONE;
}


SimpleditContent * simpledit_content_new (GtkWindow * pWindow, GtkSourceView * pSrcView) {
	SimpleditContent * pEditData = SIMPLEDIT_CONTENT(g_object_new (SIMPLEDIT_TYPE_CONTENT, 
		"window", pWindow, "sourceview", pSrcView, NULL));

    pEditData->pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pEditData->pSrcView));
    pEditData->pSrcLang = NULL;
	
	gtk_text_buffer_set_text(pEditData->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

    return pEditData;
}

gboolean simpledit_content_update_title(SimpleditContent * pEditData) {
	GString * pStrTitle = NULL;
	
	pStrTitle = g_string_new("simplEdit");
	
	if (pEditData->pcFiletitle != NULL) {
		g_string_append(pStrTitle, " - ");
		g_string_append(pStrTitle, pEditData->pcFiletitle);
	}	
	
	if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(pEditData->pTxtBuff))) {
		g_string_append(pStrTitle, " *");
	}
	
	gtk_window_set_title(GTK_WINDOW(pEditData->pWindow), pStrTitle->str);
	g_string_free(pStrTitle, TRUE);
	
	return TRUE;
}


gboolean simpledit_content_have_filename(SimpleditContent * pEditData) {
	return pEditData->pcFiletitle != NULL;
}

gboolean simpledit_content_is_modified(SimpleditContent * pEditData) {
	return gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(pEditData->pTxtBuff));
}

gchar * simpledit_content_get_status(SimpleditContent * pEditData) {
	GtkTextIter sIter;
	GString * pStrStatus = g_string_new("");
	gint iTotalNbLine = 0, iLine = 0, iCol = 0, iPos = 0;
	
	g_object_get(GTK_TEXT_BUFFER(pEditData->pTxtBuff), "cursor-position", &iPos, NULL);
	gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(pEditData->pTxtBuff), &sIter, iPos);
	iLine = gtk_text_iter_get_line(&sIter) + 1;
	iCol = gtk_text_iter_get_line_offset(&sIter) + 1;
	
	iTotalNbLine = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(pEditData->pTxtBuff));
		
	g_string_append_printf(pStrStatus, "Ligne : %d / %d \tCol : %d", iLine, iTotalNbLine, iCol);
	
	g_string_append_printf(pStrStatus, "\tFile type : %s", pEditData->pcLanguage);

	return g_string_free(pStrStatus, FALSE);
}

gboolean simpledit_content_reset(SimpleditContent * pEditData) {
	gtk_source_file_set_location(pEditData->pSrcFile, NULL);
	g_object_unref(pEditData->pSrcFile);
	pEditData->pSrcFile = gtk_source_file_new();

	
	pEditData->pEncod = NULL;
	pEditData->pForcedEncod = NULL;
	pEditData->eTypeEOL = GTK_SOURCE_NEWLINE_TYPE_DEFAULT;
	pEditData->eCompType = GTK_SOURCE_COMPRESSION_TYPE_NONE;
	
	gtk_text_buffer_set_text(pEditData->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(pEditData->pTxtBuff, FALSE);

	g_free(pEditData->pcFilename);
	pEditData->pcFilename = NULL;

	g_free(pEditData->pcFiletitle);
	pEditData->pcFiletitle = NULL;
	
	return TRUE;
}

gboolean simpledit_content_set_filename(SimpleditContent * pEditData, const gchar * pcFilename) {
	GFile * pFile = NULL;
    
	if (pcFilename) {
		pFile = g_file_new_for_path(pcFilename);
		if (!pFile) {
			return FALSE;
		}
		
		g_object_unref(pEditData->pSrcFile);
		pEditData->pSrcFile = gtk_source_file_new();
		gtk_source_file_set_location(pEditData->pSrcFile, pFile);
        if (gtk_source_file_is_local(pEditData->pSrcFile)) {
            gtk_source_file_check_file_on_disk(pEditData->pSrcFile);
        }
		
		const GtkSourceEncoding * pEncod = gtk_source_file_get_encoding(pEditData->pSrcFile);
		if (pEncod != NULL) {
			g_print("SetFilename - Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEncod), gtk_source_encoding_get_charset(pEncod));
		} else {
			g_print("SetFilename - Encoding : %s (%s)\n", NULL, NULL);
		}
		g_print("SetFilename - EOL : %d\n", gtk_source_file_get_newline_type(pEditData->pSrcFile));
		g_print("SetFilename - Compress : %d\n", gtk_source_file_get_compression_type(pEditData->pSrcFile));
		
		g_free(pEditData->pcFilename);
		pEditData->pcFilename = g_strdup(pcFilename);
		
		g_free(pEditData->pcFiletitle);
		pEditData->pcFiletitle = g_file_get_basename(pFile);
		
		g_object_unref(pFile);
        
        return TRUE;
	}
    
    return FALSE;
}

void insertCharsetIntoCombo(gpointer data, gpointer user_data) {
	GtkSourceEncoding * pCurrEncod = (GtkSourceEncoding *)data;
	GtkListStore *pLstModel = GTK_LIST_STORE(user_data);
	
	const gchar * pcCharset = gtk_source_encoding_get_charset(pCurrEncod);
	gchar * pcName = g_strdup_printf("%s (%s)", gtk_source_encoding_get_name(pCurrEncod), pcCharset);
			
//g_print("Adding to encoding list : %s\n", pcName);
	
	gtk_list_store_insert_with_values (pLstModel, NULL, -1, 0, pcCharset, 1, pcName, -1);
	
	g_free(pcName);
}

struct _searchParam {
	GtkWidget * pCombo;
	const gchar * txtSel;
	gint iSel;
} searchParam;


gboolean searchTextIntoCombo (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	struct _searchParam * pSrchParam = (struct _searchParam *)data;
	gchar * pCurrCharset;
	
	gtk_tree_model_get(model, iter, 0, &pCurrCharset, -1);
	
	if (g_strcmp0(pCurrCharset, pSrchParam->txtSel) == 0) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(pSrchParam->pCombo), iter);
		return TRUE;
	}
	
	return FALSE;
}

gboolean searchIntIntoCombo (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	struct _searchParam * pSrchParam = (struct _searchParam *)data;
	gint iCurrValue;
	
	gtk_tree_model_get(model, iter, 0, &iCurrValue, -1);
	
	if (iCurrValue == pSrchParam->iSel) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(pSrchParam->pCombo), iter);
		return TRUE;
	}
	
	return FALSE;
}


gint simpledit_content_file_dialog(SimpleditContent * pEditData, GtkWidget * pDlgFile, gboolean bReadOnly) {
	GtkWidget  * pHBox, * pLabel, * pLstWidgetCharset, * pLstWidgetEndOfLines, * pLstWidgetCompress;
	GtkCellRenderer * pCellRndr = NULL;
	GtkListStore * pLstModelEOL, *pLstModelCompress, * pLstModelCharset;
	GtkTreeIter iter, * pIterSel;
	GValue vRetVal = G_VALUE_INIT;
    const gchar * pSelCharset = NULL;
	gint iResult = GTK_RESPONSE_CANCEL;
	
    if (!pDlgFile) {
		return GTK_RESPONSE_CANCEL;
	}

	if (pEditData->pEncod != NULL) {
		pSelCharset = gtk_source_encoding_get_charset(pEditData->pEncod);
g_print("Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEditData->pEncod), pSelCharset);
	} else {
g_print("Encoding : %s (%s)\n", NULL, NULL);	
	}
g_print("EOL : %d\n", pEditData->eTypeEOL);
g_print("Compress : %d\n", pEditData->eCompType);
    
    
	pHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_widget_show(pHBox);
	
	pLabel = gtk_label_new("Encoding :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

	pLstModelCharset = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	GSList * pLstAllEncod = gtk_source_encoding_get_all();
	gtk_list_store_insert_with_values(GTK_LIST_STORE(pLstModelCharset), NULL, -1, 0, NULL, 1, "Detect from file", -1);
	g_slist_foreach(pLstAllEncod, insertCharsetIntoCombo, (gpointer)pLstModelCharset);
	g_slist_free(pLstAllEncod);
	pLstWidgetCharset = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelCharset));
 	pCellRndr = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(pLstWidgetCharset), pCellRndr, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(pLstWidgetCharset), pCellRndr, "text", 1, NULL);
	
	if (bReadOnly) {
		searchParam = (struct _searchParam){.pCombo = pLstWidgetCharset, .txtSel = NULL, .iSel = 0};
	} else {
		searchParam = (struct _searchParam){.pCombo = pLstWidgetCharset, .txtSel = pSelCharset, .iSel = 0};
	}
	gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelCharset), searchTextIntoCombo, &searchParam);
	
	gtk_widget_show(pLstWidgetCharset);
	gtk_box_pack_start(GTK_BOX(pHBox), pLstWidgetCharset, TRUE, TRUE, 1);

	if (!bReadOnly) {
		pLabel = gtk_label_new("End of line :");
		gtk_widget_show(pLabel);
		gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);
		
		pLstModelEOL = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
		gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_LF,   1, "Unix (LF)", -1);
		gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_CR_LF, 1, "Windows (CR LF)", -1);
		gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_CR,   1, "Mac (CR)", -1);
		pLstWidgetEndOfLines = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelEOL));
		pCellRndr = gtk_cell_renderer_text_new ();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (pLstWidgetEndOfLines), pCellRndr, FALSE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (pLstWidgetEndOfLines), pCellRndr, "text", 1, NULL);
		
		searchParam = (struct _searchParam){.pCombo = pLstWidgetEndOfLines, .txtSel = NULL, .iSel = pEditData->eTypeEOL};
		gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelEOL), searchIntIntoCombo, &searchParam);
		
		gtk_widget_show(pLstWidgetEndOfLines);
		gtk_box_pack_start(GTK_BOX(pHBox), pLstWidgetEndOfLines, TRUE, TRUE, 1);

		pLabel = gtk_label_new("Compression :");
		gtk_widget_show(pLabel);
		gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

		pLstModelCompress = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
		gtk_list_store_insert_with_values(pLstModelCompress, &iter, -1, 0, GTK_SOURCE_COMPRESSION_TYPE_NONE, 1, "None", -1);
		gtk_list_store_insert_with_values(pLstModelCompress, &iter, -1, 0, GTK_SOURCE_COMPRESSION_TYPE_GZIP, 1, "Gzip (.gz)", -1);
		pLstWidgetCompress = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelCompress));
		pCellRndr = gtk_cell_renderer_text_new ();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (pLstWidgetCompress), pCellRndr, FALSE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (pLstWidgetCompress), pCellRndr, "text", 1, NULL);

		searchParam = (struct _searchParam){.pCombo = pLstWidgetCompress, .txtSel = NULL, .iSel = pEditData->eCompType};
		gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelCompress), searchIntIntoCombo, &searchParam);
		
		gtk_widget_show(pLstWidgetCompress);
		gtk_box_pack_start(GTK_BOX(pHBox), pLstWidgetCompress, TRUE, TRUE, 1);
	}    

	gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(pDlgFile), pHBox);
	
	iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));
	
	if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(pLstWidgetCharset), &iter)) {
		gtk_tree_model_get_value(GTK_TREE_MODEL(pLstModelCharset), &iter, 0, &vRetVal);
		const gchar * pCharset = g_value_get_string(&vRetVal);
		if (pCharset) {
g_print("Selected - Encoding - Have charset ...\n");
			pEditData->pEncod = gtk_source_encoding_get_from_charset(pCharset);
g_print("Selected - Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEditData->pEncod), gtk_source_encoding_get_charset(pEditData->pEncod));
		} else {
g_print("Selected - Encoding - Have NO charset !!!\n");
			pEditData->pEncod = NULL;
		}
		g_value_unset(&vRetVal);
	}
	
	if (!bReadOnly && gtk_combo_box_get_active_iter(GTK_COMBO_BOX(pLstWidgetEndOfLines), &iter)) {
		gtk_tree_model_get_value(GTK_TREE_MODEL(pLstModelEOL), &iter, 0, &vRetVal);
		pEditData->eTypeEOL = g_value_get_int(&vRetVal);
		g_value_unset(&vRetVal);
		
g_print("Selected - EOL : %d\n", pEditData->eTypeEOL);
	}
	
	if (!bReadOnly && gtk_combo_box_get_active_iter(GTK_COMBO_BOX(pLstWidgetCompress), &iter)) {
		gtk_tree_model_get_value(GTK_TREE_MODEL(pLstModelCompress), &iter, 0, &vRetVal);
		pEditData->eCompType = g_value_get_int(&vRetVal);
		g_value_unset(&vRetVal);
		
g_print("Selected - Compress : %d\n", pEditData->eCompType);
	}
	return iResult;
}

gboolean simpledit_content_select_name(SimpleditContent * pEditData, GtkFileChooserAction action) {
    GtkWidget * pDlgFile = NULL;
    gint iResult = 0;
    gboolean bSelectName = FALSE;
    
    if (action == GTK_FILE_CHOOSER_ACTION_OPEN) {
        pDlgFile = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(pEditData->pWindow), GTK_FILE_CHOOSER_ACTION_OPEN, 
                    "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    } else if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
        pDlgFile = gtk_file_chooser_dialog_new ("Save File", GTK_WINDOW(pEditData->pWindow), GTK_FILE_CHOOSER_ACTION_SAVE,
                    "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);
	}
    
    if (pDlgFile) {
        if (pEditData->pcFilename) {
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), pEditData->pcFilename);
        } else if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(pDlgFile), "New file");
        }
        
        iResult = simpledit_content_file_dialog(pEditData, pDlgFile, (action == GTK_FILE_CHOOSER_ACTION_OPEN));
	    
        if (iResult == GTK_RESPONSE_ACCEPT) {
            GtkFileChooser * pChooser = GTK_FILE_CHOOSER (pDlgFile);
            gchar * pcNewFilename = gtk_file_chooser_get_filename (pChooser);
		
            simpledit_content_set_filename(pEditData, pcNewFilename);
            
            g_free(pcNewFilename);
            bSelectName = TRUE;
        }

        gtk_widget_destroy (pDlgFile);
    }
    
    return bSelectName;
}

gboolean simpledit_content_update_highlight(SimpleditContent * pEditData, GtkSourceLanguage * pSrcLang) {
	GtkSourceLanguageManager * prcLangMngr = gtk_source_language_manager_get_default();
	GtkSourceLanguage * pCurrentSrcLang = pSrcLang;
	gchar * pcContentType = NULL;
	gboolean bResultUncertain = FALSE;
	
	if (!pCurrentSrcLang && pEditData->pcFilename) {
		pcContentType = g_content_type_guess (pEditData->pcFilename, NULL, 0, &bResultUncertain);
		if (bResultUncertain) {
			g_free (pcContentType);
			pcContentType = NULL;
		}
		
		pCurrentSrcLang = gtk_source_language_manager_guess_language (prcLangMngr, pEditData->pcFilename, pcContentType);
		g_free (pcContentType);
	}
	
	pEditData->pSrcLang = pCurrentSrcLang;
	gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pCurrentSrcLang);
	
	if (pEditData->pSrcLang) {
		pEditData->pcLanguage = gtk_source_language_get_name(pEditData->pSrcLang);
	} else {
		pEditData->pcLanguage = g_strdup("Text");
	}
}

void simpledit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	SimpleditContent * pEditData = SIMPLEDIT_CONTENT(user_data);
	GtkSourceFileLoader * pSrcFileLoader = GTK_SOURCE_FILE_LOADER(source_object);
	GError * pErr = NULL;
	gboolean success = FALSE;

	success = gtk_source_file_loader_load_finish(pSrcFileLoader, res, &pErr);

	if (!success) {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
										 "Error reading '%s' : (%i) %s",
										 pEditData->pcFilename, pErr->code, pErr->message);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	} else {
		if (pEditData->pForcedEncod) {
			pEditData->pEncod = pEditData->pForcedEncod;
			pEditData->pForcedEncod = NULL;
		} else {
			pEditData->pEncod = gtk_source_file_get_encoding(pEditData->pSrcFile);
		}
		
		if (!pEditData->pEncod) {
			pEditData->pEncod = gtk_source_encoding_get_utf8();
		}
		
		pEditData->eTypeEOL = gtk_source_file_get_newline_type(pEditData->pSrcFile);
		pEditData->eCompType = gtk_source_file_get_compression_type(pEditData->pSrcFile);
g_print("load_cb_async - Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEditData->pEncod), gtk_source_encoding_get_charset(pEditData->pEncod));
g_print("load_cb_async - EOL : %d\n", pEditData->eTypeEOL);
g_print("load_cb_async - Compress : %d\n", pEditData->eCompType);
		
		g_signal_emit_by_name(pEditData->pTxtBuff, "changed", pEditData->pWindow);
		simpledit_content_update_highlight(pEditData, NULL);
	}
}


gboolean simpledit_content_load(SimpleditContent * pEditData) {
	GtkSourceFileLoader * pSrcFileLoader = NULL;
	GInputStream * pInStream = NULL;
	GError * pErr = NULL;
	
/*
	pInStream = G_INPUT_STREAM(g_file_read(gtk_source_file_get_location(pEditData->pSrcFile), NULL, &pErr));
	if (pErr) {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
										 "Error reading (stream) '%s' : (%i) %s",
										 pEditData->pcFilename, pErr->code, pErr->message);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
		
		return FALSE;
	}
	
	if (pEditData->pEncod != NULL && pEditData->pEncod != gtk_source_encoding_get_utf8()) {
		GCharsetConverter * pCharsetConv = NULL;
		
g_print("Forced - Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEditData->pEncod), gtk_source_encoding_get_charset(pEditData->pEncod));
		pCharsetConv = g_charset_converter_new ("UTF-8", gtk_source_encoding_get_charset(pEditData->pEncod), &pErr);
		if (pErr) {
			GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
											 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
											 "Error reading (Charset conv) '%s' : (%i) %s",
											 pEditData->pcFilename, pErr->code, pErr->message);
			gtk_dialog_run (GTK_DIALOG (pDlgMsg));
			gtk_widget_destroy (pDlgMsg);
			
			return FALSE;
		}
		
		pInStream = g_converter_input_stream_new(G_INPUT_STREAM(pInStream), G_CONVERTER(pCharsetConv));
	}
	
	pSrcFileLoader = gtk_source_file_loader_new_from_stream(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile, G_INPUT_STREAM(pInStream));
*/
	pSrcFileLoader = gtk_source_file_loader_new(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile);
	
	gtk_source_file_loader_load_async(pSrcFileLoader, G_PRIORITY_DEFAULT, NULL, NULL, NULL, NULL, simpledit_content_load_cb_async, (gpointer)pEditData);

	return TRUE;
}

void simpledit_content_save_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	SimpleditContent * pEditData = SIMPLEDIT_CONTENT(user_data);
	GtkSourceFileSaver * pSrcFileSaver = GTK_SOURCE_FILE_SAVER(source_object);
	GError * pErr = NULL;
	gboolean success = FALSE;

	success = gtk_source_file_saver_save_finish(pSrcFileSaver, res, &pErr);

	if (!success) {
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
										 "Error writing '%s' : (%i) %s",
										 pEditData->pcFilename, pErr->code, pErr->message);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	} else {
		pEditData->pEncod = gtk_source_file_get_encoding(pEditData->pSrcFile);
		if (!pEditData->pEncod) {
			pEditData->pEncod = gtk_source_encoding_get_utf8();
		}
		
		pEditData->eTypeEOL = gtk_source_file_get_newline_type(pEditData->pSrcFile);
		pEditData->eCompType = gtk_source_file_get_compression_type(pEditData->pSrcFile);
g_print("save_cb_async - Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEditData->pEncod), gtk_source_encoding_get_charset(pEditData->pEncod));
g_print("save_cb_async - EOL : %d\n", pEditData->eTypeEOL);
g_print("save_cb_async - Compress : %d\n", pEditData->eCompType);
		
		g_signal_emit_by_name(pEditData->pTxtBuff, "changed", pEditData->pWindow);
		simpledit_content_update_highlight(pEditData, NULL);
	}
}

gboolean simpledit_content_save(SimpleditContent * pEditData) {
	GtkSourceFileSaver * pSrcFileSaver = NULL;
	pSrcFileSaver = gtk_source_file_saver_new(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile);
	
	gtk_source_file_saver_set_encoding(pSrcFileSaver, pEditData->pEncod);
	gtk_source_file_saver_set_newline_type(pSrcFileSaver, pEditData->eTypeEOL);
	gtk_source_file_saver_set_compression_type(pSrcFileSaver, pEditData->eCompType);
	
	gtk_source_file_saver_save_async(pSrcFileSaver, G_PRIORITY_DEFAULT, NULL, NULL, NULL, NULL, simpledit_content_save_cb_async, (gpointer)pEditData);

	return TRUE;
}
