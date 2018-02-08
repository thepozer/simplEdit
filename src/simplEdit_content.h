
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
//SimpleditContent * simpledit_content_new (void);

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

gboolean simplEdit_content_init_old(SEditorData * pEditData, GtkBuilder * pBuilder);
gboolean simplEdit_content_reset(SEditorData * pEditData);
gboolean simplEdit_content_load(SEditorData * pEditData, const gchar * pcFilename);
gboolean simplEdit_content_save(SEditorData * pEditData, const gchar * pcFilename);

#endif /* __SIMPLEDIT_CONTENT_H__ */
