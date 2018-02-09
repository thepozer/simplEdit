#include "simplEdit_content.h"

struct _SimpleditContent {
	GObject parent_instance;

	/* Other members, including private data. */
	GtkBuilder    * pBuilder;
	GtkWidget     * pWndEdit;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
	GtkSourceFile * pSrcFile;
	gchar * pcFilename;
	gchar * pcFiletitle;
	gboolean bWritable;
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
	
	if (self->pBuilder) {
		self->pWndEdit = GTK_WIDGET(gtk_builder_get_object(self->pBuilder, "wndSimplEdit"));
		self->pSrcView = GTK_SOURCE_VIEW(gtk_builder_get_object(self->pBuilder, "srcView"));
		self->pTxtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->pSrcView));
	}
	
	self->pSrcFile = gtk_source_file_new();
	gtk_source_file_set_location(self->pSrcFile, NULL);
	
	gtk_text_buffer_set_text(self->pTxtBuff, "", 0);
	gtk_text_buffer_set_modified(self->pTxtBuff, FALSE);

	gtk_builder_connect_signals(self->pBuilder, self);

	gtk_widget_show_all(self->pWndEdit);
}


SimpleditContent * simpledit_content_new (GtkBuilder * pBuilder) {
	return SIMPLEDIT_CONTENT(g_object_new (SIMPLEDIT_TYPE_CONTENT, "builder", pBuilder, NULL));
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


void simpledit_content_load_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);
void simpledit_content_save_cb_async (GObject *source_object, GAsyncResult *res, gpointer user_data);

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


gboolean simpledit_content_load(SimpleditContent * pEditData, const gchar * pcFilename) {
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

gboolean simpledit_content_save(SimpleditContent * pEditData, const gchar * pcFilename) {
	GtkTextIter sIterStart, sIterEnd;
	GFile * pFile = NULL;
	GError * pErr = NULL;
	gchar * pcContent = NULL;
	gsize sContent;
	gboolean bReturnValue = FALSE;

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
	}
	
	GtkSourceFileSaver * pSrcFileSaver = NULL;
	pSrcFileSaver = gtk_source_file_saver_new(GTK_SOURCE_BUFFER(pEditData->pTxtBuff), pEditData->pSrcFile);
	
	gtk_source_file_saver_save_async(pSrcFileSaver, G_PRIORITY_DEFAULT, NULL, NULL, NULL, NULL, simpledit_content_save_cb_async, (gpointer)pEditData);

	return TRUE;
}
