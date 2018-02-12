#include "simplEdit_content.h"

struct _SimpleditContent {
	GObject parent_instance;

	/* Read/Write Data */
	GtkBuilder    * pBuilder;
	GtkWidget     * pWndEdit;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
	GtkSourceFile * pSrcFile;
	gchar * pcFilename;
	gchar * pcFiletitle;
	gboolean bWritable;
	
	/* Private Data */
	GtkWidget * pLstWidgetCharset;
	GtkWidget * pLstWidgetEndOfLines;
	GtkWidget * pLstWidgetCompress;
};

G_DEFINE_TYPE (SimpleditContent, simpledit_content, G_TYPE_OBJECT) ;

enum
{
  PROP_BUILDER = 1,
  PROP_WINDOW,
  PROP_SOURCEVIEW,
  PROP_TEXTBUFFER,
  PROP_SOURCEFILE,
  PROP_FILENAME,
  PROP_FILETITLE,
  PROP_WRITABLE,
  N_PROPERTIES
};

static GParamSpec * arObjectProperties[N_PROPERTIES] = { NULL, };

GtkWidget * simpledit_content_extra_widget(SimpleditContent * pEditData);

void simpledit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);
void simpledit_content_save_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);

static void simpledit_content_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	SimpleditContent *self = SIMPLEDIT_CONTENT(object);

	switch (property_id) {
		case PROP_BUILDER:
			self->pBuilder = GTK_BUILDER(g_value_get_object(value));
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
		case PROP_BUILDER:
			g_value_set_object(value, self->pBuilder);
			break;
		case PROP_WINDOW:
			g_value_set_object(value, self->pWndEdit);
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

	arObjectProperties[PROP_BUILDER]    = g_param_spec_object("builder", "Builder", "Builder of the application.", 
										GTK_TYPE_BUILDER, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
									
	arObjectProperties[PROP_WINDOW]     = g_param_spec_object("window", "Window", "Window of the application.", 
										GTK_TYPE_WINDOW, G_PARAM_READABLE);

	arObjectProperties[PROP_SOURCEVIEW] = g_param_spec_object("sourceview", "sourceview", "sourceview of the application.", 
										GTK_SOURCE_TYPE_VIEW, G_PARAM_READABLE);
	arObjectProperties[PROP_TEXTBUFFER] = g_param_spec_object("textbuffer", "textbuffer", "textbuffer of the sourceview.", 
										GTK_TYPE_TEXT_BUFFER, G_PARAM_READABLE);
	arObjectProperties[PROP_SOURCEFILE] = g_param_spec_object("sourcefile", "sourcefile", "sourcefile of the application.", 
										GTK_SOURCE_TYPE_FILE, G_PARAM_READABLE);
	
	arObjectProperties[PROP_FILENAME]   = g_param_spec_string("filename", "Filename", "File name of the application.", 
										NULL, G_PARAM_READWRITE);
	arObjectProperties[PROP_FILETITLE]  = g_param_spec_string("filetitle", "Filetitle", "File Title of the application.", 
										NULL, G_PARAM_READABLE);

	arObjectProperties[PROP_WRITABLE]   = g_param_spec_boolean ("writable", "Writable", "TextView is writable.", 
										TRUE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

	g_object_class_install_properties (pObjectClass, N_PROPERTIES, arObjectProperties);
	
}

static void simpledit_content_init (SimpleditContent *self) {
  /* initialize all public and private members to reasonable default values.
   * They are all automatically initialized to 0 to begin with. */
	
	self->pSrcFile = gtk_source_file_new();
	gtk_source_file_set_location(self->pSrcFile, NULL);
}


SimpleditContent * simpledit_content_new (GtkBuilder * pBuilder) {
	SimpleditContent *self = SIMPLEDIT_CONTENT(g_object_new (SIMPLEDIT_TYPE_CONTENT, "builder", pBuilder, NULL));

    self->pWndEdit = GTK_WIDGET(gtk_builder_get_object(self->pBuilder, "wndSimplEdit"));
    self->pSrcView = GTK_SOURCE_VIEW(gtk_builder_get_object(self->pBuilder, "srcView"));
    self->pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->pSrcView));
	
	gtk_text_buffer_set_text(self->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(self->pTxtBuff, FALSE);

	gtk_builder_connect_signals(self->pBuilder, self);

	gtk_widget_show_all(self->pWndEdit);
    
    return self;
}

gboolean simpledit_content_update_title(SimpleditContent * pEditData) {
	gchar * pcTitle = NULL;
	
	if (pEditData->pcFiletitle != NULL) {
		pcTitle = g_strdup_printf("simplEdit - %s", pEditData->pcFiletitle);
		gtk_window_set_title(GTK_WINDOW(pEditData->pWndEdit), pcTitle);
	} else {
		gtk_window_set_title(GTK_WINDOW(pEditData->pWndEdit), "simplEdit");
	}
	
	g_free(pcTitle);
	
	return TRUE;
}

GtkWidget * simpledit_content_get_widget(SimpleditContent * pEditData, const gchar * sWidgetId) {
	return GTK_WIDGET(gtk_builder_get_object(pEditData->pBuilder, sWidgetId));
}


gboolean simpledit_content_have_filename(SimpleditContent * pEditData) {
	return pEditData->pcFiletitle != NULL;
}

gboolean simpledit_content_is_modified(SimpleditContent * pEditData) {
	return gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(pEditData->pTxtBuff));
}

