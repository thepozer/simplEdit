
#ifndef __SIMPLEDIT_WINDOW_H__
#define __SIMPLEDIT_WINDOW_H__

#include <string.h>

#include "simplEdit_main.h"
#include "simplEdit_app.h"
#include "simplEdit_content.h"

#define SIMPLEDIT_APP_WINDOW_TYPE (simpledit_app_window_get_type ())
G_DECLARE_FINAL_TYPE (SimpleditAppWindow, simpledit_app_window, SIMPLEDIT, APP_WINDOW, GtkApplicationWindow)


SimpleditAppWindow * simpledit_app_window_new       (SimpleditApp * pApp);
void                 simpledit_app_window_open      (SimpleditAppWindow * pWindow, GFile * pFile);
gboolean             simpledit_app_window_close_all (SimpleditAppWindow * pWindow);

GtkMenu          * simpledit_app_window_get_language_menu   (SimpleditAppWindow * pWindow);
SimpleditContent * simpledit_app_window_get_content         (SimpleditAppWindow * pWindow);
void               simpledit_app_window_clear_search_dialog (SimpleditAppWindow * pWindow);
void               simpledit_app_window_clean_status        (SimpleditAppWindow * pWindow);

void smpldt_clbk_text_changed            (GtkTextBuffer * textbuffer, gpointer user_data);
void smpldt_clbk_cursor_position_changed (GtkTextBuffer * textbuffer, GParamSpec * pspec, gpointer user_data);
void smpldt_clbk_mark_set                (GtkTextBuffer * textbuffer, GtkTextIter * location, GtkTextMark * mark, gpointer user_data);

#endif /* __SIMPLEDIT_WINDOW_H__ */
