
#ifndef __SIMPLEDIT_SEARCH_DIALOG_H__
#define __SIMPLEDIT_SEARCH_DIALOG_H__

#include "simplEdit_main.h"

#define SIMPLEDIT_SEARCH_DIALOG_TYPE (simpledit_search_dialog_get_type ())
G_DECLARE_FINAL_TYPE (SimpleditSearchDialog, simpledit_search_dialog, SIMPLEDIT, SEARCH_DIALOG, GtkDialog)

SimpleditSearchDialog * simpledit_search_dialog_new(SimpleditAppWindow * pWindow);

#endif /* __SIMPLEDIT_SEARCH_DIALOG_H__ */