gboolean simpledit_content_reset(SimpleditContent * pEditData) {
	gtk_source_file_set_location(pEditData->pSrcFile, NULL);
	
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
		
		gtk_source_file_set_location(pEditData->pSrcFile, pFile);
        if (gtk_source_file_is_local(pEditData->pSrcFile)) {
            gtk_source_file_check_file_on_disk(pEditData->pSrcFile);
        }
		
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


GtkWidget * simpledit_content_extra_widget(SimpleditContent * pEditData) {
	GtkWidget * pHBox, * pLabel;
	GtkCellRenderer * pCellRndr = NULL;
	GtkListStore * pLstModelEOL, *pLstModelCompress, * pLstModelCharset;
	GtkTreeIter iter, * pIterSel;
    const GtkSourceEncoding * pEncod = NULL;
	GtkSourceNewlineType eTypeEOL;
    GtkSourceCompressionType eCompType;
    const gchar * pSelCharset = NULL;
    
    pEncod = gtk_source_file_get_encoding(pEditData->pSrcFile);
    if (!pEncod) {
        pEncod = gtk_source_encoding_get_utf8();
    }
    pSelCharset = gtk_source_encoding_get_charset(pEncod);
    eTypeEOL = gtk_source_file_get_newline_type(pEditData->pSrcFile);
    eCompType = gtk_source_file_get_compression_type(pEditData->pSrcFile);
//g_print("Encoding : %s (%s)\n", gtk_source_encoding_get_name(pEncod), pSelCharset);
//g_print("EOL : %d\n", eTypeEOL);
//g_print("Compress : %d\n", eCompType);
    
    
	pHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_widget_show(pHBox);
	
	pLabel = gtk_label_new("Encoding :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

	pLstModelCharset = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	GSList * pLstAllEncod = gtk_source_encoding_get_all();
	g_slist_foreach(pLstAllEncod, insertCharsetIntoCombo, (gpointer)pLstModelCharset);
	g_slist_free(pLstAllEncod);
	pEditData->pLstWidgetCharset = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelCharset));
 	pCellRndr = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(pEditData->pLstWidgetCharset), pCellRndr, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(pEditData->pLstWidgetCharset), pCellRndr, "text", 1, NULL);
	
	searchParam = (struct _searchParam){.pCombo = pEditData->pLstWidgetCharset, .txtSel = pSelCharset, .iSel = 0};
	gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelCharset), searchTextIntoCombo, &searchParam);
	
	gtk_widget_show(pEditData->pLstWidgetCharset);
	gtk_box_pack_start(GTK_BOX(pHBox), pEditData->pLstWidgetCharset, TRUE, TRUE, 1);

	pLabel = gtk_label_new("End of line :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);
	
	pLstModelEOL = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_LF,   1, "Unix (\\n)", -1);
	gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_CR_LF, 1, "Windows (\\r\\n)", -1);
	gtk_list_store_insert_with_values (pLstModelEOL, &iter, -1, 0, GTK_SOURCE_NEWLINE_TYPE_CR,   1, "Mac (\\r)", -1);
	pEditData->pLstWidgetEndOfLines = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelEOL));
 	pCellRndr = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (pEditData->pLstWidgetEndOfLines), pCellRndr, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (pEditData->pLstWidgetEndOfLines), pCellRndr, "text", 1, NULL);
	
	searchParam = (struct _searchParam){.pCombo = pEditData->pLstWidgetEndOfLines, .txtSel = NULL, .iSel = eTypeEOL};
	gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelEOL), searchIntIntoCombo, &searchParam);
	
	gtk_widget_show(pEditData->pLstWidgetEndOfLines);
	gtk_box_pack_start(GTK_BOX(pHBox), pEditData->pLstWidgetEndOfLines, TRUE, TRUE, 1);

	pLabel = gtk_label_new("Compression :");
	gtk_widget_show(pLabel);
	gtk_box_pack_start(GTK_BOX(pHBox), pLabel, TRUE, TRUE, 1);

	pLstModelCompress = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
    gtk_list_store_insert_with_values(pLstModelCompress, &iter, -1, 0, GTK_SOURCE_COMPRESSION_TYPE_NONE, 1, "None", -1);
    gtk_list_store_insert_with_values(pLstModelCompress, &iter, -1, 0, GTK_SOURCE_COMPRESSION_TYPE_GZIP, 1, "Gzip (.gz)", -1);
	pEditData->pLstWidgetCompress = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pLstModelCompress));
 	pCellRndr = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (pEditData->pLstWidgetCompress), pCellRndr, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (pEditData->pLstWidgetCompress), pCellRndr, "text", 1, NULL);

	searchParam = (struct _searchParam){.pCombo = pEditData->pLstWidgetCompress, .txtSel = NULL, .iSel = eTypeEOL};
	gtk_tree_model_foreach(GTK_TREE_MODEL(pLstModelCompress), searchIntIntoCombo, &searchParam);
	
	gtk_widget_show(pEditData->pLstWidgetCompress);
	gtk_box_pack_start(GTK_BOX(pHBox), pEditData->pLstWidgetCompress, TRUE, TRUE, 1);
    
	return pHBox;
}

