
#ifndef __SIMPLEDIT_CONTENT_H__
#define __SIMPLEDIT_CONTENT_H__

#include <string.h>

#include "simplEdit_main.h"
#include "simplEdit_app.h"

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
SimpleditContent * simpledit_content_new (GtkWindow * pWindow);
gboolean simpledit_content_close (SimpleditContent * pEditData);

void simpledit_content_add_to_stack (SimpleditContent * pEditData, GtkNotebook * bookEditors);

gboolean simpledit_content_update_title(SimpleditContent * pEditData);
gchar * simpledit_content_get_status(SimpleditContent * pEditData);

gint simpledit_content_file_dialog(SimpleditContent * pEditData, GtkWidget * pDlgFile);

gboolean simpledit_content_search(SimpleditContent * pEditData, GtkSourceSearchSettings * pSearchSettings);
gboolean simpledit_content_searchall(SimpleditContent * pEditData, GtkSourceSearchSettings * pSearchSettings);

gboolean simpledit_content_get_overwrite(SimpleditContent * pEditData);
void simpledit_content_toggle_overwrite(SimpleditContent * pEditData);

gchar * simpledit_content_get_language(SimpleditContent * pEditData);

gboolean simpledit_content_have_filename(SimpleditContent * pEditData);
gboolean simpledit_content_is_modified(SimpleditContent * pEditData);

gboolean simpledit_content_set_filename(SimpleditContent * pEditData, const gchar * pcFilename);
gboolean simpledit_content_select_name(SimpleditContent * pEditData, GtkFileChooserAction action);

void simpledit_content_update_highlight(SimpleditContent * pEditData, GtkSourceLanguage * pSrcLang);
void simpledit_content_text_highlight(SimpleditContent * pEditData);

void simpledit_content_show_message(SimpleditContent * pEditData, GtkMessageType vMsgType, gchar * pcMessage);

gboolean simpledit_content_reset(SimpleditContent * pEditData);
gboolean simpledit_content_load(SimpleditContent * pEditData);
gboolean simpledit_content_save(SimpleditContent * pEditData);

gboolean simpledit_content_print(SimpleditContent * pEditData);

G_END_DECLS

#endif /* __SIMPLEDIT_CONTENT_H__ */
