
#ifndef __SMPLDT_CONTENT_H__
#define __SMPLDT_CONTENT_H__

#include <string.h>

#include "simplEdit_main.h"

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

gboolean simplEdit_content_init(SEditorData * pEditData, GtkBuilder * pBuilder);
gboolean simplEdit_content_reset(SEditorData * pEditData);
gboolean simplEdit_content_load(SEditorData * pEditData, const gchar * pcFilename);
gboolean simplEdit_content_save(SEditorData * pEditData, const gchar * pcFilename);

#endif /* __SMPLDT_CONTENT_H__ */
