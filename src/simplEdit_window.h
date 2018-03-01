
#ifndef __SIMPLEDIT_WINDOW_H__
#define __SIMPLEDIT_WINDOW_H__

#include <string.h>

#include "simplEdit_main.h"
#include "simplEdit_app.h"
#include "simplEdit_content.h"

#define SIMPLEDIT_APP_WINDOW_TYPE (simpledit_app_window_get_type ())
G_DECLARE_FINAL_TYPE (SimpleditAppWindow, simpledit_app_window, SIMPLEDIT, APP_WINDOW, GtkApplicationWindow)


SimpleditAppWindow * simpledit_app_window_new  (SimpleditApp *pApp);
void                 simpledit_app_window_open (SimpleditAppWindow * pWindow, GFile * pFile);


GtkMenu * simpledit_app_window_get_language_menu(SimpleditAppWindow *pWindow);
SimpleditContent * simpledit_app_window_get_content (SimpleditAppWindow *pWindow);
void simpledit_app_window_clear_search_dialog (SimpleditAppWindow *pWindow);

#endif /* __SIMPLEDIT_WINDOW_H__ */
