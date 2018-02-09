
#ifndef __SIMPLEDIT_CONTENT_H__
#define __SIMPLEDIT_CONTENT_H__

#include <string.h>

#include "simplEdit_main.h"

#include <glib-object.h>

G_BEGIN_DECLS

/*
 * Type declaration.
 */
#define SIMPLEDIT_TYPE_CONTENT simpledit_content_get_type ()
G_DECLARE_FINAL_TYPE (SimpleditContent, simpledit_content, SIMPLEDIT, CONTENT, GObject)

/*
 * Method definitions.
 */
SimpleditContent * simpledit_content_new (GtkBuilder * pBuilder);

gboolean simpledit_content_update_title(SimpleditContent * pEditData);
GtkWidget * simpledit_content_get_widget(SimpleditContent * pEditData, const gchar * sWidgetId);

gboolean simpledit_content_have_filename(SimpleditContent * pEditData);
gboolean simpledit_content_is_modified(SimpleditContent * pEditData);

gboolean simpledit_content_reset(SimpleditContent * pEditData);
gboolean simpledit_content_load(SimpleditContent * pEditData, const gchar * pcFilename);
gboolean simpledit_content_save(SimpleditContent * pEditData, const gchar * pcFilename);


G_END_DECLS


typedef struct {
	GtkBuilder    * pBuilder;
	GtkWidget     * pWndEdit;
	GtkSourceView * pSrcView;
	GtkTextBuffer * pTxtBuff;
	GtkSourceFile * pSrcFile;
	gchar * pcFilename;
	gchar * pcFiletitle;
	gboolean bWritable;
} SEditorData;

#endif /* __SIMPLEDIT_CONTENT_H__ */
