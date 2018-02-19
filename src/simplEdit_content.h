
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
SimpleditContent * simpledit_content_new (GtkWindow * pWindow, GtkSourceView * pSrcView);

gboolean simpledit_content_update_title(SimpleditContent * pEditData);

gint simpledit_content_file_dialog(SimpleditContent * pEditData, GtkWidget * pDlgFile);

gchar * simpledit_content_get_status(SimpleditContent * pEditData);

gboolean simpledit_content_have_filename(SimpleditContent * pEditData);
gboolean simpledit_content_is_modified(SimpleditContent * pEditData);

gboolean simpledit_content_set_filename(SimpleditContent * pEditData, const gchar * pcFilename);
gboolean simpledit_content_select_name(SimpleditContent * pEditData, GtkFileChooserAction action);

gboolean simpledit_content_update_highlight(SimpleditContent * pEditData, GtkSourceLanguage * pSrcLang);

gboolean simpledit_content_reset(SimpleditContent * pEditData);
gboolean simpledit_content_load(SimpleditContent * pEditData);
gboolean simpledit_content_save(SimpleditContent * pEditData);


G_END_DECLS

#endif /* __SIMPLEDIT_CONTENT_H__ */