gboolean simpledit_content_select_name(SimpleditContent * pEditData, GtkFileChooserAction action) {
    GtkWidget * pDlgFile = NULL;
    gint iResult = 0;
    gboolean bSelectName = FALSE;
    
    if (action == GTK_FILE_CHOOSER_ACTION_OPEN) {
        pDlgFile = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(pEditData->pWndEdit), GTK_FILE_CHOOSER_ACTION_OPEN, 
                    "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    } else if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
        pDlgFile = gtk_file_chooser_dialog_new ("Save File", GTK_WINDOW(pEditData->pWndEdit), GTK_FILE_CHOOSER_ACTION_SAVE,
                    "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);
	}
    
    if (pDlgFile) {
        if (pEditData->pcFilename) {
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(pDlgFile), pEditData->pcFilename);
        } else if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(pDlgFile), "New file");
        }
	    
	    if (action == GTK_FILE_CHOOSER_ACTION_SAVE) {
			GtkWidget * pHBox = simpledit_content_extra_widget(pEditData);
			gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(pDlgFile), pHBox);
		}
	    
        iResult = gtk_dialog_run (GTK_DIALOG (pDlgFile));

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

void simpledit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	SimpleditContent * pEditData = SIMPLEDIT_CONTENT(user_data);
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


gboolean simpledit_content_load(SimpleditContent * pEditData) {
	GtkSourceFileLoader * pSrcFileLoader = NULL;
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
		GtkWidget * pDlgMsg = gtk_message_dialog_new(GTK_WINDOW(pEditData->pWndEdit), GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
										 "Error writing '%s' : (%i) %s",
										 pEditData->pcFilename, pErr->code, pErr->message);
		gtk_dialog_run (GTK_DIALOG (pDlgMsg));
		gtk_widget_destroy (pDlgMsg);
	}
}

gboolean simpledit_content_save(SimpleditContent * pEditData) {
	GtkSourceFileSaver * pSrcFileSaver = NULL;
	pSrcFileSaver = gtk_source_file_saver_new(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile);
	
	gtk_source_file_saver_save_async(pSrcFileSaver, G_PRIORITY_DEFAULT, NULL, NULL, NULL, NULL, simpledit_content_save_cb_async, (gpointer)pEditData);

	return TRUE;
}
